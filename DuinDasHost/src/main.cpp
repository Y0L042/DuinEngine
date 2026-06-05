// DuinDasHost — headless, engine-linked daslang compile host.
//
// Compiles a target .das with Duin's engine modules registered (so dn_* bindings
// resolve), then emits structured diagnostics as JSON to stdout. Foundation for the
// editor-agnostic language tooling (LSP) tracked in docs/plans/DuinDasHost_plan.md.
//
// Usage:
//   DuinDasHost.exe <file.das> [--project <path.das_project>] [--dasroot <path>] [--json]
//   DuinDasHost.exe --serve      [--project <path.das_project>] [--dasroot <path>]
//   DuinDasHost.exe --serve-tcp   --project <path.das_project>  [--dasroot <path>]
//
// One-shot mode emits a single JSON compile result on stdout and exits.
//
// Both serve modes are the LSP foundation: they register the engine modules and warm the
// daslib/engine module graph ONCE (~4s), then compile only the target file per request
// (~tens of ms each), so editors get near-instant diagnostics.
//
//   --serve (stdin): single-client. Reads one request per stdin line, writes one
//     result-JSON line. Prints {"ready":true} after warm-up. For piping / manual testing.
//
//   --serve-tcp: per-PROJECT daemon. Binds 127.0.0.1:<free-port>, writes a discovery/lock
//     file "<project>.das_project.ddh" {pid,port,transport:tcp,...}, and serves one request
//     per TCP connection (bespoke line protocol; used by the `ddh compile` CLI). The
//     .das_project path IS the daemon identity (1 project : 1 daemon : many editors).
//     Requests: {"file":...,"project":...} (compile), {"ping":true} -> {"ready":true,"pid":N},
//     {"quit":true} -> shutdown (removes .ddh).
//
//   --serve-ws (editor LSP proxy): same per-project daemon + same requests, but over
//     WebSocket (libhv) so the daslang `.das` LSP proxy connects via dasHV's
//     HvWebSocketClient. .ddh carries transport:ws + wsUrl (ws://127.0.0.1:<port>).
//     Auto-shutdown: exits after --idle-timeout <secs> (default 30, 0=never) at zero WS
//     connections. Holds a <project>.das_project.ddh.lock while running (spawn-race guard)
//     so two editors opening at once converge on one shared daemon.
//
// stdout: JSON only (one-shot result / stdin-serve lines). stderr: all engine/daslang noise.
// Exit code: 0 on success/clean exit, 1 on compile errors (one-shot), 2 on bad arguments.

#include <daScript/daScript.h>

#include <Duin/Core/Debug/DNLog.h>
#include <Duin/Core/Debug/DNAssert.h>
#include <Duin/ECS/DnFlecs.h>
#include <Duin/IO/Filesystem.h>
#include <Duin/Script/Script.h>
#include <Duin/Script/ScriptModules.h>

#include <daScript/misc/crash_handler.h>
#include <flecs_das.h>

// libhv (dasHV's underlying C++ lib) — WebSocket server for the editor-facing endpoint (M3).
#include <hv/WebSocketServer.h>
#include <hv/WebSocketChannel.h>
#include <hv/hsocket.h> // Listen() — synchronous bind+listen, returns the listen fd

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
#include <iostream>
#include <ctime>
#include <atomic>
#include <mutex>
#include <deque>
#include <optional>
#include <chrono>

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#include <process.h> // _getpid
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#define DN_DUP _dup
#define DN_DUP2 _dup2
#define DN_FILENO _fileno
#define DN_GETPID _getpid
using dn_socket_t = SOCKET;
#define DN_INVALID_SOCKET INVALID_SOCKET
#define DN_SOCKET_ERROR SOCKET_ERROR
#define DN_CLOSESOCKET closesocket
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define DN_DUP dup
#define DN_DUP2 dup2
#define DN_FILENO fileno
#define DN_GETPID getpid
using dn_socket_t = int;
#define DN_INVALID_SOCKET (-1)
#define DN_SOCKET_ERROR (-1)
#define DN_CLOSESOCKET close
#endif

namespace
{

// Redirects the process's stdout (fd 1) to stderr (fd 2) for the lifetime of this
// object, restoring it on destruction. Used so engine spdlog output and daslang's
// TextPrinter — both of which write to stdout — do not pollute the JSON we emit.
class StdoutToStderrGuard
{
  public:
    StdoutToStderrGuard()
    {
        std::fflush(stdout);
        savedStdoutFd_ = DN_DUP(DN_FILENO(stdout));
        DN_DUP2(DN_FILENO(stderr), DN_FILENO(stdout));
    }

    ~StdoutToStderrGuard()
    {
        Restore();
    }

    // Restores the original stdout. Idempotent.
    void Restore()
    {
        if (savedStdoutFd_ < 0)
            return;
        std::fflush(stdout);
        DN_DUP2(savedStdoutFd_, DN_FILENO(stdout));
#ifdef _WIN32
        _close(savedStdoutFd_);
#else
        close(savedStdoutFd_);
#endif
        savedStdoutFd_ = -1;
    }

