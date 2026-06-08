// DuinDasHost LSP proxy (M4).
//
// A thin, editor-agnostic Language Server that editors launch over stdio:
//   daslang.exe proxy.das
// It speaks LSP (Content-Length-framed JSON-RPC) to the editor and forwards requests
// over WebSocket to the per-project DuinDasHost daemon (--serve-ws), mapping results
// back to LSP responses/notifications. The warm Script lives in the C++ daemon; this
// proxy is stateless translation.
//
// Lifecycle: the proxy only CONNECTS to an already-running daemon (reads the project's
// <project>.das_project.ddh, connects to its wsUrl). Spawning the daemon is the editor
// plugin's job (daslang has no detached-spawn primitive). If no live daemon is found, the
// proxy reports it via window/showMessage so the plugin can spawn one.
//
// M3: initialize / initialized / shutdown / exit, textDocument/didOpen + didSave +
//     didChange -> compile -> publishDiagnostics, workspace/executeCommand (refreshBindings).
// M4: textDocument/definition, references, hover, formatting; didChange debounce.

options gen2
options persistent_heap
options gc

require dashv/dashv_boost public
require daslib/json public
require daslib/json_boost public
require daslib/fio
require daslib/strings_boost
require daslib/das_source_formatter
require strings
require math

// ===== stdio LSP framing =====
// LSP frames each message as:  Content-Length: <N>\r\n\r\n<N bytes of JSON>

var g_stdin : file
var g_stdout : file

// ===== Debug logging =====
// nvim swallows the proxy's stderr, so trace to a file next to the proxy script.
var g_log : file
var g_log_open = false

def plog(s : string) : void {
    if (!g_log_open) return
    fprint(g_log, "{s}\n")
    fflush(g_log)
}

// Reads one header line (terminated by \n; trailing \r stripped). ok=false on EOF.
def read_header_line(var ok : bool&) : string {
    let chunk = fgets(g_stdin)
    if (empty(chunk)) { ok = false; return ""; }
    ok = true
    var len = length(chunk)
    // strip trailing \n and optional \r
    if (len > 0 && character_at(chunk, len - 1) == '\n') { len--; }
    if (len > 0 && character_at(chunk, len - 1) == '\r') { len--; }
    return slice(chunk, 0, len)
}

// Reads exactly n bytes of body from stdin.
def read_body(n : int) : string {
    var bytes : array<uint8>
    fload(g_stdin, n) <| $(data) {
        for (b in data) {
            bytes |> push(b)
        }
    }
    let s = string(bytes)
    delete bytes
    return s
}

// Parses an int out of the value after "Content-Length:", tolerating surrounding spaces.
def parse_content_length(line : string) : int {
    // line is "Content-Length: 123"; take everything after the colon.
    let n = length(line)
    var i = 0
    // find ':'
    while (i < n && character_at(line, i) != ':') { i++; }
    i++ // skip ':'
    // skip spaces
    while (i < n && character_at(line, i) == ' ') { i++; }
    return to_int(slice(line, i, n))
}

// Reads one full LSP message body. eof=true at end of stream; "" on malformed framing.
def read_lsp_message(var eof : bool&) : string {
    eof = false
    var contentLength = -1
    while (true) {
        var ok = false
        let line = read_header_line(ok)
        if (!ok) { eof = true; return ""; }
        if (empty(line)) break  // blank line ends headers
        if (starts_with(line, "Content-Length:")) {
            contentLength = parse_content_length(line)
        }
    }
    if (contentLength <= 0) return ""
    return read_body(contentLength)
}

def write_lsp_message(payload : string) : void {
    // IMPORTANT: stdout is in text mode on Windows, so every '\n' written is translated to
    // '\r\n' by the CRT. If we wrote literal "\r\n" it would become "\r\r\n" and nvim's LSP
    // reader rejects the malformed header (the bug that prevented attachment). So we write
    // bare '\n' for the CRLF terminators and let the text-mode translation produce "\r\n".
    fprint(g_stdout, "Content-Length: {length(payload)}\n\n")
    fprint(g_stdout, payload)
    fflush(g_stdout)
}

