options gen2
require dastest/testing_boost public

require dn_log
require dn_window

[test]
def test_flag_constants_match_sdl() {
    verify(DN_WINDOW_FULLSCREEN == 0x1ul, "DN_WINDOW_FULLSCREEN matches SDL_WINDOW_FULLSCREEN")
    verify(DN_WINDOW_HIDDEN == 0x8ul, "DN_WINDOW_HIDDEN matches SDL_WINDOW_HIDDEN")
    verify(DN_WINDOW_BORDERLESS == 0x10ul, "DN_WINDOW_BORDERLESS matches SDL_WINDOW_BORDERLESS")
    verify(DN_WINDOW_RESIZABLE == 0x20ul, "DN_WINDOW_RESIZABLE matches SDL_WINDOW_RESIZABLE")
    verify(DN_WINDOW_MINIMIZED == 0x40ul, "DN_WINDOW_MINIMIZED matches SDL_WINDOW_MINIMIZED")
    verify(DN_WINDOW_MAXIMIZED == 0x80ul, "DN_WINDOW_MAXIMIZED matches SDL_WINDOW_MAXIMIZED")
    verify(DN_WINDOW_ALWAYS_ON_TOP == 0x10000ul, "DN_WINDOW_ALWAYS_ON_TOP matches SDL_WINDOW_ALWAYS_ON_TOP")
}

[test]
def test_default_window_is_invalid() {
    var win = new DnWindow()
    verify(!win.is_valid(), "default-constructed window is invalid")
    verify(!win.can_draw(), "invalid window cannot draw")
    verify(!win.is_minimized(), "invalid window is not minimized")
    unsafe {
        delete win
    }
}

[test]
def test_invalid_window_accessors_are_null_safe() {
    var win = new DnWindow()
    let sz = win.size()
    verify(sz.x == 0 && sz.y == 0, "invalid window size is zero")
    verify(win.title() == "", "invalid window title is empty")
    verify(win.flags() == 0ul, "invalid window flags are zero")
    verify(win.clear_color() == 0u, "invalid window clear color is zero")
    verify(!win.close(), "closing an invalid window fails")
    win.set_size(100, 100)
    win.set_title("ignored")
    win.set_flags(DN_WINDOW_RESIZABLE)
    win.set_clear_color(0xff0000ffu)
    win.set_camera(null)
    unsafe {
        delete win
    }
}

[test]
def test_queue_draw_on_invalid_window_is_noop() {
    var win = new DnWindow()
    var ran = false
    win.queue_draw() <| @capture(& ran) () {
        ran = true
    }
    win.queue_draw_ui() <| @capture(& ran) () {
        ran = true
    }
    verify(!ran, "queued lambdas never run on an invalid window")
    unsafe {
        delete win
    }
}

[test]
def test_secondary_window_count_nonnegative() {
    verify(dn_window_count() >= 0, "secondary window count is non-negative")
}
