// query_type_of.das — daslang hover query for DuinDasHost.
// Spawned by the daemon for textDocument/hover requests.
// Args: <daslang_exe> <this_script> <file> <line> <col> <project>
// Stdout: one JSON line {"hover":"<text>"} or {"hover":""}

options gen2
options rtti
options no_unused_function_arguments = false
options no_unused_block_arguments = false

require daslib/ast_cursor
require daslib/ast_boost
require strings

def json_escape(s : string) : string {
    var out = ""
    for (c in s) {
        if (c == '"') { out += "\\\""; }
        elif (c == '\\') { out += "\\\\"; }
        elif (c == '\n') { out += "\\n"; }
        elif (c == '\r') { out += "\\r"; }
        elif (c == '\t') { out += "\\t"; }
        else { out += string(c); }
    }
    return out
}

def lineinfo_file(at : LineInfo) : string {
    if (at.fileInfo != null) { return string(at.fileInfo.name); }
    return ""
}

def describe_hit(hit : CursorHit) : string {
    let expr = hit.expr
    if (expr == null) { return ""; }
    var result = ""
    if (expr._type != null) {
        result = describe(expr._type)
        let td = expr._type
        if (td.structType != null) {
            let sf = lineinfo_file(td.structType.at)
            if (!empty(sf)) { result += "  // {sf}:{int(td.structType.at.line)}"; }
        } elif (td.enumType != null) {
            let ef = lineinfo_file(td.enumType.at)
            if (!empty(ef)) { result += "  // {ef}:{int(td.enumType.at.line)}"; }
        }
    } else {
        result = "(no type)"
    }
    if (!empty(hit.name)) { return "{hit.name} : {result}"; }
    return result
}

[export]
def main {
    let args <- get_command_line_arguments()
    // args[0]=exe args[1]=script args[2]=file args[3]=line args[4]=col args[5]=project
    if (length(args) < 6) { print("\{\"hover\":\"\"}\n"); return ; }
    let file = args[2]
    let line = to_int(args[3])
    let col = to_int(args[4])
    let project = args[5]
    if (line <= 0 || col <= 0) { print("\{\"hover\":\"\"}\n"); return ; }

    var inscope access <- make_file_access(project)
    using() $(var mg : ModuleGroup) {
        using() $(var cop : CodeOfPolicies) {
            cop.rtti = true
            cop.ignore_shared_modules = true
            compile_file(file, access, unsafe(addr(mg)), cop) $(ok; program; issues) {
                if (!ok) { print("\{\"hover\":\"\"}\n"); return ; }
                var hits <- find_at_cursor(program, file, line, col)
                if (empty(hits)) { print("\{\"hover\":\"\"}\n"); return ; }
                let text = describe_hit(hits[0])
                print("\{\"hover\":\"{json_escape(text)}\"}\n")
            }
        }
    }
}