// ===== JSON helpers =====
// Field access: jv ?["key"] -> JsonValue?. Scalar via ?? default. Arrays via `as _array`.

def jstr(v : JsonValue? const; key : string; defv : string = "") : string {
    return v ?[key] ?? defv
}
def jint(v : JsonValue? const; key : string; defv : int = 0) : int {
    return v ?[key] ?? defv
}

// Encodes a string as a JSON string literal (quotes + escapes).
def json_string(s : string) : string {
    return write_json(JV(s))
}

// ===== Daemon WebSocket client =====

class DaemonClient : HvWebSocketClient {
    reply : string
    got_reply : bool = false
    connected : bool = false

    def override onOpen {
        connected = true
    }
    def override onClose {
        connected = false
    }
    def override onMessage(msg : string#) {
        reply = string(msg)
        got_reply = true
    }
}

var g_daemon : DaemonClient?
var g_project_path : string
var g_ws_url : string

// ===== didChange debounce =====
// Stores the most recent un-fired didChange so rapid keystrokes collapse into one compile.
// Fired on the next non-didChange LSP message (cooperative debounce — no background timer).
var g_pending_uri : string
var g_pending_content : string
var g_has_pending = false

// Reads the wsUrl from a project's .ddh sidecar ("" if missing / not a ws daemon).
def read_ddh_wsurl(project_path : string) : string {
    let ddh = "{project_path}.ddh"
    if (!stat(ddh).is_reg) return ""
    let text = fread(ddh)
    var err : string
    let jv = read_json(text, err)
    if (jv == null) return ""
    if (jstr(jv, "transport") != "ws") return ""
    return jstr(jv, "wsUrl")
}

def connect_daemon(project_path : string) : bool {
    g_ws_url = read_ddh_wsurl(project_path)
    if (empty(g_ws_url)) return false

    g_daemon = new DaemonClient()
    if (g_daemon->init(g_ws_url) != 0) return false

    var elapsed = 0
    while (!g_daemon.connected && elapsed < 2000) {
        g_daemon->process_event_que()
        sleep(10u)
        elapsed += 10
    }
    return g_daemon.connected
}

// Sends a compile request for `file_path` (native path), returns the reply JSON ("" on fail).
// Pass refresh=true to trigger RefreshBindings on the daemon (re-reads engine .das from disk).
// Pass content (non-empty) to compile unsaved buffer text instead of reading from disk.
def daemon_compile(file_path : string; refresh : bool = false; content : string = "") : string {
    if (g_daemon == null || !g_daemon.connected) return ""
    g_daemon.got_reply = false
    g_daemon.reply = ""
    let refresh_field = refresh ? ",\"refresh\":true" : ""
    let content_field = empty(content) ? "" : ",\"content\":{json_string(content)}"
    let req = "\{\"file\":{json_string(file_path)},\"project\":{json_string(g_project_path)}{refresh_field}{content_field}}"
    g_daemon->send(req)

    var elapsed = 0
    while (!g_daemon.got_reply && elapsed < 10000) {
        g_daemon->process_event_que()
        sleep(5u)
        elapsed += 5
    }
    return g_daemon.got_reply ? g_daemon.reply : ""
}

// Sends an arbitrary JSON request to the daemon and returns the reply ("" on fail/timeout).
// Used for action requests (goto_definition, type_of, find_references).
def daemon_query(req_json : string) : string {
    if (g_daemon == null || !g_daemon.connected) { return ""; }
    g_daemon.got_reply = false
    g_daemon.reply = ""
    g_daemon->send(req_json)
    var elapsed = 0
    while (!g_daemon.got_reply && elapsed < 10000) {
        g_daemon->process_event_que()
        sleep(5u)
        elapsed += 5
    }
    return g_daemon.got_reply ? g_daemon.reply : ""
}

// ===== URI <-> path =====

