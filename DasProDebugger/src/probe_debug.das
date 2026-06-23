options gen2

module probe_debug shared public

// Requiring debug auto-installs the full DAP agent via [_macro].
// Handles: breakpoints, step-in/out/over, stack trace, variable inspection, eval.
// TCP port 10000 by default; override with -- --das-debug-port N.
require daslib/debug public
