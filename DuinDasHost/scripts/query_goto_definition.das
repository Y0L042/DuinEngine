// query_goto_definition.das — daslang goto-definition query for DuinDasHost.
// Spawned by the daemon for textDocument/definition requests.
// Args: <daslang_exe> <this_script> <file> <line> <col> <project>
// Stdout: one JSON line:
//   {"found":true,"file":"...","line":N,"column":N}
//   {"found":false}

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

struct DefLoc {
    found : bool
    file : string
    line : int
    column : int
}

def resolve_definition(hit : CursorHit) : DefLoc {
    let expr = hit.expr
    if (expr == null) { return DefLoc(); }

    // ExprVar -> variable declaration
    if (expr is ExprVar) {
        let v = expr as ExprVar
        if (v.variable != null) {
            let vr = v.variable
            return DefLoc(found = true, file = lineinfo_file(vr.at), line = int(vr.at.line), column = int(vr.at.column))
        }
    }

    // ExprCall -> function declaration
    if (expr is ExprCall) {
        let c = expr as ExprCall
        if (c.func != null && !c.func.flags.builtIn) {
            let fn = c.func
            return DefLoc(found = true, file = lineinfo_file(fn.at), line = int(fn.at.line), column = int(fn.at.column))
        }
    }

    // ExprField -> struct field
    if (expr is ExprField) {
        let f = expr as ExprField
        var vt = f.value != null ? f.value._type : null
        if (vt != null && vt.structType == null && vt.firstType != null) { vt = vt.firstType; }
        if (vt != null && vt.structType != null) {
            let st = vt.structType
            for (fld in st.fields) {
                if (fld.name == f.name) {
                    return DefLoc(found = true, file = lineinfo_file(fld.at), line = int(fld.at.line), column = int(fld.at.column))
                }
            }
        }
    }

    // ExprSafeField -> struct field (?.)
    if (expr is ExprSafeField) {
        let f = expr as ExprSafeField
        var vt = f.value != null ? f.value._type : null
        if (vt != null && vt.structType == null && vt.firstType != null) { vt = vt.firstType; }
        if (vt != null && vt.structType != null) {
            let st = vt.structType
            for (fld in st.fields) {
                if (fld.name == f.name) {
                    return DefLoc(found = true, file = lineinfo_file(fld.at), line = int(fld.at.line), column = int(fld.at.column))
                }
            }
        }
    }

    // ExprConstEnumeration -> enum declaration
    if (expr is ExprConstEnumeration) {
        let e = expr as ExprConstEnumeration
        if (e.enumType != null) {
            let en = e.enumType
            return DefLoc(found = true, file = lineinfo_file(en.at), line = int(en.at.line), column = int(en.at.column))
        }
    }

    // Fallback: struct/enum type reference
    if (expr._type != null) {
        let td = expr._type
        if (td.structType != null) {
            let st = td.structType
            return DefLoc(found = true, file = lineinfo_file(st.at), line = int(st.at.line), column = int(st.at.column))
        }
        if (td.enumType != null) {
            let en = td.enumType
            return DefLoc(found = true, file = lineinfo_file(en.at), line = int(en.at.line), column = int(en.at.column))
        }
    }

    return DefLoc()
}

[export]
def main {
    let args <- get_command_line_arguments()
    // args[0]=exe args[1]=script args[2]=file args[3]=line args[4]=col args[5]=project
    if (length(args) < 6) { print("\{\"found\":false}\n"); return ; }
    let file = args[2]
    let line = to_int(args[3])
    let col = to_int(args[4])
    let project = args[5]
    if (line <= 0 || col <= 0) { print("\{\"found\":false}\n"); return ; }

    var inscope access <- make_file_access(project)
    using() $(var mg : ModuleGroup) {
        using() $(var cop : CodeOfPolicies) {
            cop.rtti = true
            cop.ignore_shared_modules = true
            compile_file(file, access, unsafe(addr(mg)), cop) $(ok; program; issues) {
                if (!ok) { print("\{\"found\":false}\n"); return ; }
                var hits <- find_at_cursor(program, file, line, col)
                if (empty(hits)) { print("\{\"found\":false}\n"); return ; }
                for (hit in hits) {
                    let defn = resolve_definition(hit)
                    if (defn.found && !empty(defn.file) && defn.line > 0) {
                        print("\{\"found\":true,\"file\":\"{json_escape(defn.file)}\",\"line\":{defn.line},\"column\":{defn.column}}\n")
                        return
                    }
                }
                print("\{\"found\":false}\n")
            }
        }
    }
}