  private:
    int savedStdoutFd_ = -1;
};

// Minimal JSON string escaper — enough for diagnostic messages and paths.
std::string JsonEscape(const std::string &s)
{
    std::string out;
    out.reserve(s.size() + 8);
    for (char c : s)
    {
        switch (c)
        {
        case '"':
            out += "\\\"";
            break;
        case '\\':
            out += "\\\\";
            break;
        case '\n':
            out += "\\n";
            break;
        case '\r':
            out += "\\r";
            break;
        case '\t':
            out += "\\t";
            break;
        default:
            if (static_cast<unsigned char>(c) < 0x20)
            {
                char buf[8];
                std::snprintf(buf, sizeof(buf), "\\u%04x", c);
                out += buf;
            }
            else
            {
                out += c;
            }
        }
    }
    return out;
}

const char *SeverityName(duin::Diagnostic::Severity sev)
{
    return sev == duin::Diagnostic::Severity::Warning ? "warning" : "error";
}

// Serializes the compile result in the shape mirrored from compile_check.das:
//   { "file": ..., "success": bool,
//     "diagnostics": [ {file,line,column,code,severity,message[,detail][,fixme]} ] }
// `detail` carries daslang's err.extra — the "candidate function" breakdown and
// "while compiling" context that explains why an overload/type didn't match. Maps
// well to LSP relatedInformation in the M2 wrapper.
std::string BuildResultJson(const std::string &file, bool success, const std::vector<duin::Diagnostic> &diags)
{
    std::ostringstream os;
    os << "{";
    os << "\"file\":\"" << JsonEscape(file) << "\",";
    os << "\"success\":" << (success ? "true" : "false") << ",";
    os << "\"diagnostics\":[";
    for (size_t i = 0; i < diags.size(); ++i)
    {
        const auto &d = diags[i];
        if (i)
            os << ",";
        os << "{";
        os << "\"file\":\"" << JsonEscape(d.file) << "\",";
        os << "\"line\":" << d.line << ",";
        os << "\"column\":" << d.column << ",";
        os << "\"code\":" << d.code << ",";
        os << "\"severity\":\"" << SeverityName(d.severity) << "\",";
        os << "\"message\":\"" << JsonEscape(d.message) << "\"";
        if (!d.extra.empty())
            os << ",\"detail\":\"" << JsonEscape(d.extra) << "\"";
        if (!d.fixme.empty())
            os << ",\"fixme\":\"" << JsonEscape(d.fixme) << "\"";
        os << "}";
    }
    os << "]";
    os << "}";
    return os.str();
}

// Returns true if <root>/daslib/builtin.das exists — the marker that <root> is a
// valid daslang root.
bool IsValidDasRoot(const std::string &root)
{
    if (root.empty())
        return false;
    std::string marker = root + "/daslib/builtin.das";
    if (FILE *f = std::fopen(marker.c_str(), "rb"))
    {
        std::fclose(f);
        return true;
    }
    return false;
}

// Walks up from the executable directory looking for Duin/vendor/daslang.
// Uses the native (C:/...) base path, so the result is a path daslang's file
// layer can open — unlike an MSYS-style /c/... path from a shell env var.
std::string DeriveDasRootFromExe()
{
    std::string base = duin::fs::GetBasePath();
    if (base.empty() || base == INVALID_PATH)
        return "";
    if (base.back() == '/')
        base.pop_back();
    std::string dir = base;
    for (int depth = 0; depth < 8; ++depth)
    {
        std::string candidate = dir + "/Duin/vendor/daslang";
        if (IsValidDasRoot(candidate))
            return candidate;
        auto slash = dir.find_last_of('/');
        if (slash == std::string::npos)
            break;
        dir = dir.substr(0, slash);
    }
    return "";
}

// Resolves the daslang root in priority order, but only accepts a source if it
// actually contains daslib/builtin.das (IsValidDasRoot). This rejects MSYS-style
// /c/... paths (e.g. from DUIN_DAS_ROOT in a shell env) that the native daslang
// file layer cannot open, falling through to the exe-derived native path.
//   1. explicit --dasroot arg (if valid)
//   2. DUIN_DAS_ROOT environment variable (if valid)
//   3. derived from the executable base path (standard build layout).
// Returns empty string if none yield a valid root (caller reports the error).
std::string ResolveDasRoot(const std::string &argRoot)
{
    if (IsValidDasRoot(argRoot))
        return argRoot;

    if (const char *env = std::getenv("DUIN_DAS_ROOT"))
    {
        if (IsValidDasRoot(env))
            return env;
    }

    return DeriveDasRootFromExe();
}

// Makes a path absolute (relative to the current working directory) with forward
// slashes. daslang's FsFileAccess anchors a project file's module_get() relative
// paths to the project file's own directory; if the project path itself is relative
// that anchor is lost and engine module requires (e.g. flecs_boost) fail to resolve.
// Canonicalizing here lets callers pass paths relative to their shell CWD.
std::string MakeAbsoluteUnix(const std::string &path)
{
    if (path.empty())
        return path;
    std::error_code ec;
    std::filesystem::path abs = std::filesystem::absolute(std::filesystem::u8path(path), ec);
    if (ec)
        return path;
    std::string s = abs.generic_string();
    return s;
}

struct Args
{
    std::string file;
    std::string project;
    std::string dasRoot;
    bool json = false;
    bool serve = false;    // daemon mode: warm up once, then compile per stdin request
    bool serveTcp = false;    // daemon mode over 127.0.0.1:0 (bespoke line protocol); writes .ddh
    bool serveWs = false;     // daemon mode over WebSocket (editor LSP proxy); writes .ddh
    int idleTimeoutSecs = 30; // --serve-ws: exit after N idle secs at 0 connections (0 = never)
    bool valid = false;
};

Args ParseArgs(int argc, char **argv)
{
    Args a;
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "--project" && i + 1 < argc)
        {
            a.project = argv[++i];
        }
        else if (arg == "--dasroot" && i + 1 < argc)
        {
            a.dasRoot = argv[++i];
        }
        else if (arg == "--json")
        {
            a.json = true;
        }
        else if (arg == "--serve")
        {
            a.serve = true;
        }
        else if (arg == "--serve-tcp")
        {
            a.serveTcp = true;
        }
        else if (arg == "--serve-ws")
        {
            a.serveWs = true;
        }
        else if (arg == "--idle-timeout" && i + 1 < argc)
        {
            a.idleTimeoutSecs = std::atoi(argv[++i]);
        }
        else if (!arg.empty() && arg[0] != '-' && a.file.empty())
        {
            a.file = arg;
        }
    }
    // One-shot mode needs a file; serve modes read files from requests.
    a.valid = a.serve || a.serveTcp || a.serveWs || !a.file.empty();
    return a;
}

