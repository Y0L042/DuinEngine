options gen2

// Negative fixture for DuinDasHost: a deliberate type error to verify that
// structured diagnostics (file/line/column/code/message) are emitted and the
// process exits non-zero. dn_info expects a string; passing an int must fail.

require dn_log

[export]
def main() {
    dn_info(42)
}
