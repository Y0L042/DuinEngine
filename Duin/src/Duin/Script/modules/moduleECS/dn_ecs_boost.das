options gen2
options indenting = 4
options no_unused_block_arguments = false
options no_unused_function_arguments = false
options strict_smart_pointers = true

module dn_ecs_boost shared private

require daslib/ast_boost
require daslib/templates_boost
require daslib/strings_boost
require daslib/ast_block_to_loop
require daslib/macro_boost

require dn_ecs

/*
    ecs_query(world) $(eid : ecs_entity_t; vel : Velocity3D; var tx : Transform3D) {
        tx.pos += vel.val
    }

    Argument convention:
        param : Type        -> read-only term (EcsIn), bound as `Type const&`
        var param : Type    -> mutable term (EcsInOut), bound as `Type&`
        eid : ecs_entity_t  -> binds the current entity id (no query term)

    Optional explicit cache key (default key is per call site):
        ecs_query(world, "my_query") $(...) { }

    Block annotations:
        [CACHED]            -> flecs EcsQueryCacheAuto
        [REQUIRE(Tag)]      -> entity must have component/tag, no data binding
        [REQUIRE_NOT(Tag)]  -> entity must NOT have component/tag

    Each call site lazily builds its flecs query once via _dn_ecs_query_get
    (dn_ecs), keyed by world pointer + site key. `return` inside the block
    skips to the next entity.

    Related macros:

    // First match wins; yields bool. `return <cond>` stops with true.
    let found = ecs_find_query(world) $(pos : TPos) {
        return pos.val.x > 40f
    }

    // Named per-frame system (Duin-level registry, runs via dn_ecs_progress).
    // The block is an ecs_query body; `dt : double` is the frame delta.
    ecs_system(world, "integrate", 0) $(vel : Velocity3D; var tx : Transform3D) {
        tx.pos += vel.val * float(dt)
    }
    dn_ecs_progress(world, delta)   // each frame

    // Component lifecycle observer (one component arg + optional entity arg).
    // LIMITATION: fires only via DnEntity.add/set/remove, not raw flecs calls.
    ecs_observer(EcsHookEvent.ONSET) $(eid : ecs_entity_t; v : Velocity3D) { ... }
*/

[block_macro(name="CACHED")]
class DnEcsCached : AstBlockAnnotation {
    //! Marks an ecs_query block's underlying flecs query as cached (EcsQueryCacheAuto).
}

[block_macro(name="REQUIRE")]
class DnEcsRequire : AstBlockAnnotation {
    //! List of components/tags the entity must have, without binding their data.
}

[block_macro(name="REQUIRE_NOT")]
class DnEcsRequireNot : AstBlockAnnotation {
    //! List of components/tags the entity must NOT have.
}

[macro_function]
def private is_entity_arg(a : VariablePtr) : bool {
    if (a._type == null) {
        return false
    }
    if (a._type.baseType == Type.tUInt64) {
        return true
    }
    // pre-infer the alias may not be resolved yet
    return (a._type.baseType == Type.alias) && (a._type.alias == "ecs_entity_t")
}

[macro_function]
def private block_has_annotation(qblk : ExprBlock?; name : string) : bool {
    for (aa in qblk.annotations) {
        if (aa.annotation.name == name) {
            return true
        }
    }
    return false
}

[macro_function]
def private gather_annotation_names(qblk : ExprBlock?; name : string; var names : array<string>) {
    for (aa in qblk.annotations) {
        if (aa.annotation.name == name) {
            for (ab in aa.arguments) {
                if (ab.basicType == Type.tBool && ab.bValue) {
                    names |> push(string(ab.name))
                }
            }
        }
    }
}

[macro_function]
def private force_ref_on_block_args(var expr : ExprCallMacro?; minArgs, maxArgs : int) {
    let totalArgs = length(expr.arguments)
    if (totalArgs < minArgs || totalArgs > maxArgs) {
        return
    }
    let block_arg_idx = totalArgs - 1
    if (!(expr.arguments[block_arg_idx] is ExprMakeBlock)) {
        return
    }
    var mblk = expr.arguments[block_arg_idx] as ExprMakeBlock
    var qblk = mblk._block as ExprBlock
    for (arg in qblk.arguments) {
        if (arg._type != null && !is_entity_arg(arg)) {
            // non-const component args are bound by reference
            if (!arg._type.flags.constant) {
                arg._type.flags.ref = true
            }
        }
    }
}