// Extracts a string value for `key` from a flat JSON object line, e.g.
// {"file":"a.das","project":"p"} -> for key "file" returns "a.das". Handles the
// \" \\ \n \r \t escapes the client mirror-emits. Returns empty if key is absent.
// Deliberately minimal — the serve protocol is single-line flat objects, no nesting.
std::string JsonExtractString(const std::string &line, const std::string &key)
{
    std::string needle = "\"" + key + "\"";
    size_t k = line.find(needle);
    if (k == std::string::npos)
        return "";
    size_t colon = line.find(':', k + needle.size());
    if (colon == std::string::npos)
        return "";
    size_t q = line.find('"', colon + 1);
    if (q == std::string::npos)
        return "";
    std::string out;
    for (size_t i = q + 1; i < line.size(); ++i)
    {
        char c = line[i];
        if (c == '\\' && i + 1 < line.size())
        {
            char n = line[++i];
            switch (n)
            {
            case 'n':
                out += '\n';
                break;
            case 'r':
                out += '\r';
                break;
            case 't':
                out += '\t';
                break;
            default:
                out += n;
                break; // covers \" and "\\"
            }
        }
        else if (c == '"')
        {
            break;
        }
        else
        {
            out += c;
        }
    }
    return out;
}

// Returns true if `line` is JSON requesting shutdown: {"quit":true}.
bool IsQuitRequest(const std::string &line)
{
    return line.find("\"quit\"") != std::string::npos;
}

// Trims leading/trailing ASCII whitespace (incl. CR from CRLF stdin).
std::string Trim(const std::string &s)
{
    size_t b = 0, e = s.size();
    while (b < e && (unsigned char)s[b] <= ' ')
        ++b;
    while (e > b && (unsigned char)s[e - 1] <= ' ')
        --e;
    return s.substr(b, e - b);
}

} // namespace

// Must live at global scope (NOT an anonymous namespace): NEED_MODULE expands to an
// `extern das::Module * register_Module_X()` declaration that inherits the enclosing
// namespace. The matching definitions from REGISTER_MODULE are at global scope, so
// declaring the extern inside a namespace produces unresolved externals at link time.
static void RegisterEngineModules()
{
    // Mirror ExampleProjects/DuinFPSDaslang/src/main.cpp — the working runtime that
    // compiles these dn_* modules. UriParser + register_builtin_modules() must run
    // here (das root is already set) so daslib/builtin.das builds against the root.
    NEED_MODULE(Module_UriParser);
    das::register_builtin_modules();

    NEED_MODULE(Module_flecs);
    NEED_MODULE(Module_imgui);
    NEED_MODULE(Module_TOMLC17);

    NEED_MODULE(Module_DnLog);
    NEED_MODULE(Module_DnRenderer);
    NEED_MODULE(Module_DnCamera);
    NEED_MODULE(Module_DnGameObject);
    NEED_MODULE(Module_DnGameStateMachine);
    NEED_MODULE(Module_DnECS);
    NEED_MODULE(Module_DecsGameWorld);
    NEED_MODULE(Module_DnSceneBuilder);
    NEED_MODULE(Module_DnInput);
    NEED_MODULE(Module_DnPhysicsServer);
    NEED_MODULE(Module_DnCharacterBody);
    NEED_MODULE(Module_DnApplication);
    NEED_MODULE(Module_DnFilesystem);
    NEED_MODULE(Module_DnUUID);
}

// Compiles one target file against an already-warmed Script and returns the result
// JSON. `displayFile` is echoed back verbatim in the result's "file" field (the path
// as the client asked for it); `scriptPath`/`projectPath` are the absolute paths
// actually fed to daslang. `mode` controls whether the cached daslib + dn_* engine
// bindings are reused (KeepCachedBindings, the fast default) or re-read from disk
// (RefreshBindings, after editing engine .das bindings).
// The StdoutToStderrGuard keeps daslang's TextPrinter noise off the result stream.
static std::string CompileOne(
    duin::Script &script, const std::string &displayFile, const std::string &scriptPath, const std::string &projectPath,
    duin::RecompileMode mode = duin::RecompileMode::KeepCachedBindings, bool *outSuccess = nullptr)
{
    StdoutToStderrGuard guard;
    if (!projectPath.empty())
        script.SetProjectFile(projectPath);
    script.SetScriptPath(scriptPath);
    script.PrepareForRecompile(mode);
    bool ok = script.Compile();
    std::string json = BuildResultJson(displayFile, ok, script.GetDiagnostics());
    guard.Restore();
    if (outSuccess)
        *outSuccess = ok;
    return json;
}

