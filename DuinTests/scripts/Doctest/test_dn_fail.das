options gen2
require dn_doctest

// dn_fail always triggers a REQUIRE-level failure.
// Export a guard function that does NOT call dn_fail.

[export]
def test_dn_fail_available() {
    dn_message("dn_fail binding is present", __LINE__)
}