[call_macro(name="ecs_query")]
class DnEcsQueryMacro : AstCallMacro {
    //! Implements `ecs_query` — cached flecs query construction + inline iteration.
    def override preVisit(prog : ProgramPtr; mod : Module?; var expr : ExprCallMacro?) : void {
        force_ref_on_block_args(expr, 2, 3)
    }

    def override visit(prog : ProgramPtr; mod : Module?; var expr : ExprCallMacro?) : ExpressionPtr {
        let totalArgs = length(expr.arguments)
        macro_verify(
            totalArgs >= 2 && totalArgs <= 3,
            prog,
            expr.at,
            "expecting ecs_query(world,$(block_with_arguments)) or ecs_query(world,\"key\",$(block_with_arguments))"
        )
        return implement(expr, totalArgs - 1, totalArgs == 3 ? 1 : -1, false)
    }

    def implement(var expr : ExprCallMacro?, block_arg_idx : int, key_arg_idx : int, find_mode : bool) : ExpressionPtr {
        for (arg in expr.arguments) {
            macro_verify(!arg._type.isAutoOrAlias, compiling_program(), expr.at, "ecs_query: argument types are not fully inferred")
        }
        macro_verify(expr.arguments[block_arg_idx] is ExprMakeBlock, compiling_program(), expr.at, "ecs_query: expecting $(block_with_arguments)")

        let mblk = expr.arguments[block_arg_idx] as ExprMakeBlock
        let qblk = mblk._block as ExprBlock
        macro_verify(!empty(qblk.arguments), compiling_program(), expr.at, "ecs_query: block must declare at least one component argument")

    // site key: explicit string literal, or module:line:column
        var site_key : string
        if (key_arg_idx >= 0) {
            macro_verify(expr.arguments[key_arg_idx] is ExprConstString, compiling_program(), expr.at, "ecs_query: query key must be a string literal")
            site_key = "{(expr.arguments[key_arg_idx] as ExprConstString).value}"
        } else {
            site_key = "{compiling_module().name}:{expr.at.line}:{expr.at.column}"
        }
        let uniq = "_{expr.at.line}_{expr.at.column}"

    // names declared by the quote skeleton get renamed to these; everything
    // built programmatically below uses the final unique names directly
        let itname = "_dn_it{uniq}"
        let iname = "_dn_i{uniq}"

    // single pass over block arguments: classify, build query terms,
    // per-entity ref bindings (qlbody) and per-batch field getters (whilebody)
        var builder_chain : ExpressionPtr
        var qlbody = new ExprBlock(at = qblk.at)
        var whilebody = new ExprBlock(at = qblk.at)
        var entity_name : string = ""
        var comp_count = 0
        unsafe {
            builder_chain = qmacro(DnWorld($e(clone_expression(expr.arguments[0]))).query_builder())
        }
        for (a in qblk.arguments) {
            macro_verify(a._type != null && !a._type.isAuto, compiling_program(), a.at, "ecs_query: argument {a.name} must have an explicit type")
            if (is_entity_arg(a)) {
                macro_verify(empty(entity_name), compiling_program(), a.at, "ecs_query: only one ecs_entity_t argument is allowed")
                macro_verify(a._type.flags.constant, compiling_program(), a.at, "ecs_query: entity argument {a.name} can't be var")
                entity_name = string(a.name)
                var eget = qmacro($i(itname).entities[$i(iname)])
                eget.genFlags.alwaysSafe = true
                force_generated(eget, true)
                var etype = clone_type(a._type)
                etype.flags.ref = false
                var elet = new ExprLet(at = a.at, atInit = a.at)
                elet.genFlags.alwaysSafe = true
                elet.variables |> emplace_new() <| new Variable(at = a.at,
                    name := string(a.name),
                    _type <- etype,
                    init <- eget,
                    flags = VariableFlags.can_shadow | VariableFlags.generated
                    )
                qlbody.list |> emplace(elet)
                continue
            }
            macro_verify(!(a._type.flags.ref && a._type.flags.constant), compiling_program(), a.at, "ecs_query: argument {a.name} is both & and constant")
            macro_verify(a._type.baseType == Type.tStructure, compiling_program(), a.at, "ecs_query: argument {a.name} must be a component struct (or ecs_entity_t)")
            let idx = comp_count
            comp_count++
            let fname = "_dn_f{idx}{uniq}"
        // query term
            var ttype = clone_type(a._type)
            ttype.flags.constant = false
            ttype.flags.ref = false
            unsafe {
                if (a._type.flags.constant) {
                    builder_chain = qmacro($e(builder_chain).with_term(type<$t(ttype)>, EcsIn))
                } else {
                    builder_chain = qmacro($e(builder_chain).with_term(type<$t(ttype)>, EcsInOut))
                }
            }
        // per-entity ref binding: let name : T& = _dn_fK[_dn_i]
            var iget = qmacro($i(fname)[$i(iname)])
            iget.genFlags.alwaysSafe = true
            force_generated(iget, true)
            var vtype = clone_type(a._type)
            vtype.flags.ref = true
            var vlet = new ExprLet(at = a.at, atInit = a.at)
            vlet.genFlags.alwaysSafe = true
            vlet.variables |> emplace_new() <| new Variable(at = a.at,
                name := string(a.name),
                _type <- vtype,
                init <- iget,
                flags = VariableFlags.can_shadow | VariableFlags.generated
                )
            qlbody.list |> emplace(vlet)
        // per-batch field getter: var _dn_fK : T? = ecs_field_get(_dn_it, type<T>, K)
            var ftype = clone_type(a._type)
            ftype.flags.constant = false
            ftype.flags.ref = false
            var fget = qmacro(ecs_field_get($i(itname), type<$t(ftype)>, $v(idx)))
            force_generated(fget, true)
            var ptype = new TypeDecl(at = a.at, baseType = Type.tPointer)
            ptype.firstType = clone_type(ftype)
            var flet = new ExprLet(at = a.at, atInit = a.at)
            flet.genFlags.alwaysSafe = true
            flet.variables |> emplace_new() <| new Variable(at = a.at,
                name := fname,
                _type <- ptype,
                init <- fget,
                flags = VariableFlags.can_shadow | VariableFlags.generated
                )
            whilebody.list |> emplace(flet)
        }
        macro_verify(comp_count > 0, compiling_program(), expr.at, "ecs_query: block must declare at least one component argument")

    // filter terms from annotations, then finish the builder chain
        unsafe {
            var req_names : array<string>
            gather_annotation_names(qblk, "REQUIRE", req_names)
            for (nm in req_names) {
                var td = new TypeDecl(at = expr.at, baseType = Type.alias, alias := nm)
                builder_chain = qmacro($e(builder_chain).with_term(type<$t(td)>, EcsInOutNone))
            }
            var reqn_names : array<string>
            gather_annotation_names(qblk, "REQUIRE_NOT", reqn_names)
            for (nm in reqn_names) {
                var td = new TypeDecl(at = expr.at, baseType = Type.alias, alias := nm)
                builder_chain = qmacro($e(builder_chain).without_term(type<$t(td)>))
            }
            if (block_has_annotation(qblk, "CACHED")) {
                builder_chain = qmacro($e(builder_chain).cached())
            }
            builder_chain = qmacro($e(builder_chain).build())
        }

    // cloned user statements run after the ref bindings
        for (l in qblk.list) {
            qlbody.list |> push(clone_expression(l))
        }
        for (fl in qblk.finalList) {
            qlbody.finalList |> push(clone_expression(fl))
        }
        // find mode: `return cond` -> stop-with-true / continue; plain mode: `return` -> next entity
        convert_block_to_loop(qlbody, false, true, find_mode)
        var qloop = new ExprFor(at = qblk.at, visibility = qblk.at)
        qloop.allowIteratorOptimization = true
        qloop.canShadow = true
        qloop.iterators |> resize(1)
        qloop.iterators[0] := iname
        qloop.iteratorsAka |> resize(1)
        qloop.iteratorsAka[0] := ""
        qloop.iteratorsAt |> push(qblk.at)
        qloop.iteratorsTags |> resize(1)
        qloop.sources |> emplace_new() <| qmacro(range($i(itname).count))
        qloop.body = qlbody
        whilebody.list |> emplace(qloop)

    // outer skeleton
        var qblock : ExpressionPtr
        unsafe {
            if (find_mode) {
                qblock = quote() {
                    invoke() <| $() : bool {
                        var _dn_q = _dn_ecs_query_get(tag_world, tag_key) <| $() : DnQuery {
                            return tag_builder
                        }
                        var _dn_it = ecs_query_iter(tag_world, _dn_q.ecs_query())
                        while (ecs_query_next_iter(_dn_it)) {
                            tag_whilebody
                        }
                        return false
                    }
                }
            } else {
                qblock = quote() {
                    var _dn_q = _dn_ecs_query_get(tag_world, tag_key) <| $() : DnQuery {
                        return tag_builder
                    }
                    var _dn_it = ecs_query_iter(tag_world, _dn_q.ecs_query())
                    while (ecs_query_next_iter(_dn_it)) {
                        tag_whilebody
                    }
                }
            }
        }
        qblock |> force_at(expr.at)
        apply_template(qblock) $(rules) {
            rules |> replaceVariable("tag_world") <| clone_expression(expr.arguments[0])
            rules |> replaceVariable("tag_key") <| new ExprConstString(at = expr.at, value := site_key)
            rules |> replaceVariable("tag_builder") <| builder_chain
            rules |> replaceVariable("tag_whilebody") <| whilebody
            rules |> renameVariable("_dn_q", "_dn_q{uniq}")
            rules |> renameVariable("_dn_it", itname)
        }
        var qres = move_unquote_block(qblock)
        qres |> force_at(expr.at)
        qres.genFlags.alwaysSafe = true
        if (find_mode) {
            // single invoke expression yielding bool
            var rqres = qres.list[0]
            rqres.genFlags.alwaysSafe = true
            return rqres
        }
        return qres
    }
}

