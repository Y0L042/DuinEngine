options gen2
require dn_doctest

[export]
def test_dn_require() {
    dn_require(true, __LINE__)
    dn_require(100 == 100, __LINE__)
}
