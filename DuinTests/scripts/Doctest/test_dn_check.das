options gen2
require dn_doctest

[export]
def test_dn_check() {
    dn_check(true, __LINE__)
    dn_check(1 + 1 == 2, __LINE__)
}
