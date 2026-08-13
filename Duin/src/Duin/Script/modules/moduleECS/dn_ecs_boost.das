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

require flecs public
require dn_ecs public

/*
    ecs_query(world) $(ent : DnEntity; vel : Velocity3D; var tx : Transform3D) {
        tx.pos += vel.val
    }

    Argument convention:
        param : Type        -> read-only term (EcsIn), bound as `Type const&`
        var param : Type    -> mutable term (EcsInOut), bound as `Type&`
        ent : DnEntity      -> binds the current entity as DnEntity (no query term)
        eid : ecs_entity_t  -> binds the raw entity id instead (no query term)

    Optional explicit cache key (default key is per call site):
        ecs_query(world, "my_query") $(...) { }

    Block annotations:
        [CACHED]            -> flecs EcsQueryCacheAuto
        [REQUIRE(Tag)]      -> entity must have component/tag, no data binding
        [REQUIRE_NOT(Tag)]  -> entity must NOT have component/tag

    Relationship traversal -- read a component from an ANCESTOR instead of from
    the matched entity. Each annotation names the block arguments it applies to:
        [CASCADE(a, b)]     -> breadth-first, parents visited before children
        [CASCADE_DESC(a)]   -> same, but children visited before parents
        [UP(a)]             -> nearest ancestor's component, no ordering guarantee
        [SELF_UP(a)]        -> own component if present, else the ancestor's

    Traversal follows ChildOf and works with both flecs hierarchy storages.

    A traversal argument's optionality is written on the argument itself:

        parent_tx : Transform3D     -> required. Root entities have no ancestor
                                       to read from, so they DO NOT match the
                                       query at all. Bound as Transform3D&.
        parent_tx : Transform3D?    -> optional. Roots DO match, and parent_tx
                                       is null for them. Bound as Transform3D?.

    The "roots silently drop out" behaviour of the required form is the most
    common surprise here; prefer the `?` form whenever roots must be processed.

        // propagate position down a hierarchy, roots included
        ecs_query(world) $ [CACHED, CASCADE(parent_tx)] (
            var tx : Transform3D;
            var parent_tx : Transform3D?
        ) {
            if (parent_tx != null) {
                tx.pos = (*parent_tx).pos + tx.local
            }
        }

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
    ecs_observer(EcsHookEvent.ONSET) $(ent : DnEntity; v : Velocity3D) { ... }
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

[block_macro(name="CASCADE")]
class DnEcsCascade : AstBlockAnnotation {
    //! Names block arguments read from an ancestor, breadth-first with parents
    //! visited before children (flecs EcsCascade).
}

[block_macro(name="CASCADE_DESC")]
class DnEcsCascadeDesc : AstBlockAnnotation {
    //! Like CASCADE, but bottom-up: children are visited before parents.
}

[block_macro(name="UP")]
class DnEcsUp : AstBlockAnnotation {
    //! Names block arguments read from the nearest ancestor (flecs EcsUp),
    //! with no ordering guarantee.
}

[block_macro(name="SELF_UP")]
class DnEcsSelfUp : AstBlockAnnotation {
    //! Names block arguments read from the entity when it has the component,
    //! otherwise inherited from the nearest ancestor (flecs EcsSelf|EcsUp).
}

// `eid : DnEntity` — bound as a full DnEntity (world + id).
[macro_function]
def private is_dn_entity_arg(a : VariablePtr) : bool {
    if (a._type == null) {
        return false
    }
    if ((a._type.baseType == Type.tStructure) && (a._type.structType != null)) {
        return string(a._type.structType.name) == "DnEntity"
    }
    // pre-infer the alias may not be resolved yet
    return (a._type.baseType == Type.alias) && (a._type.alias == "DnEntity")
}

// `eid : ecs_entity_t` — bound as the raw uint64 id.
[macro_function]
def private is_raw_entity_arg(a : VariablePtr) : bool {
    if (a._type == null) {
        return false
    }
    return a._type.baseType == Type.tUInt64
}

[macro_function]
def private is_entity_arg(a : VariablePtr) : bool {
    return is_raw_entity_arg(a) || is_dn_entity_arg(a)
}

[macro_function]
def private block_has_annotation(query_block : ExprBlock?; name : string) : bool {
    for (annotation in query_block.annotations) {
        if (annotation.annotation.name == name) {
            return true
        }
    }
    return false
}

[macro_function]
def private gather_annotation_names(query_block : ExprBlock?; name : string; var names : array<string>) {
    for (annotation in query_block.annotations) {
        if (annotation.annotation.name == name) {
            for (annotation_arg in annotation.arguments) {
                if (annotation_arg.basicType == Type.tBool && annotation_arg.bValue) {
                    names |> push(string(annotation_arg.name))
                }
            }
        }
    }
}

// Record every argument named by a traversal annotation, mapping the argument
// name to the annotation that named it. A bareword annotation argument parses
// as a tBool-true whose *name* is the payload, exactly like REQUIRE(Tag).
[macro_function]
def private gather_traversal_names(query_block : ExprBlock?; annotation_name : string; var kinds : table<string; string>) {
    for (annotation in query_block.annotations) {
        if (annotation.annotation.name == annotation_name) {
            for (annotation_arg in annotation.arguments) {
                if (annotation_arg.basicType == Type.tBool && annotation_arg.bValue) {
                    kinds[string(annotation_arg.name)] = annotation_name
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
    var make_block = expr.arguments[block_arg_idx] as ExprMakeBlock
    var query_block = make_block._block as ExprBlock
    for (arg in query_block.arguments) {
        if (arg._type != null && !is_entity_arg(arg)) {
            // non-const component args are bound by reference. Pointer args are
            // left alone: they mark an optional traversal term and are bound as
            // a raw T? so the body can null-check them, and forcing ref here
            // would make them T?&. This runs before annotations are available,
            // which is why optionality keys off the pointer and not the marker.
            if (!arg._type.flags.constant && arg._type.baseType != Type.tPointer) {
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

    // Lowers a query call site into cached query construction plus an inline
    // iteration loop, in three stages:
    //
    //   1. classify each block argument (entity / component / traversal) and
    //      append a .with_term(...) call to `builder_chain`
    //   2. emit two bodies -- `per_batch_body` gets one ecs_field_get per term,
    //      `per_entity_body` gets one binding per argument -- and nest the
    //      latter inside the former via `entity_loop`
    //   3. splice both into a quote() `skeleton` with apply_template
    //
    // Names ending in _expr / _type / _decl hold pieces of GENERATED code, not
    // values: e.g. `element_expr` is the AST for `_dn_fK[i]`, not an element.
    // Identifiers inside quote() blocks and the "tag_*" template keys name
    // generated variables and must not be renamed alongside the locals here.
    //
    // Shared by ecs_query, ecs_find_query and ecs_system -- a change here
    // affects all three.
    def implement(var expr : ExprCallMacro?, block_arg_idx : int, key_arg_idx : int, find_mode : bool) : ExpressionPtr {
        for (arg in expr.arguments) {
            macro_verify(arg._type != null, compiling_program(), expr.at, "ecs_query: struct/type does not exist. Ensure that all component types are correct; no auto or alias (check spelling and such too) ")
            macro_verify(!arg._type.isAutoOrAlias, compiling_program(), expr.at, "ecs_query: argument types are not fully inferred ")
        }
        macro_verify(expr.arguments[block_arg_idx] is ExprMakeBlock, compiling_program(), expr.at, "ecs_query: expecting $(block_with_arguments)")

        let make_block = expr.arguments[block_arg_idx] as ExprMakeBlock
        let query_block = make_block._block as ExprBlock
        macro_verify(!empty(query_block.arguments), compiling_program(), expr.at, "ecs_query: block must declare at least one component argument")

    // site key: explicit string literal, or module:line:column
        var site_key : string
        if (key_arg_idx >= 0) {
            macro_verify(expr.arguments[key_arg_idx] is ExprConstString, compiling_program(), expr.at, "ecs_query: query key must be a string literal")
            site_key = "{(expr.arguments[key_arg_idx] as ExprConstString).value}"
        } else {
            site_key = "{compiling_module().name}:{expr.at.line}:{expr.at.column}"
        }
        let site_suffix = "_{expr.at.line}_{expr.at.column}"

    // names declared by the quote skeleton get renamed to these; everything
    // built programmatically below uses the final site_suffixue names directly
        let it_var = "_dn_it{site_suffix}"
        let index_var = "_dn_i{site_suffix}"
        let world_var = "_dn_wp{site_suffix}"

    // single pass over block arguments: classify, build query terms,
    // per-entity ref bindings (per_entity_body) and per-batch field getters (per_batch_body)
        var builder_chain : ExpressionPtr
        var per_entity_body = new ExprBlock(at = query_block.at)
        var per_batch_body = new ExprBlock(at = query_block.at)
        var entity_name : string = ""
        var comp_count = 0
        unsafe {
            builder_chain = qmacro(DnWorld($e(clone_expression(expr.arguments[0]))).query_builder())
        }
    // traversal markers: [CASCADE(a)] / [UP(a)] / ... name the arguments whose
    // component is sourced from an ancestor rather than the matched entity
        var traversal_by_arg : table<string; string>
        gather_traversal_names(query_block, "CASCADE", traversal_by_arg)
        gather_traversal_names(query_block, "CASCADE_DESC", traversal_by_arg)
        gather_traversal_names(query_block, "UP", traversal_by_arg)
        gather_traversal_names(query_block, "SELF_UP", traversal_by_arg)
        var traversal_seen : table<string; bool>
        for (a in query_block.arguments) {
            macro_verify(a._type != null && !a._type.isAuto, compiling_program(), a.at, "ecs_query: argument {a.name} must have an explicit type")
            if (is_entity_arg(a)) {
                macro_verify(empty(entity_name), compiling_program(), a.at, "ecs_query: only one entity argument (DnEntity or ecs_entity_t) is allowed")
                macro_verify(a._type.flags.constant, compiling_program(), a.at, "ecs_query: entity argument {a.name} can't be var")
                entity_name = string(a.name)
                var entity_expr : ExpressionPtr
                unsafe {
                    // the raw id read indexes a pointer, so it must be marked
                    // safe itself -- flagging only an enclosing call is not enough
                    var raw_id_expr = qmacro($i(it_var).entities[$i(index_var)])
                    raw_id_expr.genFlags.alwaysSafe = true
                    force_generated(raw_id_expr, true)
                    if (is_dn_entity_arg(a)) {
                        entity_expr = qmacro(dn_make_entity($i(world_var), $e(raw_id_expr)))
                    } else {
                        entity_expr = raw_id_expr
                    }
                }
                entity_expr.genFlags.alwaysSafe = true
                force_generated(entity_expr, true)
                var entity_type = clone_type(a._type)
                entity_type.flags.ref = false
                var entity_decl = new ExprLet(at = a.at, atInit = a.at)
                entity_decl.genFlags.alwaysSafe = true
                entity_decl.variables |> emplace_new() <| new Variable(at = a.at,
                    name := string(a.name),
                    _type <- entity_type,
                    init <- entity_expr,
                    flags = VariableFlags.can_shadow | VariableFlags.generated
                    )
                per_entity_body.list |> emplace(entity_decl)
                continue
            }
            macro_verify(!(a._type.flags.ref && a._type.flags.constant), compiling_program(), a.at, "ecs_query: argument {a.name} is both & and constant")
            let arg_name = string(a.name)
            let is_traversal = key_exists(traversal_by_arg, arg_name)
            if (is_traversal) {
                traversal_seen[arg_name] = true
            }
        // A pointer argument marks the traversal term optional, so root
        // entities (which have no ancestor to read from) still match and the
        // body sees null for them. It is only meaningful on a traversal arg.
            let is_optional = a._type.baseType == Type.tPointer
            if (is_optional) {
                macro_verify(is_traversal, compiling_program(), a.at, "ecs_query: pointer argument {a.name} is only allowed on a CASCADE/CASCADE_DESC/UP/SELF_UP argument, where it marks the term optional")
                macro_verify(a._type.firstType != null && a._type.firstType.baseType == Type.tStructure, compiling_program(), a.at, "ecs_query: argument {a.name} must point to a component struct")
            } else {
                macro_verify(a._type.baseType == Type.tStructure, compiling_program(), a.at, "ecs_query: argument {a.name} must be a component struct (or DnEntity / ecs_entity_t)")
            }
            let idx = comp_count
            comp_count++
            let field_var = "_dn_f{idx}{site_suffix}"
        // query term. For a pointer arg the component is the pointee.
            var term_type = clone_type(is_optional ? a._type.firstType : a._type)
            term_type.flags.constant = false
            term_type.flags.ref = false
            unsafe {
                let inout_expr = a._type.flags.constant ? qmacro(EcsIn) : qmacro(EcsInOut)
                if (is_traversal) {
                    // flags are emitted as literal constant expressions rather
                    // than a computed value: they are uint64 bit flags in bits
                    // 59..63, and any narrowing would silently zero them and
                    // degrade the term into a plain one -- wrong results, no error.
                    let oper_expr = is_optional ? qmacro(EcsOptional) : qmacro(EcsAnd)
                    let traversal_kind = traversal_by_arg[arg_name]
                    var flags_expr : ExpressionPtr
                    if (traversal_kind == "CASCADE") {
                        flags_expr = qmacro(EcsCascade)
                    } elif (traversal_kind == "CASCADE_DESC") {
                        flags_expr = qmacro(EcsCascade | EcsDesc)
                    } elif (traversal_kind == "SELF_UP") {
                        flags_expr = qmacro(EcsSelf | EcsUp)
                    } else {
                        flags_expr = qmacro(EcsUp)
                    }
                    builder_chain = qmacro($e(builder_chain).with_term_up(type<$t(term_type)>, $e(flags_expr), $e(inout_expr), $e(oper_expr), 0ul))
                } elif (a._type.flags.constant) {
                    builder_chain = qmacro($e(builder_chain).with_term(type<$t(term_type)>, EcsIn))
                } else {
                    builder_chain = qmacro($e(builder_chain).with_term(type<$t(term_type)>, EcsInOut))
                }
            }
        // per-entity binding
            var binding_type : TypeDeclPtr
            var binding_init : ExpressionPtr
            if (is_optional) {
            // optional traversal: bind the raw T? so the body can null-check it.
            // ecs_field_get yields null when the field is unset, which is
            // exactly the root-entity case.
                binding_type = clone_type(a._type)
                binding_type.flags.ref = false
                unsafe {
                    binding_init = qmacro($i(field_var))
                }
            } else {
            // let name : T& = _dn_fK[...]. A traversed field is SHARED across
            // the whole result -- ecs_field returns a pointer to one value, not
            // a per-entity array -- so it must be read at [0]. Indexing it by
            // _dn_i would read past the end for every entity after the first.
                var element_expr : ExpressionPtr
                unsafe {
                    element_expr = is_traversal ? qmacro($i(field_var)[0]) : qmacro($i(field_var)[$i(index_var)])
                }
                element_expr.genFlags.alwaysSafe = true
                force_generated(element_expr, true)
                binding_type = clone_type(is_traversal ? term_type : a._type)
                binding_type.flags.ref = true
                binding_init = element_expr
            }
            var binding_decl = new ExprLet(at = a.at, atInit = a.at)
            binding_decl.genFlags.alwaysSafe = true
            binding_decl.variables |> emplace_new() <| new Variable(at = a.at,
                name := string(a.name),
                _type <- binding_type,
                init <- binding_init,
                flags = VariableFlags.can_shadow | VariableFlags.generated
                )
            per_entity_body.list |> emplace(binding_decl)
        // per-batch field getter: var _dn_fK : T? = ecs_field_get(_dn_it, type<T>, K)
            var field_type = clone_type(is_optional ? a._type.firstType : a._type)
            field_type.flags.constant = false
            field_type.flags.ref = false
            var field_get_expr = qmacro(ecs_field_get($i(it_var), type<$t(field_type)>, $v(idx)))
            force_generated(field_get_expr, true)
            var field_ptr_type = new TypeDecl(at = a.at, baseType = Type.tPointer)
            field_ptr_type.firstType = clone_type(field_type)
            var field_decl = new ExprLet(at = a.at, atInit = a.at)
            field_decl.genFlags.alwaysSafe = true
            field_decl.variables |> emplace_new() <| new Variable(at = a.at,
                name := field_var,
                _type <- field_ptr_type,
                init <- field_get_expr,
                flags = VariableFlags.can_shadow | VariableFlags.generated
                )
            per_batch_body.list |> emplace(field_decl)
        }
        macro_verify(comp_count > 0, compiling_program(), expr.at, "ecs_query: block must declare at least one component argument")
    // a traversal annotation naming an argument that does not exist is almost
    // always a typo, and would otherwise be silently ignored
        for (arg_name in keys(traversal_by_arg)) {
            macro_verify(key_exists(traversal_seen, arg_name), compiling_program(), expr.at, "ecs_query: {traversal_by_arg[arg_name]} names unknown block argument '{arg_name}'")
        }

    // filter terms from annotations, then finish the builder chain
        unsafe {
            var require_names : array<string>
            gather_annotation_names(query_block, "REQUIRE", require_names)
            for (tag_name in require_names) {
                var tag_type = new TypeDecl(at = expr.at, baseType = Type.alias, alias := tag_name)
                builder_chain = qmacro($e(builder_chain).with_term(type<$t(tag_type)>, EcsInOutNone))
            }
            var require_not_names : array<string>
            gather_annotation_names(query_block, "REQUIRE_NOT", require_not_names)
            for (tag_name in require_not_names) {
                var tag_type = new TypeDecl(at = expr.at, baseType = Type.alias, alias := tag_name)
                builder_chain = qmacro($e(builder_chain).without_term(type<$t(tag_type)>))
            }
            if (block_has_annotation(query_block, "CACHED")) {
                builder_chain = qmacro($e(builder_chain).cached())
            }
            builder_chain = qmacro($e(builder_chain).build())
        }

    // cloned user statements run after the ref bindings
        for (stmt in query_block.list) {
            per_entity_body.list |> push(clone_expression(stmt))
        }
        for (final_stmt in query_block.finalList) {
            per_entity_body.finalList |> push(clone_expression(final_stmt))
        }
        // find mode: `return cond` -> stop-with-true / continue; plain mode: `return` -> next entity
        convert_block_to_loop(per_entity_body, false, true, find_mode)
        var entity_loop = new ExprFor(at = query_block.at, visibility = query_block.at)
        entity_loop.allowIteratorOptimization = true
        entity_loop.canShadow = true
        entity_loop.iterators |> resize(1)
        entity_loop.iterators[0] := index_var
        entity_loop.iteratorsAka |> resize(1)
        entity_loop.iteratorsAka[0] := ""
        entity_loop.iteratorsAt |> push(query_block.at)
        entity_loop.iteratorsTags |> resize(1)
        entity_loop.sources |> emplace_new() <| qmacro(range($i(it_var).count))
        entity_loop.body = per_entity_body
        per_batch_body.list |> emplace(entity_loop)

    // outer skeleton
        var skeleton : ExpressionPtr
        unsafe {
            if (find_mode) {
                skeleton = quote() {
                    invoke() <| $() : bool {
                        var _dn_wp = DnWorld(tag_world).ecs_world()
                        var _dn_q = _dn_ecs_query_get(_dn_wp, tag_key) <| $() : DnQuery {
                            return tag_builder
                        }
                        var _dn_it = ecs_query_iter(_dn_wp, _dn_q.ecs_query())
                        while (ecs_query_next_iter(_dn_it)) {
                            tag_whilebody
                        }
                        return false
                    }
                }
            } else {
                skeleton = quote() {
                    var _dn_wp = DnWorld(tag_world).ecs_world()
                    var _dn_q = _dn_ecs_query_get(_dn_wp, tag_key) <| $() : DnQuery {
                        return tag_builder
                    }
                    var _dn_it = ecs_query_iter(_dn_wp, _dn_q.ecs_query())
                    while (ecs_query_next_iter(_dn_it)) {
                        tag_whilebody
                    }
                }
            }
        }
        skeleton |> force_at(expr.at)
        apply_template(skeleton) $(rules) {
            rules |> replaceVariable("tag_world") <| clone_expression(expr.arguments[0])
            rules |> replaceVariable("tag_key") <| new ExprConstString(at = expr.at, value := site_key)
            rules |> replaceVariable("tag_builder") <| builder_chain
            rules |> replaceVariable("tag_whilebody") <| per_batch_body
            rules |> renameVariable("_dn_q", "_dn_q{site_suffix}")
            rules |> renameVariable("_dn_wp", world_var)
            rules |> renameVariable("_dn_it", it_var)
        }
        var expansion = move_unquote_block(skeleton)
        expansion |> force_at(expr.at)
        expansion.genFlags.alwaysSafe = true
        if (find_mode) {
            // single invoke expression yielding bool
            var rexpansion = expansion.list[0]
            rexpansion.genFlags.alwaysSafe = true
            return rexpansion
        }
        return expansion
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
        var query_expansion = implement(expr, 3, 1, false)
        if (query_expansion == null) {
            return null
        }
        let sys_name = "{(expr.arguments[1] as ExprConstString).value}"
        var skeleton : ExpressionPtr
        unsafe {
            skeleton = quote() {
                dn_ecs_register_system(DnWorld(tag_world).ecs_world(), tag_name, tag_phase) <| @(dt : double) : void {
                    tag_sysbody
                }
            }
        }
        skeleton |> force_at(expr.at)
        apply_template(skeleton) $(rules) {
            rules |> replaceVariable("tag_world") <| clone_expression(expr.arguments[0])
            rules |> replaceVariable("tag_name") <| new ExprConstString(at = expr.at, value := sys_name)
            rules |> replaceVariable("tag_phase") <| clone_expression(expr.arguments[2])
            rules |> replaceVariable("tag_sysbody") <| query_expansion
        }
        var expansion = move_unquote_block(skeleton)
        expansion |> force_at(expr.at)
        expansion.genFlags.alwaysSafe = true
        return expansion
    }
}

[call_macro(name="ecs_observer")]
class DnEcsObserverMacro : AstCallMacro {
    //! Component lifecycle observer: `ecs_observer(<EcsHookEvent>) $(ent : DnEntity; cmp : T) { }`.
    //! The entity argument may also be declared `ecs_entity_t` to get the raw id.
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
        let make_block = expr.arguments[1] as ExprMakeBlock
        let query_block = make_block._block as ExprBlock

        var entity_name : string = ""
        var comp_count = 0
        var component_type : TypeDeclPtr
        for (a in query_block.arguments) {
            macro_verify(a._type != null && !a._type.isAuto, compiling_program(), a.at, "ecs_observer: argument {a.name} must have an explicit type")
            macro_verify(a._type.baseType != Type.tPointer, compiling_program(), a.at, "ecs_observer: pointer argument {a.name} is not supported; traversal (CASCADE/UP) applies to queries, not hook observers")
            macro_verify(a._type.flags.constant, compiling_program(), a.at, "ecs_observer: argument {a.name} can't be var (hooks receive a value copy)")
            if (is_entity_arg(a)) {
                macro_verify(empty(entity_name), compiling_program(), a.at, "ecs_observer: only one entity argument (DnEntity or ecs_entity_t) is allowed")
                entity_name = string(a.name)
            } else {
                comp_count++
                component_type = clone_type(a._type)
                component_type.flags.constant = false
                component_type.flags.ref = false
            }
        }
        macro_verify(comp_count == 1, compiling_program(), expr.at, "ecs_observer: block must declare exactly one component argument")

    // hook callback body: bind user arg names, then the cloned user statements
        var hook_body = new ExprBlock(at = query_block.at)
        for (a in query_block.arguments) {
            var arg_type = clone_type(a._type)
            arg_type.flags.ref = false
            var src : ExpressionPtr
            unsafe {
                if (is_dn_entity_arg(a)) {
                    src = qmacro(dn_make_entity(_dn_hw, _dn_he))
                } elif (is_raw_entity_arg(a)) {
                    src = qmacro(_dn_he)
                } else {
                    src = qmacro(_dn_hc)
                }
            }
            force_generated(src, true)
            var arg_decl = new ExprLet(at = a.at, atInit = a.at)
            arg_decl.genFlags.alwaysSafe = true
            arg_decl.variables |> emplace_new() <| new Variable(at = a.at,
                name := string(a.name),
                _type <- arg_type,
                init <- src,
                flags = VariableFlags.can_shadow | VariableFlags.generated
                )
            hook_body.list |> emplace(arg_decl)
        }
        for (stmt in query_block.list) {
            hook_body.list |> push(clone_expression(stmt))
        }
        for (final_stmt in query_block.finalList) {
            hook_body.finalList |> push(clone_expression(final_stmt))
        }
        convert_block_to_loop(hook_body, true, false, false)

        var skeleton : ExpressionPtr
        unsafe {
            skeleton = quote() {
                ecs_add_hook(type<tag_cmp_type>, tag_event) <| @(var _dn_hw : ecs_world_t?; var _dn_he : ecs_entity_t; _dn_hc : tag_cmp_type_c) {
                    tag_cbody
                }
            }
        }
        skeleton |> force_at(expr.at)
        // the rule replaces the whole TypeDecl including flags, so the lambda's
        // const cmp param needs its own const-flagged copy of the type
        var component_type_c = clone_type(component_type)
        component_type_c.flags.constant = true
        apply_template(skeleton) $(rules) {
            rules |> replaceTypeWithTypeDecl("tag_cmp_type") <| component_type
            rules |> replaceTypeWithTypeDecl("tag_cmp_type_c") <| component_type_c
            rules |> replaceVariable("tag_event") <| clone_expression(expr.arguments[0])
            rules |> replaceVariable("tag_cbody") <| hook_body
        }
        var expansion = move_unquote_block(skeleton)
        expansion |> force_at(expr.at)
        expansion.genFlags.alwaysSafe = true
        return expansion
    }
}

/*
def register_components(var world: DnWorld) : void {}
*/

// Macro to auto-register ECS Components
[structure_macro(name="ecs_comp")]
class SerializableMacro : AstStructureAnnotation {
}