[call_macro(name="ecs_find_query")]
class DnEcsFindQueryMacro : DnEcsQueryMacro {
    //! Like `ecs_query`, but stops at the first match and yields a bool.
    //! Inside the block, `return <cond>` stops with `true` when cond holds,
    //! otherwise continues; falling off the end continues to the next entity.
    def override visit(prog : ProgramPtr; mod : Module?; var expr : ExprCallMacro?) : ExpressionPtr {
        let totalArgs = length(expr.arguments)
        macro_verify(
            totalArgs >= 2 && totalArgs <= 3,
            prog,
            expr.at,
            "expecting ecs_find_query(world,$(block_with_arguments)) or ecs_find_query(world,\"key\",$(block_with_arguments))"
        )
        return implement(expr, totalArgs - 1, totalArgs == 3 ? 1 : -1, true)
    }
}

[call_macro(name="ecs_system")]
class DnEcsSystemMacro : DnEcsQueryMacro {
    //! Registers a named per-frame system: `ecs_system(world, "name", phase) $(...) { }`.
    //! The block is an ecs_query body; the lambda parameter `dt : double` is the
    //! frame delta. Registration is idempotent by (world, name); systems run in
    //! phase order via dn_ecs_progress(world, dt).
    def override preVisit(prog : ProgramPtr; mod : Module?; var expr : ExprCallMacro?) : void {
        force_ref_on_block_args(expr, 4, 4)
    }