// Outcome of interpreting one request line, shared by the stdin and TCP loops.
struct RequestResult
{
    bool quit = false;    // client asked the daemon to shut down
    std::string response; // single JSON line to send back (no trailing newline)
};

// Interprets one request line and (for compile/ping) produces the JSON response.
// Request forms:
//   {"file":"...","project":"..."}  -> compile (project optional, defaults below)
//   {"ping":true}                   -> {"ready":true,"pid":N}
//   {"quit":true}                   -> shut down (quit=true, empty response)
//   <bare path>                     -> compile that file
// Empty/whitespace lines yield an empty response (caller skips sending).
static RequestResult HandleRequestLine(
    duin::Script &script, const std::string &rawLine, const std::string &defaultProject)
{
    RequestResult out;
    std::string req = Trim(rawLine);
    if (req.empty())
        return out;

    if (IsQuitRequest(req))
    {
        out.quit = true;
        return out;
    }
    if (req.find("\"ping\"") != std::string::npos)
    {
        char buf[64];
        std::snprintf(buf, sizeof(buf), "{\"ready\":true,\"pid\":%d}", (int)DN_GETPID());
        out.response = buf;
        return out;
    }

    std::string displayFile;
    std::string projectArg;
    duin::RecompileMode mode = duin::RecompileMode::KeepCachedBindings;
    if (req.front() == '{')
    {
        displayFile = JsonExtractString(req, "file");
        projectArg = JsonExtractString(req, "project");
        // "refresh":true re-reads the dn_* engine bindings (and daslib) from disk —
        // for after you edit a binding .das. Absent/false reuses the warm cached graph.
        if (req.find("\"refresh\"") != std::string::npos && req.find("\"refresh\":false") == std::string::npos)
            mode = duin::RecompileMode::RefreshBindings;
    }
    else
    {
        displayFile = req; // bare path
    }

    if (displayFile.empty())
    {
        out.response = "{\"file\":\"\",\"success\":false,\"diagnostics\":[{\"file\":\"\",\"line\":0,"
                       "\"column\":0,\"code\":0,\"severity\":\"error\","
                       "\"message\":\"request missing 'file'\"}]}";
        return out;
    }

    std::string projectPath = MakeAbsoluteUnix(projectArg.empty() ? defaultProject : projectArg);
    std::string scriptPath = MakeAbsoluteUnix(displayFile);
    out.response = CompileOne(script, displayFile, scriptPath, projectPath, mode);
    return out;
}

// Daemon loop over stdin/stdout. Reads one request per line until EOF or {"quit":true},
// emitting one result-JSON line per request. Single-client; for piping/manual testing.
static int RunServeLoop(duin::Script &script, const std::string &defaultProject)
{
    // Modules are already registered by main() before we get here. Signal readiness so
    // clients know the daemon is up. Note the first request still pays the one-time
    // daslib/engine graph compile (~4s); subsequent requests reuse the warm base set.
    std::fprintf(stdout, "{\"ready\":true}\n");
    std::fflush(stdout);

    std::string line;
    while (std::getline(std::cin, line))
    {
        RequestResult r = HandleRequestLine(script, line, defaultProject);
        if (r.quit)
            break;
        if (r.response.empty())
            continue;
        std::fprintf(stdout, "%s\n", r.response.c_str());
        std::fflush(stdout);
    }

    return 0;
}

// ---- TCP daemon (editor-facing) -------------------------------------------------

// Path of the discovery/lock file for a project: "<project>.das_project.ddh".
// `absProject` must already be the resolved absolute project path.
static std::string DdhPathFor(const std::string &absProject)
{
    return absProject + ".ddh";
}

static std::string DdhLockPathFor(const std::string &absProject)
{
    return absProject + ".ddh.lock";
}

// RAII holder for the spawn lock: removes the .lock file on destruction when held.
// Acquired in main() (before warm-up) so a race loser exits fast; lives until the
// daemon process ends.
struct SpawnLock
{
    std::string path;
    bool held = false;
    ~SpawnLock()
    {
        if (!held)
            return;
        std::error_code ec;
        std::filesystem::remove(std::filesystem::u8path(path), ec);
    }
};

