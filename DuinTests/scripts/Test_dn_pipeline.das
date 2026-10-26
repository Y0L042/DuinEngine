options gen2
require dastest/testing_boost public

require dn_log
require dn_ecs
require dn_pipeline

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

def make_world() : DnWorld {
    var w : DnWorld
    w.initialize()
    return w
}

def make_pipeline(var w : DnWorld) : DnPipeline? {
    return new DnPipeline(w.ecs_world())
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

[test]
def test_pipeline_is_valid_after_create() {
    var w = make_world()
    var p = make_pipeline(w)
    verify(p.is_valid(), "DnPipeline is valid after creation")
    p.fini()
}

[test]
def test_pipeline_invalid_after_fini() {
    var w = make_world()
    var p = make_pipeline(w)
    p.fini()
    verify(!p.is_valid(), "DnPipeline is invalid after fini")
}

// ---------------------------------------------------------------------------
// Stages
// ---------------------------------------------------------------------------

[test]
def test_add_stage_registers_stage() {
    var w = make_world()
    var p = make_pipeline(w)
    p.add_stage(DnExecutionStep.UPDATE, "Movement")
    verify(p.has_stage(DnExecutionStep.UPDATE, "Movement"), "stage 'Movement' registered under UPDATE")
    p.fini()
}

[test]
def test_has_stage_false_for_missing() {
    var w = make_world()
    var p = make_pipeline(w)
    p.add_stage(DnExecutionStep.UPDATE, "Movement")
    verify(!p.has_stage(DnExecutionStep.UPDATE, "Missing"), "unregistered stage reports false")
    p.fini()
}

[test]
def test_stages_separate_per_step() {
    var w = make_world()
    var p = make_pipeline(w)
    p.add_stage(DnExecutionStep.UPDATE, "UpdateStage")
    p.add_stage(DnExecutionStep.DRAW, "DrawStage")
    verify(p.has_stage(DnExecutionStep.UPDATE, "UpdateStage"), "UpdateStage under UPDATE")
    verify(p.has_stage(DnExecutionStep.DRAW, "DrawStage"), "DrawStage under DRAW")
    verify(!p.has_stage(DnExecutionStep.DRAW, "UpdateStage"), "UpdateStage not under DRAW")
    p.fini()
}

[test]
def test_add_stages_bulk() {
    var w = make_world()
    var p = make_pipeline(w)
    p.add_stages(DnExecutionStep.UPDATE, ["Input", "Move", "Resolve"])
    verify(p.has_stage(DnExecutionStep.UPDATE, "Input"), "Input registered")
    verify(p.has_stage(DnExecutionStep.UPDATE, "Move"), "Move registered")
    verify(p.has_stage(DnExecutionStep.UPDATE, "Resolve"), "Resolve registered")
    p.fini()
}

// ---------------------------------------------------------------------------
// Systems
// ---------------------------------------------------------------------------

[test]
def test_add_system_does_not_crash() {
    var w = make_world()
    var p = make_pipeline(w)
    p.add_stage(DnExecutionStep.UPDATE, "Movement")
    var e = w.create_entity("MoveSystem")
    p.add_system(DnExecutionStep.UPDATE, "Movement", e)
    verify(p.has_stage(DnExecutionStep.UPDATE, "Movement"), "stage still present after add_system")
    p.fini()
}

// ---------------------------------------------------------------------------
// Run
// ---------------------------------------------------------------------------

[test]
def test_run_step_does_not_crash() {
    var w = make_world()
    var p = make_pipeline(w)
    p.add_stage(DnExecutionStep.UPDATE, "Movement")
    p.run_step(DnExecutionStep.UPDATE, 0.016f)
    verify(p.has_stage(DnExecutionStep.UPDATE, "Movement"), "stage intact after run_step")
    p.fini()
}

[test]
def test_run_all_does_not_crash() {
    var w = make_world()
    var p = make_pipeline(w)
    p.add_stage(DnExecutionStep.INITIALIZE, "Init")
    p.add_stage(DnExecutionStep.UPDATE, "Update")
    p.add_stage(DnExecutionStep.DRAW, "Draw")
    p.run_all(0.016f)
    verify(p.has_stage(DnExecutionStep.INITIALIZE, "Init"), "Init intact after run_all")
    verify(p.has_stage(DnExecutionStep.UPDATE, "Update"), "Update intact after run_all")
    verify(p.has_stage(DnExecutionStep.DRAW, "Draw"), "Draw intact after run_all")
    p.fini()
}

// ---------------------------------------------------------------------------
// Construction and teardown edge cases
// ---------------------------------------------------------------------------

[test]
def test_pipeline_from_dn_world() {
    var w = make_world()
    var p = new DnPipeline(w)
    verify(p.is_valid(), "DnPipeline(DnWorld) is valid")
    p.add_stage(DnExecutionStep.UPDATE, "FromWorld")
    verify(p.has_stage(DnExecutionStep.UPDATE, "FromWorld"), "stage registered on DnWorld-built pipeline")
    p.fini()
}

[test]
def test_pipeline_fini_twice_is_safe() {
    var w = make_world()
    var p = make_pipeline(w)
    p.fini()
    p.fini()
    verify(!p.is_valid(), "pipeline stays invalid after a second fini")
}

[test]
def test_pipeline_calls_after_fini_are_noops() {
    var w = make_world()
    var p = make_pipeline(w)
    p.fini()
    p.add_stage(DnExecutionStep.UPDATE, "Late")
    p.add_stages(DnExecutionStep.DRAW, ["A", "B"])
    var e = w.create_entity("LateSystem")
    p.add_system(DnExecutionStep.UPDATE, "Late", e)
    p.run_step(DnExecutionStep.UPDATE, 0.016f)
    p.run_all(0.016f)
    verify(!p.has_stage(DnExecutionStep.UPDATE, "Late"), "no stage registered after fini")
}

[test]
def test_add_system_to_missing_stage_creates_nothing() {
    var w = make_world()
    var p = make_pipeline(w)
    var e = w.create_entity("OrphanSystem")
    p.add_system(DnExecutionStep.UPDATE, "Missing", e)
    verify(!p.has_stage(DnExecutionStep.UPDATE, "Missing"), "add_system does not create a stage")
    p.fini()
}

[test]
def test_add_system_with_invalid_entity_is_ignored() {
    var w = make_world()
    var p = make_pipeline(w)
    p.add_stage(DnExecutionStep.UPDATE, "Movement")
    var invalid : DnEntity = DnEntity(w.ecs_world(), 0ul)
    invalid.world = null
    p.add_system(DnExecutionStep.UPDATE, "Movement", invalid)
    p.run_step(DnExecutionStep.UPDATE, 0.016f)
    verify(p.has_stage(DnExecutionStep.UPDATE, "Movement"), "stage intact after ignored add_system")
    p.fini()
}

[test]
def test_execution_step_values_match_cpp_enum() {
    verify(int(DnExecutionStep.INITIALIZE) == 0, "INITIALIZE is 0")
    verify(int(DnExecutionStep.READY) == 1, "READY is 1")
    verify(int(DnExecutionStep.PHYSICSUPDATE) == 2, "PHYSICSUPDATE is 2")
    verify(int(DnExecutionStep.UPDATE) == 3, "UPDATE is 3")
    verify(int(DnExecutionStep.ONEVENT) == 4, "ONEVENT is 4")
    verify(int(DnExecutionStep.DRAW) == 5, "DRAW is 5")
    verify(int(DnExecutionStep.DRAWUI) == 6, "DRAWUI is 6")
    verify(int(DnExecutionStep.DEBUG) == 7, "DEBUG is 7")
    verify(int(DnExecutionStep.EXIT) == 8, "EXIT is 8")
}