    def override visit(prog : ProgramPtr; mod : Module?; var expr : ExprCallMacro?) : ExpressionPtr {
        macro_verify(
            length(expr.arguments) == 4,
            prog,
            expr.at,
            "expecting ecs_system(world,\"name\",phase,$(block_with_arguments))"
        )
        macro_verify(expr.arguments[1] is ExprConstString, prog, expr.at, "ecs_system: name must be a string literal")
        var expansion = implement(expr, 3, 1, false)
        if (expansion == null) {
            return null
        }
        let sys_name = "{(expr.arguments[1] as ExprConstString).value}"
        var qblock : ExpressionPtr
        unsafe {
            qblock = quote() {
                dn_ecs_register_system(tag_world, tag_name, tag_phase) <| @(dt : double) : void {
                    tag_sysbody
                }
            }
        }
        qblock |> force_at(expr.at)
        apply_template(qblock) $(rules) {
            rules |> replaceVariable("tag_world") <| clone_expression(expr.arguments[0])
            rules |> replaceVariable("tag_name") <| new ExprConstString(at = expr.at, value := sys_name)
            rules |> replaceVariable("tag_phase") <| clone_expression(expr.arguments[2])
            rules |> replaceVariable("tag_sysbody") <| expansion
        }
        var qres = move_unquote_block(qblock)
        qres |> force_at(expr.at)
        qres.genFlags.alwaysSafe = true
        return qres
    }
}

