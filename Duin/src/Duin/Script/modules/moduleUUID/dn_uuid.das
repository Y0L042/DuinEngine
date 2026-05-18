options gen2

module dn_uuid shared public
require dn_uuid_core public

typedef DnUUID = uint64

def dn_uuid_to_string(uuid : DnUUID) : string {
    return dn_uuid_to_string_hex(uuid)
}
