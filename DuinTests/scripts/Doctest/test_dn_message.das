options gen2
require dn_doctest

[export]
def test_dn_message() {
    dn_message("hello from daslang", __LINE__)
    dn_message("second message", __LINE__)
}
