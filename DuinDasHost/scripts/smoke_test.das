options gen2

// Smoke test for DuinDasHost binding-aware compilation.
//
// This file requires engine (dn_*) modules and calls a binding. It compiles ONLY
// when the host has registered the C++ engine modules:
//   - Stock daslang.exe  -> FAILS (require dn_log: unknown module).
//   - DuinDasHost.exe     -> SUCCEEDS (modules registered, dn_info resolves).
//
// To verify the error path (diagnostics JSON), introduce a deliberate type error,
// e.g. change the dn_info argument to an int: `dn_info(42)`.

require dn_log

[export]
def main() {
    dn_info("DuinDasHost smoke test: dn_* bindings resolved.")
}
