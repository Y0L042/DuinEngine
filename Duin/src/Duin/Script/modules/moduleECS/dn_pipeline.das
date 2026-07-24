options gen2
options remove_unused_symbols = false
options rtti

require daslib/class_boost public

module dn_pipeline shared public

require dn_pipeline_core public
require dn_log public
require dn_ecs public

// Mirrors duin::ExecutionStep (Pipeline.h). Int values MUST match the C++ enum
// order; they are cast to int at the extern boundary.
enum DnExecutionStep {
    INITIALIZE
    READY
    PHYSICSUPDATE
    UPDATE
    ONEVENT
    DRAW
    DRAWUI
    DEBUG
    EXIT
}

class DnPipeline {
    _handle : void? = null

    def DnPipeline(world : ecs_world_t?) : void {
        _handle = dn_create_pipeline_impl(world)
    }

    def DnPipeline(dn_world : DnWorld) : void {
        _handle = dn_create_pipeline_impl(dn_world.world)
    }

    [class_method]
    def static is_valid() : bool {
        return _handle != null
    }

    [class_method]
    def static add_stage(step : DnExecutionStep; name : string) : DnPipeline {
        if (_handle != null) {
            dn_pipeline_add_stage_impl(_handle, int(step), name)
        }
        return self
    }

    [class_method]
    def static add_stages(step : DnExecutionStep; names : array<string>) : DnPipeline {
        if (_handle != null) {
            for (name in names) {
                dn_pipeline_add_stage_impl(_handle, int(step), name)
            }
        }
        return self
    }

    [class_method]
    def static add_system(step : DnExecutionStep; stage_name : string; e : DnEntity) : DnPipeline {
        if (_handle != null && e.world != null) {
            dn_pipeline_add_system_impl(_handle, int(step), stage_name, e.world, e.entity)
        }
        return self
    }

    [class_method]
    def static run_all(dt : float) : void {
        if (_handle != null) {
            dn_pipeline_run_all_impl(_handle, dt)
        }
    }

    [class_method]
    def static run_step(step : DnExecutionStep; dt : float) : void {
        if (_handle != null) {
            dn_pipeline_run_step_impl(_handle, int(step), dt)
        }
    }

    [class_method]
    def static has_stage(step : DnExecutionStep; name : string) : bool {
        if (_handle == null) {
            return false
        }
        return dn_pipeline_has_stage_impl(_handle, int(step), name)
    }

    [class_method]
    def static fini() : void {
        if (_handle != null) {
            dn_destroy_pipeline_impl(_handle)
            _handle = null
        }
    }
}
