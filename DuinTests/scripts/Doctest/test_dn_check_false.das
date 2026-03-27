options gen2
require dn_doctest

[export]
def test_dn_check_false() {
    dn_check_false(false, __LINE__)
    dn_check_false(1 + 1 == 3, __LINE__)
}
