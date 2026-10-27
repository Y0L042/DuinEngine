options gen2

require imgui_boost
require dn_log
require dn_application
require dn_window

var windows : array<DnWindow?>
var opened_count = 0

def open_window() {
    opened_count++
    let title = "Window #{opened_count}"
    var win = new DnWindow(title, 480, 320)
    if (!win.is_valid()) {
        dn_warn("Could not open {title}")
        unsafe {
            delete win
        }
        return
    }
    windows |> push(win)
}

def close_window(index : int) {
    var win = windows[index]
    win.close()
    windows |> erase(index)
    unsafe {
        delete win
    }
}

def prune_closed_windows() {
    for (i in range(length(windows) - 1, -1, -1)) {
        if (!windows[i].is_valid()) {
            var win = windows[i]
            windows |> erase(i)
            unsafe {
                delete win
            }
        }
    }
}

[export]
def game_ready() {
    open_window()
    open_window()
}

[export]
def game_update(delta : double) {
    prune_closed_windows()
}

[export]
def game_draw_ui() {
    imgui_boost::Begin("Windows")
    imgui_boost::Text("Open secondary windows: {dn_window_count()}")
    if (imgui_boost::Button("Open window")) {
        open_window()
    }
    var to_close = -1
    for (i, win in count(), windows) {
        let title = win.title()
        if (imgui_boost::Button("Close {title}")) {
            to_close = i
        }
        if (win.can_draw()) {
            win.queue_draw_ui() <| @capture(= title) () {
                imgui_boost::Begin("Hello")
                imgui_boost::Text("This is {title}")
                imgui_boost::End()
            }
        }
    }
    if (to_close >= 0) {
        close_window(to_close)
    }
    imgui_boost::End()
}

[export]
def main() {
}