// Tries to atomically claim the spawn lock for a project (exclusive-create the .lock file,
// writing our pid). Returns true if we won the lock, false if another daemon already holds
// it. Breaks the two-editors-spawn-at-once race: both proxies may launch a daemon, but only
// one wins the lock and binds; the loser exits and its proxy connects to the winner's .ddh.
// A stale lock (older than staleSecs with no live .ddh yet) is reclaimed — covers a daemon
// that crashed after locking but before writing .ddh.
static bool TryAcquireSpawnLock(const std::string &lockPath, const std::string &ddhPath, int staleSecs)
{
    auto tryExclusiveCreate = [&]() -> bool {
        // "wx"/"x" → fail if the file exists (C11 exclusive create); maps to O_EXCL.
        FILE *f = std::fopen(lockPath.c_str(), "wx");
        if (!f)
            return false;
        std::fprintf(f, "%d\n", (int)DN_GETPID());
        std::fclose(f);
        return true;
    };

    if (tryExclusiveCreate())
        return true;

    // Lock exists. Reclaim it only if it looks stale: old enough AND no .ddh has appeared
    // (a live/starting daemon writes .ddh quickly). This avoids stealing a lock from a
    // daemon that is mid-startup.
    std::error_code ec;
    if (std::filesystem::exists(std::filesystem::u8path(ddhPath), ec))
        return false; // a daemon is up (or nearly); don't claim
    auto lockTime = std::filesystem::last_write_time(std::filesystem::u8path(lockPath), ec);
    if (!ec)
    {
        auto age = std::filesystem::file_time_type::clock::now() - lockTime;
        if (std::chrono::duration_cast<std::chrono::seconds>(age).count() >= staleSecs)
        {
            std::filesystem::remove(std::filesystem::u8path(lockPath), ec);
            return tryExclusiveCreate();
        }
    }
    return false;
}

// Current UTC time as ISO-8601 (e.g. 2026-06-04T12:41:05Z), for the .ddh "started" field.
static std::string NowIso8601Utc()
{
    std::time_t t = std::time(nullptr);
    std::tm tmv{};
#ifdef _WIN32
    gmtime_s(&tmv, &t);
#else
    gmtime_r(&t, &tmv);
#endif
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &tmv);
    return buf;
}

// Atomically writes the .ddh discovery file (temp + rename) so a reader never sees a
// half-written file. `transport` is "tcp" (bespoke line protocol, for `ddh compile`) or
// "ws" (WebSocket, for the editor LSP proxy); for "ws" a `wsUrl` field is added so the
// proxy can connect without reconstructing the URL. Returns false on failure.
static bool WriteDdhFile(const std::string &ddhPath, const std::string &absProject, int port, const char *transport)
{
    std::ostringstream os;
    os << "{\"pid\":" << (int)DN_GETPID() << ",\"port\":" << port << ",\"host\":\"127.0.0.1\","
       << "\"transport\":\"" << transport << "\",";
    if (std::string(transport) == "ws")
        os << "\"wsUrl\":\"ws://127.0.0.1:" << port << "\",";
    os << "\"project\":\"" << JsonEscape(absProject) << "\",\"protocol\":1,"
       << "\"started\":\"" << NowIso8601Utc() << "\"}\n";
    std::string content = os.str();

    std::string tmp = ddhPath + ".tmp";
    if (FILE *f = std::fopen(tmp.c_str(), "wb"))
    {
        std::fwrite(content.data(), 1, content.size(), f);
        std::fclose(f);
    }
    else
    {
        return false;
    }
    std::error_code ec;
    std::filesystem::rename(std::filesystem::u8path(tmp), std::filesystem::u8path(ddhPath), ec);
    if (ec)
    {
        // Fallback: remove existing then rename (Windows rename fails if target exists).
        std::filesystem::remove(std::filesystem::u8path(ddhPath), ec);
        std::filesystem::rename(std::filesystem::u8path(tmp), std::filesystem::u8path(ddhPath), ec);
    }
    return !ec;
}

// Reads one '\n'-terminated request line from a connected socket. Returns false on
// EOF/error before any newline. Caps the line to guard against unbounded input.
static bool RecvLine(dn_socket_t sock, std::string &out)
{
    out.clear();
    char c;
    for (;;)
    {
        int n = ::recv(sock, &c, 1, 0);
        if (n <= 0)
            return !out.empty(); // partial line on EOF still usable
        if (c == '\n')
            return true;
        if (c != '\r')
            out += c;
        if (out.size() > (1u << 20))
            return true; // 1 MiB cap
    }
}

// Sends a string in full over a socket (loops on partial writes).
static void SendAll(dn_socket_t sock, const std::string &s)
{
    size_t sent = 0;
    while (sent < s.size())
    {
        int n = ::send(sock, s.data() + sent, (int)(s.size() - sent), 0);
        if (n <= 0)
            return;
        sent += (size_t)n;
    }
}

// TCP daemon loop. Binds 127.0.0.1 on an OS-assigned free port, writes the project's
// .ddh discovery file, then serves one request per accepted connection against the warm
// Script. Multiple editors may connect concurrently; requests are handled one at a time
// (compilation is not reentrant). Exits on a {"quit":true} request, removing the .ddh.
static int RunServeTcp(duin::Script &script, const std::string &absProject)
{
    if (absProject.empty())
    {
        std::fprintf(stderr, "error: --serve-tcp requires --project (the .das_project is the daemon identity)\n");
        return 2;
    }

#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        std::fprintf(stderr, "error: WSAStartup failed\n");
        return 2;
    }
