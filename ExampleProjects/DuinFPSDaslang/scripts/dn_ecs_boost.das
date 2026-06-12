options gen2
options indenting = 4
options no_unused_block_arguments = false
options no_unused_function_arguments = false
options strict_smart_pointers = true

module dn_ecs_boost shared private

require daslib/flat_hash_table
require daslib/ast_boost
require daslib/templates_boost
require daslib/strings_boost
require daslib/ast_block_to_loop
require daslib/defer
require daslib/macro_boost

require dn_ecs

/*

    var query_integrate_velocity : DnQuery = DnQuery()

    query_integrate_velocity = (world.query_builder()
        .with_term(type<Transform3D>)
        .with_term(type<Velocity3D>)
        .cached()
        .build())

    def system_integrate_velocity(world : ecs_world_t?; delta : double) : void {
        var it = ecs_query_iter(world, query_integrate_velocity.ecs_query())
        unsafe {
            while (ecs_query_next_iter(it)) {
                var tx = ecs_field_get(it, type<Transform3D>, 0)
                var v  = ecs_field_get(it, type<Velocity3D>,  1)
                for (i in range(it.count)) {
                    tx[i].pos += v[i].val * float(delta)
                }
            }
        }
    }

    query_integrate_velocity.fini()

    // Called
    system_integrate_velocity(world, delta)


    Target syntax:

    def system_integrate_velocity(world : ecs_world_t?, delta : double) : void {
        var query(<name : string>, <eid : ecs_entity_t>) $ [CACHED] (tx : Transform3D, vel : Velocity3D) {

        }
    }

    2 Parts:
        1. Build and cache query
            - Query gets stored in `cached_queries`
        2. Execute query

*/

var cached_queries : table<string, DnQuery>

[call_macro(name="ecs_query")]
class DecsQueryMacro : AstCallMacro {
    def override preVisit( prog : ProgramPtr; 
                        mod : Module?; 
                        var expr : ExprCallMacro?
                       ) : void {
        let totalArgs = length(expr.arguments)
        if (totalArgs < 1 || totalArgs > 2) {
            return
        }

        let block_arg_idx = totalArgs - 1 // i.e. last argument
        if (!(expr.arguments[block_arg_idx] is ExprMakeBlock)) {
            return
        }

        var mblk = expr.arguments[block_arg_idx] as ExprMakeBlock
        var qblk = mblk._block as ExprBlock
        for (arg in qblk.arguments) {
            if (arg._type != null) {
                // If arg not constant, make it passed by ref
                if (!arg._type.flags.constant) {
                    arg._type.flags.ref = true
                }
            }
        }
    }

    def override visit( prog : ProgramPtr; 
                        mod : Module?; 
                        var expr : ExprCallMacro?
                       ) : ExpressionPtr {
        let totalArgs = length(expr.arguments)
        macro_verify(
            totalArgs >= 1 && totalArgs <= 2,
            prog,
            expr.at,
            "expecting query($(block_with_arguments)) or query(eid,$(block_with_arguments))"
        )
        let block_arg_idx = totalArgs - 1 // i.e. last argument
        return <- implement(expr, block_arg_idx)
    }

    // Does actual codegen
    def implement(var expr : ExprCallMacro?, block_arg_idx : int) : ExpressionPtr {
        for (arg in expr.arguments) {
            // No auto types
            macro_verify(!arg._type.isAutoOrAlias, compiling_program(), expr.at, "argument types are not fully inferred")
        }
        macro_verify(expr.arguments[block_arg_idx] is ExprMakeBlock, compiling_program(), expr.at, "expecting $(block_with_arguments)")

    // Extract Block
        let mblk = expr.arguments[block_arg_idx] as ExprMakeBlock
        let qblk = mblk._block as ExprBlock
        macro_verify(!empty(qblk.arguments), compiling_program(), expr.at, "ecs_query: block must declare at least one component argument")

    // Determine query cache key
        var query_key : string
        if (block_arg_idx == 1) {
            // Key passed
            macro_verify(expr.arguments[0] is ExprConstString, compiling_program(), expr.at, "ecs_query: first argument must be a string literal")
            query_key := (expr.arguments[0] as ExprConstString).value
        } else {
            query_key = "__ecs_query_{expr.at.line}_{expr.at.column}"
        }

    // Build query-builder chain iteratively
        var builder_chain : ExpressionPtr
        unsafe {
            builder_chain = qmacro(world.query_builder())
            // appends with_term(type) at the end
            for (arg in qblk.arguments) {
                builder_chain = qmacro($v(builder_chain).with_term(type< $t(arg._type) >))
            }

            // TODO add if for "if query must be cached"
            builder_chain = qmacro($v(builder_chain).cache())

            builder_chain = qmacro($v(builder_chain).build())
        }

        return builder_chain
    }
}

