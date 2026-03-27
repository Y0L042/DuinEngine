options gen2
require dn_doctest

// dn_fail_check always triggers a CHECK-level failure.
// Export a guard function that does NOT call dn_fail_check.

[export]
def test_dn_fail_check_available() {
    dn_message("dn_fail_check binding is present", __LINE__)
}