#endif

    // All socket functions are called with leading :: (global scope). The PCH
    // (apppch.h) force-includes <functional>, so an unqualified bind(...) would be
    // ambiguous with std::bind via argument-dependent lookup. :: pins the C API.
    dn_socket_t listener = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == DN_INVALID_SOCKET)
    {
        std::fprintf(stderr, "error: socket() failed\n");
        return 2;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ::htonl(INADDR_LOOPBACK); // 127.0.0.1 only
    addr.sin_port = ::htons(0);                      // OS picks a free port

    if (::bind(listener, (sockaddr *)&addr, (int)sizeof(addr)) == DN_SOCKET_ERROR ||
        ::listen(listener, 8) == DN_SOCKET_ERROR)
    {
        std::fprintf(stderr, "error: bind/listen failed\n");
        DN_CLOSESOCKET(listener);
        return 2;
    }

    // Read back the assigned port.
    sockaddr_in bound{};
#ifdef _WIN32
    int blen = sizeof(bound);
#else
    socklen_t blen = sizeof(bound);
#endif
    if (::getsockname(listener, (sockaddr *)&bound, &blen) == DN_SOCKET_ERROR)
    {
        std::fprintf(stderr, "error: getsockname failed\n");
        DN_CLOSESOCKET(listener);
        return 2;
    }
    int port = ::ntohs(bound.sin_port);

    std::string ddhPath = DdhPathFor(absProject);
    if (!WriteDdhFile(ddhPath, absProject, port, "tcp"))
    {
        std::fprintf(stderr, "error: could not write .ddh file: %s\n", ddhPath.c_str());
        DN_CLOSESOCKET(listener);
        return 2;
    }
    std::fprintf(
        stderr,
        "[DuinDasHost] serving project '%s' on 127.0.0.1:%d (pid %d)\n",
        absProject.c_str(),
        port,
        (int)DN_GETPID());
    std::fprintf(stderr, "[DuinDasHost] wrote discovery file %s\n", ddhPath.c_str());

    bool quit = false;
    while (!quit)
    {
        dn_socket_t conn = ::accept(listener, nullptr, nullptr);
        if (conn == DN_INVALID_SOCKET)
            continue;

        std::string line;
        if (RecvLine(conn, line))
        {
            RequestResult r = HandleRequestLine(script, line, absProject);
            if (r.quit)
            {
                SendAll(conn, "{\"bye\":true}\n");
                quit = true;
            }
            else if (!r.response.empty())
            {
                SendAll(conn, r.response + "\n");
            }
        }
        DN_CLOSESOCKET(conn);
    }

    DN_CLOSESOCKET(listener);
    std::error_code ec;
    std::filesystem::remove(std::filesystem::u8path(ddhPath), ec);
    std::fprintf(stderr, "[DuinDasHost] shut down; removed %s\n", ddhPath.c_str());
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}

// Reads the OS-assigned port from a bound listening socket fd via getsockname.
// Returns -1 on failure.
static int PortFromFd(int fd)
{
    sockaddr_in bound{};
#ifdef _WIN32
    int blen = sizeof(bound);
#else
    socklen_t blen = sizeof(bound);
#endif
    if (::getsockname((dn_socket_t)fd, (sockaddr *)&bound, &blen) == DN_SOCKET_ERROR)
        return -1;
    return ::ntohs(bound.sin_port);
}

