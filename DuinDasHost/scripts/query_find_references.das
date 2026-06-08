// query_find_references.das — daslang find-references query for DuinDasHost.
// Spawned by the daemon for textDocument/references requests.
// Args: <daslang_exe> <this_script> <file> <line> <col> <project>
// Stdout: one JSON line:
//   {"locations":[{"file":"...","line":N,"column":N},...]}

options gen2
options rtti
options no_unused_function_arguments = false
options no_unused_block_arguments = false

require daslib/ast_cursor
require daslib/ast_boost
require daslib/strings_boost
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

def to_voidp(p) : void? {
    return unsafe(reinterpret<void?> p)
}

enum RefKind {
    none
    rfunc
    rvariable
    rstructure
    rfield
    renumeration
}

struct RefTarget {
    kind : RefKind
    ptr : void?
    field_struct_ptr : void?
    field_name : string
}

def identify_target(hit : CursorHit) : RefTarget {
    let expr = hit.expr
    if (expr == null) { return RefTarget(); }

    if (expr is ExprCall) {
        let c = expr as ExprCall
        if (c.func != null) { return RefTarget(kind = RefKind.rfunc, ptr = to_voidp(c.func)); }
    }
    if (expr is ExprVar) {
        let v = expr as ExprVar
        if (v.variable != null) { return RefTarget(kind = RefKind.rvariable, ptr = to_voidp(v.variable)); }
    }
    if (expr is ExprField) {
        let f = expr as ExprField
        var vt = f.value != null ? f.value._type : null
        if (vt != null && vt.structType == null && vt.firstType != null) { vt = vt.firstType; }
        if (vt != null && vt.structType != null) {
            return RefTarget(kind = RefKind.rfield, field_struct_ptr = to_voidp(vt.structType), field_name = string(f.name))
        }
    }
    if (expr is ExprSafeField) {
        let f = expr as ExprSafeField
        var vt = f.value != null ? f.value._type : null
        if (vt != null && vt.structType == null && vt.firstType != null) { vt = vt.firstType; }
        if (vt != null && vt.structType != null) {
            return RefTarget(kind = RefKind.rfield, field_struct_ptr = to_voidp(vt.structType), field_name = string(f.name))
        }
    }
    if (expr is ExprConstEnumeration) {
        let e = expr as ExprConstEnumeration
        if (e.enumType != null) { return RefTarget(kind = RefKind.renumeration, ptr = to_voidp(e.enumType)); }
    }
    if (expr._type != null) {
        let td = expr._type
        if (td.structType != null) { return RefTarget(kind = RefKind.rstructure, ptr = to_voidp(td.structType)); }
        if (td.enumType != null) { return RefTarget(kind = RefKind.renumeration, ptr = to_voidp(td.enumType)); }
    }
    return RefTarget()
}

struct RefLoc {
    file : string
    line : int
    column : int
}

class RefVisitor : AstVisitor {
    target : RefTarget
    results : array<RefLoc>

    def add_ref(at : LineInfo) {
        if (int(at.line) == 0) { return ; }
        let fname = at.fileInfo != null ? string(at.fileInfo.name) : ""
        results |> emplace(RefLoc(file = fname, line = int(at.line), column = int(at.column)))
    }

    def override preVisitExprCall(expr : ExprCall?) {
        if (target.kind == RefKind.rfunc && expr.func != null) {
            if (to_voidp(expr.func) == target.ptr) { add_ref(expr.at); }
        }
    }

    def override preVisitExprVar(expr : ExprVar?) {
        if (target.kind == RefKind.rvariable && expr.variable != null) {
            if (to_voidp(expr.variable) == target.ptr) { add_ref(expr.at); }
        }
    }

    def override preVisitExprField(expr : ExprField?) {
        if (target.kind == RefKind.rfield && expr.name == target.field_name) {
            var vt = expr.value != null ? expr.value._type : null
            if (vt != null && vt.structType == null && vt.firstType != null) { vt = vt.firstType; }
            if (vt != null && vt.structType != null && to_voidp(vt.structType) == target.field_struct_ptr) {
                add_ref(expr.atField)
            }
        }
    }

    def override preVisitExprSafeField(expr : ExprSafeField?) {
        if (target.kind == RefKind.rfield && expr.name == target.field_name) {
            var vt = expr.value != null ? expr.value._type : null
            if (vt != null && vt.structType == null && vt.firstType != null) { vt = vt.firstType; }
            if (vt != null && vt.structType != null && to_voidp(vt.structType) == target.field_struct_ptr) {
                add_ref(expr.atField)
            }
        }
    }

    def override preVisitExprConstEnumeration(expr : ExprConstEnumeration?) {
        if (target.kind == RefKind.renumeration && expr.enumType != null) {
            if (to_voidp(expr.enumType) == target.ptr) { add_ref(expr.at); }
        }
    }

    def override preVisitTypeDecl(typ : TypeDeclPtr) {
        let td = typ
        if (target.kind == RefKind.rstructure && td.structType != null) {
            if (to_voidp(td.structType) == target.ptr) { add_ref(td.at); }
        }
        if (target.kind == RefKind.renumeration && td.enumType != null) {
            if (to_voidp(td.enumType) == target.ptr) { add_ref(td.at); }
        }
    }
}

[export]
def main {
    let args <- get_command_line_arguments()
    // args[0]=exe args[1]=script args[2]=file args[3]=line args[4]=col args[5]=project
    if (length(args) < 6) { print("\{\"locations\":[]}\n"); return ; }
    let file = args[2]
    let line = to_int(args[3])
    let col = to_int(args[4])
    let project = args[5]
    if (line <= 0 || col <= 0) { print("\{\"locations\":[]}\n"); return ; }

    var inscope access <- make_file_access(project)
    using() $(var mg : ModuleGroup) {
        using() $(var cop : CodeOfPolicies) {
            cop.rtti = true
            cop.ignore_shared_modules = true
            compile_file(file, access, unsafe(addr(mg)), cop) $(ok; program; issues) {
                if (!ok) { print("\{\"locations\":[]}\n"); return ; }
                var hits <- find_at_cursor(program, file, line, col)
                if (empty(hits)) { print("\{\"locations\":[]}\n"); return ; }
                var target : RefTarget
                for (hit in hits) {
                    target = identify_target(hit)
                    if (target.kind != RefKind.none) { break; }
                }
                if (target.kind == RefKind.none) { print("\{\"locations\":[]}\n"); return ; }

                var visitor = new RefVisitor()
                visitor.target = target
                make_visitor(*visitor) $(adapter) {
                    visit(program, adapter)
                }
                var entries : array<string>
                for (r in visitor.results) {
                    entries |> push("\{\"file\":\"{json_escape(r.file)}\",\"line\":{r.line},\"column\":{r.column}}")
                }
                print("\{\"locations\":[{join(entries, ",")}]}\n")
                unsafe { delete visitor; }
            }
        }
    }
}
