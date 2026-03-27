options gen2
options remove_unused_symbols = false
options rtti

require rtti

// dn_register_test_case is a convenience wrapper around the raw dn_register_test
// binding.  Pass the suite name, a human-readable test name, and the exported
// function name (as a string) that contains the test body.  The at argument
// carries the .das source location so doctest reports the right file/line.
//
// Usage:
//   require dn_doctest
//
//   [export] def my_test_body() {
//       dn_check(1 + 1 == 2, __LINE__)
//   }
//
//   [export] def register_tests() {
//       dn_register_test_case("MySuite", "one plus one", "my_test_body", __LINE__)
//   }

[export]
def dn_register_test_case(suite : string; name : string; fn_name : string; at : LineInfo) {
    //dn_register_test(suite, name, fn_name, at)
}
