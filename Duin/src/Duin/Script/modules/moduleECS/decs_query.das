options gen2

require decs_entity public
require decs_world public
require decs_components public
require decs_query

/*
    ecs_query_t *q = 
    ecs_query
    (
        world, 
        {
            .terms = 
            {
                { 
                    ecs_id(Position) 
                }, 
                {  
                    ecs_id(Velocity) 
                },
            }
        }
    );

    CreateQuery
    (
        World,
        Desc
        {
            Terms
            {
                [
                    Term1
                    {
                        ID
                    },
                    Term2
                    {
                        ID
                    }...
                ]
            }
        }
    )    
*/

/*
struct dn_Term {
    id : uint64
}
*/

typedef dn_Term = uint64

struct dn_QueryDesc {
    terms : array<dn_Term>
}

class dn_Query {
    handle_ : void?
    world_handle_ : void?

    def begin_iter() : void? {
        return decs_query_iter_begin_impl(world_handle_, handle_)
    }

    def iter_next(var it : void?) : bool {
        return decs_query_iter_next_impl(it)
    }

    def iter_fini(var it : void?) {
        decs_query_iter_fini_impl(it)
    }

    def iter_count(it : void?) : int {
        return decs_iter_count_impl(it)
    }

    def iter_entity(it : void?; index : int) : uint64 {
        return decs_iter_entity_impl(it, index)
    }
}

def dn_query_each(var q : dn_Query?, blk : block<(entity : dn_Entity?)>) {
    if (q != null) {
          decs_query_each_impl(q.world_handle_, q.handle_) <| $(eid : uint64) {
              var e : dn_Entity? = new dn_Entity()
              e._id = eid
              e._world_handle = q.world_handle_
              invoke(blk, e)
          }
    }
}

def dn_query_builder(var world : dn_World?; var query_desc : dn_QueryDesc) : dn_Query? {
    if (world == null) {
        return null
    }

    var q = new dn_Query()
    q.world_handle_ = world._handle
    unsafe {
        var term_ids : array<uint64>
        for (t in query_desc.terms) {
            push(term_ids, t.id)
        }
        q.handle_ = decs_query_impl(world._handle, addr(term_ids[0]), length(term_ids))
    }
    return q
}