def uri_to_path(uri : string) : string {
    var p = uri
    if (starts_with(p, "file://")) {
        p = slice(p, 7, length(p))
    }
    // Windows: file:///C:/x -> /C:/x ; strip the leading slash before a drive letter.
    if (length(p) >= 3 && character_at(p, 0) == '/' && character_at(p, 2) == ':') {
        p = slice(p, 1, length(p))
    }
    return p
}

// Converts a native path back to a file:// URI (Windows-aware).
def path_to_uri(p : string) : string {
    let fwd = replace(p, "\\", "/")
    // Windows absolute path: C:/... -> file:///C:/...
    if (length(fwd) >= 2 && character_at(fwd, 1) == ':') {
        return "file:///{fwd}"
    }
    return "file://{fwd}"
}

// ===== Diagnostics mapping =====
// Daemon: {diagnostics:[{file,line,column,code,severity,message,...}]} (1-based line/col).
// LSP wants 0-based; severity error=1 / warning=2; a 1-char range at the point for now.
// Returns a table mapping native file path -> JSON diagnostics array string.

def map_diagnostics_by_file(result_json : string; var out : table<string; array<string>>) {
    var err : string
    let jv = read_json(result_json, err)
    if (jv == null) return
    let diagsV = jv ?["diagnostics"]
    if (diagsV == null) return
    if (!(diagsV.value is _array)) return

    for (d in diagsV.value as _array) {
        let dfile = jstr(d, "file")
        let line : int = max(0, jint(d, "line", 1) - 1)
        let col : int = max(0, jint(d, "column", 1) - 1)
        let colEnd : int = col + 1
        let sev : int = jstr(d, "severity", "error") == "warning" ? 2 : 1
        let code : int = jint(d, "code", 0)
        let msg = jstr(d, "message")
        let rng = "\{\"start\":\{\"line\":{line},\"character\":{col}},\"end\":\{\"line\":{line},\"character\":{colEnd}}}"
        let entry = "\{\"range\":{rng},\"severity\":{sev},\"code\":{code},\"source\":\"duin\",\"message\":{json_string(msg)}}"
        out[dfile] |> push(entry)
    }
}

// ===== LSP method handlers =====

def send_response(id_json : string; result_json : string) {
    write_lsp_message("\{\"jsonrpc\":\"2.0\",\"id\":{id_json},\"result\":{result_json}}")
}

def send_notification(method : string; params_json : string) {
    write_lsp_message("\{\"jsonrpc\":\"2.0\",\"method\":\"{method}\",\"params\":{params_json}}")
}

def show_message(typ : int; text : string) {
    // window/showMessage — typ: 1=error 2=warning 3=info
    send_notification("window/showMessage", "\{\"type\":{typ},\"message\":{json_string(text)}}")
}

// Nearest *.das_project at or above root_path ("" if none).
def find_project(root_path : string) : string {
    var cur = root_path
    for (depth in range(0, 32)) {
        var found = ""
        dir(cur) <| $(name) {
            if (empty(found) && ends_with(name, ".das_project")) {
                found = "{cur}/{name}"
            }
        }
        if (!empty(found)) return found
        let parent = dir_name(cur)
        if (parent == cur || empty(parent)) break
        cur = parent
    }
    return ""
}

def handle_initialize(id_json : string; params : JsonValue?) {
    plog("handle_initialize: enter")
    var root = jstr(params, "rootUri")
    plog("  rootUri raw = {root}")
    if (!empty(root)) {
        root = uri_to_path(root)
    } else {
        root = jstr(params, "rootPath")
    }
    plog("  root path = {root}")

    g_project_path = empty(root) ? "" : find_project(root)
    plog("  project_path = {g_project_path}")

    let caps = "\{\"textDocumentSync\":\{\"openClose\":true,\"save\":true,\"change\":1},\"hoverProvider\":true,\"definitionProvider\":true,\"referencesProvider\":true,\"documentFormattingProvider\":true,\"executeCommandProvider\":\{\"commands\":[\"duin.refreshBindings\"]}}"
    send_response(id_json, "\{\"capabilities\":{caps}}")
    plog("  initialize response sent")

    if (empty(g_project_path)) {
        show_message(2, "Duin LSP: no .das_project found under {root}")
    } elif (!connect_daemon(g_project_path)) {
        show_message(2, "Duin LSP: no running daemon for {g_project_path} (editor plugin must start one)")
    } else {
        show_message(3, "Duin LSP: connected to daemon at {g_ws_url}")
    }
}

