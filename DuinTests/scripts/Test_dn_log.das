options gen2
require dastest/testing_boost public

require dn_log

[test]
def test_info_does_not_crash() {
    dn_info("test: info message")
}

[test]
def test_warn_does_not_crash() {
    dn_warn("test: warn message")
}

[test]
def test_error_does_not_crash() {
    dn_error("test: error message")
}

[test]
def test_trace_does_not_crash() {
    dn_trace("test: trace message")
}

[test]
def test_fatal_does_not_crash() {
    dn_fatal("test: fatal message")
}
