options gen2
require dastest/testing_boost public

require dn_log
require dn_uuid

[test]
def test_generate_nonzero() {
    let uuid = dn_uuid_make()
    assert(uuid != 0ul, "dn_uuid_make returns non-zero")
}

[test]
def test_generate_unique() {
    let a = dn_uuid_make()
    let b = dn_uuid_make()
    assert(a != b, "two consecutive UUIDs are distinct")
}

[test]
def test_invalid_uuid_is_zero() {
    let inv = dn_uuid_invalid()
    assert(inv == 0ul, "invalid UUID is zero")
}

[test]
def test_is_valid_for_generated() {
    let uuid = dn_uuid_make()
    verify(dn_uuid_is_valid(uuid), "generated UUID is valid")
}

[test]
def test_is_valid_false_for_invalid() {
    let inv = dn_uuid_invalid()
    verify(!dn_uuid_is_valid(inv), "invalid UUID reports not valid")
}

[test]
def test_to_string_hex_nonempty() {
    let uuid = dn_uuid_make()
    let s = dn_uuid_to_string_hex(uuid)
    verify(!empty(s), "hex string is non-empty")
}

[test]
def test_to_string_dec_nonempty() {
    let uuid = dn_uuid_make()
    let s = dn_uuid_to_string_dec(uuid)
    verify(!empty(s), "dec string is non-empty")
}

[test]
def test_roundtrip_hex() {
    let uuid = dn_uuid_make()
    let s = dn_uuid_to_string_hex(uuid)
    let restored = dn_uuid_from_string_hex(s)
    assert(uuid == restored, "hex roundtrip preserves value")
}

[test]
def test_roundtrip_dec() {
    let uuid = dn_uuid_make()
    let s = dn_uuid_to_string_dec(uuid)
    let restored = dn_uuid_from_string_dec(s)
    assert(uuid == restored, "dec roundtrip preserves value")
}