def compile_and_publish(uri : string; refresh : bool = false; content : string = "") {
    if (g_daemon == null || !g_daemon.connected) return
    let result = daemon_compile(uri_to_path(uri), refresh, content)
    if (empty(result)) return

    // Group diagnostics by the file they belong to, then publish each file separately.
    // This prevents errors in required files (e.g. fileB.das) from being shown on the
    // file that triggered the compile (fileA.das).
    var byFile : table<string; array<string>>
    map_diagnostics_by_file(result, byFile)

    // Always publish the compiled file (possibly with an empty array to clear stale diags).
    let compiledPath = uri_to_path(uri)
    if (!key_exists(byFile, compiledPath)) {
        byFile[compiledPath] |> resize(0)
    }

    for (filePath, entries in keys(byFile), values(byFile)) {
        let fileUri = path_to_uri(filePath)
        let joined = join(entries, ",")
        send_notification("textDocument/publishDiagnostics",
            "\{\"uri\":{json_string(fileUri)},\"diagnostics\":[{joined}]}")
        plog("publishDiagnostics -> {fileUri} ({length(entries)} diags)")
    }
    delete byFile
}

// Handles workspace/executeCommand. Currently: duin.refreshBindings — recompile the current
// file with refresh:true so the daemon re-reads engine .das modules from disk before compiling.
def handle_execute_command(id_json : string; params : JsonValue?) {
    let command = jstr(params, "command")
    send_response(id_json, "null")  // LSP requires a response before side-effects
    if (command == "duin.refreshBindings") {
        // We need a file URI to compile against. Use the arguments array if provided, else skip.
        let argsV = params ?["arguments"]
        var uri = ""
        if (argsV != null && argsV.value is _array) {
            assume arr = argsV.value as _array
            if (length(arr) > 0) {
                uri = arr[0] ?? ""
            }
        }
        if (empty(uri)) {
            show_message(2, "Duin LSP: refreshBindings needs a file URI (pass current buffer URI)")
        } else {
            show_message(3, "Duin LSP: refreshing engine bindings...")
            compile_and_publish(uri, true)
        }
    }
}

// Fires any pending debounced didChange compile (call before dispatching non-change messages).
def flush_pending_change {
    if (!g_has_pending) { return ; }
    g_has_pending = false
    let uri = g_pending_uri
    let content = g_pending_content
    g_pending_uri = ""
    g_pending_content = ""
    plog("flush_pending_change: compiling {uri}")
    compile_and_publish(uri, false, content)
}

// Handles textDocument/hover: sends type_of request to daemon, returns Hover response.
def handle_hover(id_json : string; params : JsonValue?) {
    let td = params ?["textDocument"]
    if (td == null) { send_response(id_json, "null"); return ; }
    let uri = jstr(td, "uri")
    let pos = params ?["position"]
    if (pos == null || empty(uri)) { send_response(id_json, "null"); return ; }
    let lsp_line = jint(pos, "line", 0)
    let lsp_col = jint(pos, "character", 0)
    let file_path = uri_to_path(uri)
    let req = "\{\"action\":\"type_of\",\"file\":{json_string(file_path)},\"project\":{json_string(g_project_path)},\"line\":{lsp_line + 1},\"column\":{lsp_col + 1}}"
    let reply = daemon_query(req)
    if (empty(reply)) { send_response(id_json, "null"); return ; }
    var err : string
    let jv = read_json(reply, err)
    if (jv == null) { send_response(id_json, "null"); return ; }
    let hover_text = jstr(jv, "hover")
    if (empty(hover_text)) { send_response(id_json, "null"); return ; }
    send_response(id_json, "\{\"contents\":\{\"kind\":\"plaintext\",\"value\":{json_string(hover_text)}}}")
}