[call_macro(name="ecs_observer")]
class DnEcsObserverMacro : AstCallMacro {
    //! Component lifecycle observer: `ecs_observer(<EcsHookEvent>) $(eid : ecs_entity_t; cmp : T) { }`.
    //! Lowers onto the dn hook registry (ecs_add_hook). LIMITATION: hooks fire
    //! only through DnEntity.add/set/remove, NOT through raw flecs mutations.
    def override visit(prog : ProgramPtr; mod : Module?; var expr : ExprCallMacro?) : ExpressionPtr {
        macro_verify(
            length(expr.arguments) == 2,
            prog,
            expr.at,
            "expecting ecs_observer(event,$(block_with_arguments))"
        )
        macro_verify(expr.arguments[1] is ExprMakeBlock, compiling_program(), expr.at, "ecs_observer: expecting $(block_with_arguments)")
        let mblk = expr.arguments[1] as ExprMakeBlock
        let qblk = mblk._block as ExprBlock

        var entity_name : string = ""
        var comp_count = 0
        var ctype : TypeDeclPtr
        for (a in qblk.arguments) {
            macro_verify(a._type != null && !a._type.isAuto, compiling_program(), a.at, "ecs_observer: argument {a.name} must have an explicit type")
            macro_verify(a._type.flags.constant, compiling_program(), a.at, "ecs_observer: argument {a.name} can't be var (hooks receive a value copy)")
            if (is_entity_arg(a)) {
                macro_verify(empty(entity_name), compiling_program(), a.at, "ecs_observer: only one ecs_entity_t argument is allowed")
                entity_name = string(a.name)
            } else {
                comp_count++
                ctype = clone_type(a._type)
                ctype.flags.constant = false
                ctype.flags.ref = false
            }
        }
        macro_verify(comp_count == 1, compiling_program(), expr.at, "ecs_observer: block must declare exactly one component argument")

    // hook callback body: bind user arg names, then the cloned user statements
        var cbody = new ExprBlock(at = qblk.at)
        for (a in qblk.arguments) {
            var atype = clone_type(a._type)
            atype.flags.ref = false
            var src : ExpressionPtr
            unsafe {
                if (is_entity_arg(a)) {
                    src = qmacro(_dn_he)
                } else {
                    src = qmacro(_dn_hc)
                }
            }
            force_generated(src, true)
            var alet = new ExprLet(at = a.at, atInit = a.at)
            alet.genFlags.alwaysSafe = true
            alet.variables |> emplace_new() <| new Variable(at = a.at,
                name := string(a.name),
                _type <- atype,
                init <- src,
                flags = VariableFlags.can_shadow | VariableFlags.generated
                )
            cbody.list |> emplace(alet)
        }
        for (l in qblk.list) {
            cbody.list |> push(clone_expression(l))
        }
        for (fl in qblk.finalList) {
            cbody.finalList |> push(clone_expression(fl))
        }
        convert_block_to_loop(cbody, true, false, false)

        var qblock : ExpressionPtr
        unsafe {
            qblock = quote() {
                ecs_add_hook(type<tag_cmp_type>, tag_event) <| @(var _dn_hw : ecs_world_t?; var _dn_he : ecs_entity_t; _dn_hc : tag_cmp_type_c) {
                    tag_cbody
                }
            }
        }
        qblock |> force_at(expr.at)
        // the rule replaces the whole TypeDecl including flags, so the lambda's
        // const cmp param needs its own const-flagged copy of the type
        var ctype_c = clone_type(ctype)
        ctype_c.flags.constant = true
        apply_template(qblock) $(rules) {
            rules |> replaceTypeWithTypeDecl("tag_cmp_type") <| ctype
            rules |> replaceTypeWithTypeDecl("tag_cmp_type_c") <| ctype_c
            rules |> replaceVariable("tag_event") <| clone_expression(expr.arguments[0])
            rules |> replaceVariable("tag_cbody") <| cbody
        }
        var qres = move_unquote_block(qblock)
        qres |> force_at(expr.at)
        qres.genFlags.alwaysSafe = true
        return qres
    }
}