// WebSocket daemon (editor-facing, M3). Serves the same compile/ping/quit requests as
// RunServeTcp, but over WebSocket so the daslang `.das` LSP proxy can connect via dasHV's
// HvWebSocketClient. One message in (a request line), one message out (the JSON result).
//
// Concurrency: libhv runs callbacks on worker threads, but duin::Script compilation is NOT
// reentrant, so setThreadNum(1) pins all WS callbacks to a single worker — preserving the
// one-at-a-time guarantee the TCP accept-loop also relies on.
static int RunServeWs(duin::Script &script, const std::string &absProject, int idleTimeoutSecs)
{
    if (absProject.empty())
    {
        std::fprintf(stderr, "error: --serve-ws requires --project (the .das_project is the daemon identity)\n");
        return 2;
    }

    std::string ddhPath = DdhPathFor(absProject);
    // Note: the spawn-race lock is already held by main() (acquired before warm-up so a
    // race loser exits fast); main's SpawnLock removes it on exit.

    // Drives clean shutdown: set when a {"quit":true} request is processed on the main
    // loop below. atomic<bool> is belt-and-suspenders (only the main thread touches it now).
    std::atomic<bool> stopRequested{false};

    // Live WS connection count, maintained by onopen/onclose (worker threads). The main
    // loop uses it for idle auto-shutdown: when it sits at 0 past idleTimeoutSecs, the
    // daemon exits so editors never have to stop it manually. A reconnect resets the timer.
    std::atomic<int> liveConnections{0};

    // A pending WS request: which channel asked, and the raw request line. libhv invokes
    // onmessage on a worker thread, but duin::Script (daslang context + engine state) was
    // warmed up on THIS (main) thread and is NOT thread-safe — compiling on the worker
    // thread access-violates. So onmessage only enqueues; the main loop below drains the
    // queue and does the compile + reply on the main thread.
    struct PendingRequest
    {
        WebSocketChannelPtr channel;
        std::string msg;
    };
    std::mutex queueMutex;
    std::deque<PendingRequest> requestQueue;

    hv::WebSocketService ws;
    ws.onopen = [&liveConnections](const WebSocketChannelPtr &, const HttpRequestPtr &) {
        liveConnections.fetch_add(1);
    };
    ws.onclose = [&liveConnections](const WebSocketChannelPtr &) { liveConnections.fetch_sub(1); };
    ws.onmessage = [&queueMutex, &requestQueue](const WebSocketChannelPtr &channel, const std::string &msg) {
        std::lock_guard<std::mutex> lock(queueMutex);
        requestQueue.push_back(PendingRequest{channel, msg});
    };

    hv::WebSocketServer server(&ws);
    server.setHost("127.0.0.1");
    // Bind the listen socket ourselves so we know the OS-assigned port BEFORE the server
    // starts. libhv's Listen(port, host) does socket→bind→listen synchronously and returns
    // the fd; passing port 0 yields a free port. (Relying on server.start()+listenfd[0]
    // failed: start() runs wait=false and returns before the worker thread has bound, so
    // listenfd[0] is still -1 when read.) We then hand the bound fd to libhv via setListenFD.
    int listenfd = Listen(0, "127.0.0.1");
    if (listenfd < 0)
    {
        std::fprintf(stderr, "error: WebSocket Listen(0) failed\n");
        return 2;
    }
    int port = PortFromFd(listenfd);
    if (port <= 0)
    {
        std::fprintf(stderr, "error: could not read bound WebSocket port\n");
        DN_CLOSESOCKET((dn_socket_t)listenfd);
        return 2;
    }
    server.setListenFD(listenfd);
    server.setThreadNum(1);

    // start() = run(wait=false): spins the event loop on worker threads using our pre-bound
    // listen fd, and returns immediately (unlike run(), which blocks).
    if (server.start() != 0)
    {
        std::fprintf(stderr, "error: WebSocket server failed to start\n");
        DN_CLOSESOCKET((dn_socket_t)listenfd);
        return 2;
    }

    if (!WriteDdhFile(ddhPath, absProject, port, "ws"))
    {
        std::fprintf(stderr, "error: could not write .ddh file: %s\n", ddhPath.c_str());
        server.stop();
        return 2;
    }
    std::fprintf(
        stderr,
        "[DuinDasHost] serving project '%s' on ws://127.0.0.1:%d (pid %d)\n",
        absProject.c_str(),
        port,
        (int)DN_GETPID());
    std::fprintf(stderr, "[DuinDasHost] wrote discovery file %s\n", ddhPath.c_str());

    if (idleTimeoutSecs > 0)
        std::fprintf(stderr, "[DuinDasHost] idle auto-shutdown after %ds at zero connections\n", idleTimeoutSecs);

    // Main loop: drain queued WS requests and compile them HERE (the thread that warmed
    // the Script). Replies go back on the request's channel. A {"quit":true} request flags
    // shutdown after replying. The libhv event loop runs connections on its worker thread
    // and only hands us request lines via the queue.
    //
    // Idle auto-shutdown: when liveConnections sits at 0 for idleTimeoutSecs (and no work is
    // queued), exit. `idleZeroSince` is the steady_clock time the count last reached zero, or
    // a null optional while connections are live. Any reconnect resets it (count > 0).
    std::optional<std::chrono::steady_clock::time_point> idleZeroSince;
    if (idleTimeoutSecs > 0 && liveConnections.load() == 0)
        idleZeroSince = std::chrono::steady_clock::now();

    while (!stopRequested.load())
    {
        PendingRequest pending;
        bool have = false;
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (!requestQueue.empty())
            {
                pending = std::move(requestQueue.front());
                requestQueue.pop_front();
                have = true;
            }
        }
        if (!have)
        {
            // Idle-shutdown bookkeeping: track the moment connections last hit zero; clear it
            // whenever a connection is live (so a reconnect cancels the pending shutdown).
            if (idleTimeoutSecs > 0)
            {
                if (liveConnections.load() == 0)
                {
                    if (!idleZeroSince)
                        idleZeroSince = std::chrono::steady_clock::now();
                    auto idleSecs = std::chrono::duration_cast<std::chrono::seconds>(
                                        std::chrono::steady_clock::now() - *idleZeroSince)
                                        .count();
                    if (idleSecs >= idleTimeoutSecs)
                    {
                        std::fprintf(stderr, "[DuinDasHost] idle %llds at zero connections; shutting down\n",
                                     (long long)idleSecs);
                        break;
                    }
                }
                else
                {
                    idleZeroSince.reset();
                }
            }
            hv_msleep(10);
            continue;
        }

        RequestResult r = HandleRequestLine(script, pending.msg, absProject);
        if (r.quit)
        {
            if (pending.channel)
                pending.channel->send(std::string("{\"bye\":true}"));
            stopRequested.store(true);
            break;
        }
        if (!r.response.empty() && pending.channel)
            pending.channel->send(r.response);
    }

    server.stop();
    std::error_code ec;
    std::filesystem::remove(std::filesystem::u8path(ddhPath), ec);
    std::fprintf(stderr, "[DuinDasHost] shut down; removed %s\n", ddhPath.c_str());
    return 0;
}

