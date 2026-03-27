options gen2
require dn_doctest

[export]
def test_combined() {
    dn_check(true, __LINE__)
    dn_check_false(false, __LINE__)
    dn_require(1 == 1, __LINE__)
    dn_require_false(1 != 1, __LINE__)
    dn_message("combined test reached end", __LINE__)
}
