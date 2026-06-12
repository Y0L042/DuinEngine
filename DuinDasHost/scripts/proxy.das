// DuinDasHost LSP proxy (M3 step 2).
//
// A thin, editor-agnostic Language Server that editors launch over stdio:
//   daslang.exe proxy.das
// It speaks LSP (Content-Length-framed JSON-RPC) to the editor and forwards compile
// requests over WebSocket to the per-project DuinDasHost daemon (--serve-ws), mapping the
// daemon's diagnostics back into LSP publishDiagnostics. The warm Script lives in the C++
// daemon; this proxy is stateless translation.
//
// Lifecycle: the proxy only CONNECTS to an already-running daemon (reads the project's
// <project>.das_project.ddh, connects to its wsUrl). Spawning the daemon is the editor
// plugin's job (daslang has no detached-spawn primitive). If no live daemon is found, the
// proxy reports it via window/showMessage so the plugin can spawn one.
//
// This slice: initialize / initialized / shutdown / exit, and textDocument/didOpen +
// didSave -> compile -> publishDiagnostics. didChange (on-type) is M5.

options gen2
options persistent_heap
options gc

require dashv/dashv_boost public
require daslib/json public
require daslib/json_boost public
require daslib/fio
require daslib/strings_boost
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

// Debounce state for textDocument/didChange. The latest unsaved buffer content is stored
// here and flushed (compiled) on the next non-didChange message so rapid keystroke bursts
// don't enqueue a compile per character.

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
// Pass non-empty content to compile an unsaved in-memory buffer instead of the file on disk.
def daemon_compile(file_path : string; refresh : bool = false; content : string = "") : string {
    if (g_daemon == null || !g_daemon.connected) return ""
    g_daemon.got_reply = false
    g_daemon.reply = ""
    let refresh_field = refresh ? ",\"refresh\":true" : ""
    let content_field = !empty(content) ? ",\"content\":{json_string(content)}" : ""
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

    let caps = "\{\"textDocumentSync\":\{\"openClose\":true,\"change\":1,\"save\":true},\"executeCommandProvider\":\{\"commands\":[\"duin.refreshBindings\"]}}"
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

def handle_text_document(method : string; params : JsonValue?) {
    let td = params ?["textDocument"]
    if (td == null) return
    let uri = jstr(td, "uri")
    if (empty(uri)) return
    if (method == "textDocument/didOpen" || method == "textDocument/didSave") {
        compile_and_publish(uri)
    } elif (method == "textDocument/didChange") {
        let changesV = params ?["contentChanges"]
        if (changesV == null) { return ; }
        if (!(changesV.value is _array)) { return ; }
        assume arr = changesV.value as _array
        if (length(arr) == 0) { return ; }
        let text = jstr(arr[0], "text")
        if (empty(text)) { return ; }
        plog("didChange: compiling {uri}")
        compile_and_publish(uri, false, text)
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
        } elif (starts_with(method, "textDocument/")) {
            handle_text_document(method, params)
        }
    }

    if (g_daemon != null) {
        g_daemon->close()
        unsafe { delete g_daemon; }
    }
}