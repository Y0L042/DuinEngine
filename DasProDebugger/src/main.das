options gen2

require live_host
require probe_debug

[export]
def init() {
    to_log(LOG_INFO, "DasProDebugger Stage 1 ready\n")
    to_log(LOG_INFO, "  DAP debug server: tcp://localhost:10000\n")
    to_log(LOG_INFO, "  Connect your IDE to this port\n")
}

[export]
def update() {
    pass
}

[export]
def shutdown() {
    pass
}
