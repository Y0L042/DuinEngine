#include "dnpch.h"
#include "Pipeline.h"

#include "DECS/World.h"

namespace duin
{

EcsPipeline::EcsPipeline(World world) : world(world)
{
}

EcsPipeline &EcsPipeline::AddStage(ExecutionStep step, const std::string &name)
{
    // TODO: implement. Skeleton only.
    std::vector<Stage> &stepStages = stages[static_cast<size_t>(step)];
    stepStages.push_back(Stage{name, step, {}});
    return *this;
}

EcsPipeline &EcsPipeline::AddStages(ExecutionStep step, const std::vector<std::string> &stages)
{
    for (const std::string &stage : stages)
    {
        AddStage(step, stage);
    }

    return *this;
}

EcsPipeline &EcsPipeline::AddSystem(ExecutionStep step, const std::string &stageName, const SystemHandle &system)
{
    // TODO very slow implementation
    std::optional<Stage> stage = GetStage(step, stageName);
    if (stage.has_value())
    {
        stage.value().systems.push_back(system);
        SetStage(step, stage.value());
    }

    return *this;
}

void EcsPipeline::RunAll(float deltaTime)
{
    // TODO: implement. Skeleton only.
    for (size_t i = 0; i < static_cast<size_t>(ExecutionStep::COUNT); ++i)
    {
        RunStep(static_cast<ExecutionStep>(i), deltaTime);
    }
}

void EcsPipeline::RunStep(ExecutionStep step, float deltaTime)
{
    // TODO: implement. Skeleton only.
    for (const Stage &stage : stages[static_cast<size_t>(step)])
    {
        RunStage(stage, deltaTime);
    }
}

void EcsPipeline::RunStage(const Stage &stage, float deltaTime)
{
    // TODO: implement. Skeleton only.
    for (const SystemHandle &system : stage.systems)
    {
        RunSystem(system, deltaTime);
    }
}

void EcsPipeline::RunSystem(const SystemHandle &systemHandle, float deltaTime)
{
    // world is a non-owning view; an empty handle has a null underlying world.
    if (!systemHandle.world.GetFlecsWorld().c_ptr())
        return;

    // Adopt the stored system entity as a runnable System and run it once.
    systemHandle.world.System(systemHandle.system).Run(deltaTime);
}

const std::vector<Stage> &EcsPipeline::GetStages(ExecutionStep step) const
{
    return stages[static_cast<size_t>(step)];
}

std::optional<Stage> EcsPipeline::GetStage(ExecutionStep step, const std::string &name) const
{
    // Linear search: few stages per step, infrequent lookups (see header).
    for (const Stage &stage : stages[static_cast<size_t>(step)])
    {
        if (stage.name == name)
            return stage;
    }
    return std::nullopt;
}

bool EcsPipeline::SetStage(ExecutionStep step, const Stage &stage)
{
    // Linear search for the stage with a matching name, then overwrite it.
    for (Stage &existing : stages[static_cast<size_t>(step)])
    {
        if (existing.name == stage.name)
        {
            existing = stage;
            return true;
        }
    }
    return false;
}

} // namespace duin
