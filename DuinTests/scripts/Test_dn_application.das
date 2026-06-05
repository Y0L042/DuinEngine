options gen2
require dastest/testing_boost public

require dn_log
require dn_application

[test]
def test_get_ticks_nonnegative() {
    let ticks = dn_get_ticks()
    verify(ticks >= 0.0lf, "dn_get_ticks returns non-negative value")
}

[test]
def test_get_ticks_milli_nonnegative() {
    let ms = dn_get_ticks_milli()
    verify(ms >= 0.0lf, "dn_get_ticks_milli returns non-negative value")
}

[test]
def test_update_not_paused_by_default() {
    verify(!dn_is_update_paused(), "update is not paused by default")
}

[test]
def test_pause_resume_update() {
    dn_pause_update()
    verify(dn_is_update_paused(), "update is paused after dn_pause_update")
    dn_resume_update()
    verify(!dn_is_update_paused(), "update is running after dn_resume_update")
}

[test]
def test_physics_not_paused_by_default() {
    verify(!dn_is_physics_paused(), "physics is not paused by default")
}

[test]
def test_pause_resume_physics() {
    dn_pause_physics()
    verify(dn_is_physics_paused(), "physics is paused after dn_pause_physics")
    dn_resume_physics()
    verify(!dn_is_physics_paused(), "physics is running after dn_resume_physics")
}

[test]
def test_debug_not_paused_by_default() {
    verify(dn_debug_is_game_paused() == 0, "game is not debug-paused by default")
}

[test]
def test_debug_pause_resume() {
    dn_debug_pause_game()
    verify(dn_debug_is_game_paused() != 0, "game is debug-paused after dn_debug_pause_game")
    dn_debug_resume_game()
    verify(dn_debug_is_game_paused() == 0, "game is running after dn_debug_resume_game")
}
