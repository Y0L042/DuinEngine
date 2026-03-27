options gen2
require dn_doctest

[export]
def test_dn_require_false() {
    dn_require_false(false, __LINE__)
    dn_require_false(1 != 1, __LINE__)
}