// Handles textDocument/definition: sends goto_definition to daemon, returns Location.
def handle_definition(id_json : string; params : JsonValue?) {
    let td = params ?["textDocument"]
    if (td == null) { send_response(id_json, "null"); return ; }
    let uri = jstr(td, "uri")
    let pos = params ?["position"]
    if (pos == null || empty(uri)) { send_response(id_json, "null"); return ; }
    let lsp_line = jint(pos, "line", 0)
    let lsp_col = jint(pos, "character", 0)
    let file_path = uri_to_path(uri)
    let req = "\{\"action\":\"goto_definition\",\"file\":{json_string(file_path)},\"project\":{json_string(g_project_path)},\"line\":{lsp_line + 1},\"column\":{lsp_col + 1}}"
    let reply = daemon_query(req)
    if (empty(reply)) { send_response(id_json, "null"); return ; }
    var err : string
    let jv = read_json(reply, err)
    if (jv == null) { send_response(id_json, "null"); return ; }
    let foundV = jv ?["found"]
    let found = foundV != null && (foundV.value is _bool) && (foundV.value as _bool)
    if (!found) { send_response(id_json, "null"); return ; }
    let def_file = jstr(jv, "file")
    let def_line = max(0, jint(jv, "line", 1) - 1)
    let def_col = max(0, jint(jv, "column", 1) - 1)
    let def_uri = path_to_uri(def_file)
    let rng = "\{\"start\":\{\"line\":{def_line},\"character\":{def_col}},\"end\":\{\"line\":{def_line},\"character\":{def_col + 1}}}"
    send_response(id_json, "\{\"uri\":{json_string(def_uri)},\"range\":{rng}}")
}

// Handles textDocument/references: sends find_references to daemon, returns Location[].
def handle_references(id_json : string; params : JsonValue?) {
    let td = params ?["textDocument"]
    if (td == null) { send_response(id_json, "[]"); return ; }
    let uri = jstr(td, "uri")
    let pos = params ?["position"]
    if (pos == null || empty(uri)) { send_response(id_json, "[]"); return ; }
    let lsp_line = jint(pos, "line", 0)
    let lsp_col = jint(pos, "character", 0)
    let file_path = uri_to_path(uri)
    let req = "\{\"action\":\"find_references\",\"file\":{json_string(file_path)},\"project\":{json_string(g_project_path)},\"line\":{lsp_line + 1},\"column\":{lsp_col + 1}}"
    let reply = daemon_query(req)
    if (empty(reply)) { send_response(id_json, "[]"); return ; }
    var err : string
    let jv = read_json(reply, err)
    if (jv == null) { send_response(id_json, "[]"); return ; }
    let locsV = jv ?["locations"]
    if (locsV == null || !(locsV.value is _array)) { send_response(id_json, "[]"); return ; }
    var entries : array<string>
    for (loc in locsV.value as _array) {
        let loc_file = jstr(loc, "file")
        let loc_line = max(0, jint(loc, "line", 1) - 1)
        let loc_col = max(0, jint(loc, "column", 1) - 1)
        let loc_uri = path_to_uri(loc_file)
        let rng = "\{\"start\":\{\"line\":{loc_line},\"character\":{loc_col}},\"end\":\{\"line\":{loc_line},\"character\":{loc_col + 1}}}"
        entries |> push("\{\"uri\":{json_string(loc_uri)},\"range\":{rng}}")
    }
    send_response(id_json, "[{join(entries, ",")}]")
    delete entries
}

