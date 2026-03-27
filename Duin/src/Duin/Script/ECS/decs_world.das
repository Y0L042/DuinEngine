options gen2

require dn_log

class dn_World
{
    _handle : void ?

    def is_valid() : bool
    {
        return (_handle != null)
    }
}

def decs_create_world() : dn_World?
{
    var w : dn_World? = new dn_World()
    //decs_create_world_impl(w)

    return w
}