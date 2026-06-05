// WS smoke-test client for the DuinDasHost --serve-ws daemon (M3 step 1 acceptance).
//
// Connects to a running daemon's WebSocket endpoint, sends one compile request frame,
// prints the result frame, then exits. Doubles as a first exercise of the dasHV
// HvWebSocketClient API that the real LSP proxy (proxy.das) will use.
//
// Run (after starting `DuinDasHost --serve-ws --project <P>` and reading wsUrl from .ddh):
//   daslang.exe ws_smoke_client.das <wsUrl> <request-json>
// e.g.
//   daslang.exe ws_smoke_client.das ws://127.0.0.1:51234 '{"file":"C:/.../smoke_test.das","project":"C:/.../duindashost.das_project"}'

options gen2
options persistent_heap
options gc

require dashv/dashv_boost public
require daslib/fio
require strings

class SmokeClient : HvWebSocketClient {
    request : string
    reply : string
    got_reply : bool = false

    def override onOpen {
        print("[smoke] connected; sending request\n")
        self->send(request)
    }

    def override onClose {
        print("[smoke] disconnected\n")
    }

    def override onMessage(msg : string#) {
        reply = string(msg)
        got_reply = true
        print("[smoke] reply: {reply}\n")
    }
}

[export]
def main {
    // args: [0]=daslang.exe [1]=script [2]="--" [3]=wsUrl [4]=request-json
    let args <- get_command_line_arguments()
    if (length(args) < 5) {
        print("usage: daslang.exe ws_smoke_client.das -- <wsUrl> <request-json>\n")
        return
    }
    let url = args[3]
    let req = args[4]

    var client = new SmokeClient()
    client.request := req

    print("[smoke] connecting to {url}\n")
    let rc = client->init(url)
    if (rc != 0) {
        print("[smoke] init failed rc={rc}\n")
        unsafe { delete client; }
        return
    }

    // Pump the event queue until the reply arrives or we time out (~5s).
    var elapsed = 0
    while (!client.got_reply && elapsed < 5000) {
        client->process_event_que()
        sleep(20u)
        elapsed += 20
    }

    if (!client.got_reply) {
        print("[smoke] TIMEOUT — no reply in {elapsed}ms\n")
    } else {
        print("[smoke] OK\n")
    }

    client->close()
    unsafe { delete client; }
}