// Handles textDocument/formatting: formats the file in-proxy via das_source_formatter.
// Returns a single whole-document TextEdit replacing all content with the formatted output.
def handle_formatting(id_json : string; params : JsonValue?) {
    let td = params ?["textDocument"]
    if (td == null) { send_response(id_json, "[]"); return ; }
    let uri = jstr(td, "uri")
    if (empty(uri)) { send_response(id_json, "[]"); return ; }
    let file_path = uri_to_path(uri)
    // Read the file and format it.
    var original = fread(file_path)
    if (empty(original)) { send_response(id_json, "[]"); return ; }
    let formatted = format_source_string(original)
    if (formatted == original) { send_response(id_json, "[]"); return ; }
    // Count lines in the original so the TextEdit range covers the whole document.
    var line_count = 0
    var last_line_len = 0
    var i = 0
    let n = length(original)
    while (i < n) {
        let ch = character_at(original, i)
        if (ch == '\n') {
            line_count++
            last_line_len = 0
        } else {
            last_line_len++
        }
        i++
    }
    // Whole-document replacement: range from (0,0) to (line_count, last_line_len).
    let rng = "\{\"start\":\{\"line\":0,\"character\":0},\"end\":\{\"line\":{line_count},\"character\":{last_line_len}}}"
    let edit = "\{\"range\":{rng},\"newText\":{json_string(formatted)}}"
    send_response(id_json, "[{edit}]")
    plog("formatting: replaced {n} bytes with {length(formatted)} bytes")
}

def handle_text_document(method : string; params : JsonValue?) {
    let td = params ?["textDocument"]
    if (td == null) return
    let uri = jstr(td, "uri")
    if (empty(uri)) return
    if (method == "textDocument/didOpen" || method == "textDocument/didSave") {
        compile_and_publish(uri)
    } elif (method == "textDocument/didChange") {
        // Full-document sync (change:1): first element of contentChanges carries the whole buffer.
        // Debounced: store the latest content and URI; flush on the next non-didChange message.
        let changesV = params ?["contentChanges"]
        if (changesV == null) { return ; }
        if (!(changesV.value is _array)) { return ; }
        assume arr = changesV.value as _array
        if (length(arr) == 0) { return ; }
        let text = jstr(arr[0], "text")
        if (empty(text)) { return ; }
        plog("didChange: debouncing {uri}")
        g_pending_uri = uri
        g_pending_content = text
        g_has_pending = true
    }
}

// ===== Main loop =====

[export]
def main {
    g_stdin = fstdin()
    g_stdout = fstdout()

    // Open the trace log (best-effort; bare fopen returns the file without auto-closing).
    g_log = fopen("C:/Projects/CPP_Projects/Duin/DuinDasHost/scripts/proxy.log", "wb")
    g_log_open = g_log != null
    plog("=== proxy started ===")

    while (true) {
        var eof = false
        let body = read_lsp_message(eof)
        if (eof) { plog("read_lsp_message -> EOF, exiting loop"); break; }
        if (empty(body)) { plog("read_lsp_message -> empty body, continue"); continue; }
        plog("RECV: {body}")

        var perr : string
        let msg = read_json(body, perr)
        if (msg == null) { plog("read_json failed: {perr}"); continue; }

        let method = jstr(msg, "method")
        let params = msg ?["params"]
        let idV = msg ?["id"]
        let id_json = idV != null ? write_json(idV) : "null"
        plog("dispatch: method={method} id={id_json}")

        // Flush debounced didChange before processing any non-didChange message.
        if (method != "textDocument/didChange") {
            flush_pending_change()
        }

        if (method == "initialize") {
            handle_initialize(id_json, params)
        } elif (method == "initialized") {
            // no-op
        } elif (method == "shutdown") {
            send_response(id_json, "null")
        } elif (method == "exit") {
            break
        } elif (method == "workspace/executeCommand") {
            handle_execute_command(id_json, params)
        } elif (method == "textDocument/hover") {
            handle_hover(id_json, params)
        } elif (method == "textDocument/definition") {
            handle_definition(id_json, params)
        } elif (method == "textDocument/references") {
            handle_references(id_json, params)
        } elif (method == "textDocument/formatting") {
            handle_formatting(id_json, params)
        } elif (starts_with(method, "textDocument/")) {
            handle_text_document(method, params)
        }
    }

    if (g_daemon != null) {
        g_daemon->close()
        unsafe { delete g_daemon; }
    }
}