int main(int argc, char **argv)
{
    Args args = ParseArgs(argc, argv);
    if (!args.valid)
    {
        std::fprintf(
            stderr,
            "usage: DuinDasHost <file.das> [--project <path.das_project>] "
            "[--dasroot <path>] [--json]\n"
            "       DuinDasHost --serve     [--project <path.das_project>] [--dasroot <path>]\n"
            "       DuinDasHost --serve-tcp  --project <path.das_project>  [--dasroot <path>]\n"
            "       DuinDasHost --serve-ws   --project <path.das_project>  [--dasroot <path>]\n"
            "         --serve (stdin): warm up once, then read one request per stdin line:\n"
            "           {\"file\":\"<path.das>\",\"project\":\"<path.das_project>\"} (project optional)\n"
            "           emits {\"ready\":true} once, then one result-JSON line per request;\n"
            "           {\"quit\":true} or EOF exits.\n"
            "         --serve-tcp: per-project daemon on 127.0.0.1:<free-port> (bespoke line\n"
            "           protocol); writes <project>.das_project.ddh {pid,port,transport:tcp,...};\n"
            "           one request per connection ({\"file\":...} | {\"ping\":true} | {\"quit\":true}).\n"
            "         --serve-ws (editor LSP proxy): same requests over WebSocket; .ddh carries\n"
            "           transport:ws + wsUrl (ws://127.0.0.1:<port>). One frame in, one frame out.\n"
            "           --idle-timeout <secs>: exit after <secs> at zero connections (default 30,\n"
            "           0 = never); a spawn-race lock (<project>.das_project.ddh.lock) is held\n"
            "           while running so concurrent editors share one daemon.\n");
        return 2;
    }

    // Resolve the daslang root (arg → env → derived from exe layout). Without it,
    // daslang cannot find daslib/builtin.das and every compile fails internally.
    std::string dasRoot = ResolveDasRoot(args.dasRoot);
    if (dasRoot.empty())
    {
        std::fprintf(
            stderr,
            "error: could not locate daslang root (daslib/builtin.das).\n"
            "  pass --dasroot <path>, set DUIN_DAS_ROOT, or run from the build tree.\n");
        return 2;
    }

    // Engine init (headless — replicates EntryPoint.h without the Application run loop).
    duin::Log::Init();
    duin::InitAsserts();
    duin::SetFlecsDaslangAssert();
    das::install_das_crash_handler();

    // Resolve project/script paths against the shell CWD. daslang anchors the
    // project file's module_get() relative paths to the project file's directory,
    // so a relative project path leaves engine modules (flecs_boost, imgui_boost,
    // dn_*) unresolvable; an absolute one fixes the anchor regardless of CWD.
    std::string defaultProjectPath = MakeAbsoluteUnix(args.project);

    // Spawn-race guard for --serve-ws: claim the lock BEFORE the ~4s warm-up so a daemon
    // that loses the race exits in milliseconds instead of wastefully warming a Script it
    // would throw away. The SpawnLock (RAII) removes the lock on every exit path and lives
    // for the whole daemon lifetime. (Other modes don't use the lock.)
    SpawnLock spawnLock;
    if (args.serveWs)
    {
        if (defaultProjectPath.empty())
        {
            std::fprintf(stderr, "error: --serve-ws requires --project (the .das_project is the daemon identity)\n");
            return 2;
        }
        if (!TryAcquireSpawnLock(DdhLockPathFor(defaultProjectPath), DdhPathFor(defaultProjectPath), /*staleSecs=*/30))
        {
            std::fprintf(stderr, "[DuinDasHost] another daemon is starting/serving this project; exiting\n");
            return 0;
        }
        spawnLock.path = DdhLockPathFor(defaultProjectPath);
        spawnLock.held = true;
    }

    // Warm up once: register engine modules and set the das root. This is the ~4s
    // cost (daslib + engine module graph); in serve mode it is paid a single time
    // and every subsequent request reuses the warm base module set.
    duin::Script script;
    {
        StdoutToStderrGuard guard;
        std::fprintf(stderr, "[DuinDasHost] resolved dasRoot = '%s'\n", dasRoot.c_str());
        script.SetDasRoot(dasRoot);
        std::fprintf(stderr, "[DuinDasHost] getDasRoot() after SetDasRoot = '%s'\n", das::getDasRoot().c_str());
        if (!defaultProjectPath.empty())
        {
            std::fprintf(stderr, "[DuinDasHost] project = '%s'\n", defaultProjectPath.c_str());
        }
        script.InitModules([]() { RegisterEngineModules(); });
        std::fprintf(stderr, "[DuinDasHost] getDasRoot() after InitModules = '%s'\n", das::getDasRoot().c_str());
        guard.Restore();
    }

    // WebSocket serve mode: per-project daemon + .ddh; the editor LSP proxy connects here.
    if (args.serveWs)
        return RunServeWs(script, defaultProjectPath, args.idleTimeoutSecs);

    // TCP serve mode: per-project daemon on 127.0.0.1:0 + .ddh discovery (bespoke protocol).
    if (args.serveTcp)
        return RunServeTcp(script, defaultProjectPath);

    // stdin serve mode: warm up once, then compile per stdin request (single-client).
    if (args.serve)
        return RunServeLoop(script, defaultProjectPath);

    // One-shot mode: compile the single file given on the command line.
    std::string scriptPath = MakeAbsoluteUnix(args.file);
    bool success = false;
    // One-shot: nothing to keep warm, so re-read everything (RefreshBindings) for correctness.
    std::string resultJson =
        CompileOne(script, args.file, scriptPath, defaultProjectPath, duin::RecompileMode::RefreshBindings, &success);

    std::fprintf(stdout, "%s\n", resultJson.c_str());
    std::fflush(stdout);

    return success ? 0 : 1;
}
