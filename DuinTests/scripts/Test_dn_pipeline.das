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

def make_pipeline(var w : DnWorld) : DnPipeline {
    return DnPipeline(w.ecs_world())
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
