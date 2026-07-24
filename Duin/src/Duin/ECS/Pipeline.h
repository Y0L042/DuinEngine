#pragma once

#include <array>
#include <optional>
#include <vector>
#include "DECS/System.h"
#include "Duin/Core/Signals/Signal.h"

/*
    ```C++
    Pipeline(world).AddStage(Update, Move, DependsOn(Input));
    ```

    ```daslang
    world.pipeline.add_stage()
    ```
*/

namespace duin
{

enum class ExecutionStep
{
    /*
    ENGINEINITIALIZE,
    INITIALIZE,

    ENGINEREADY,
    READY,
    ENGINEPOSTREADY,

    ENGINEPREFRAME,

    ENGINEONEVENT,
    ONEVENT,

    ENGINEUPDATE,
    UPDATE,
    ENGINEPOSTUPDATE,

    ENGINEPHYSICSUPDATE,
    PHYSICSUPDATE,
    ENGINEPOSTPHYSICSUPDATE,

    ENGINEDRAW,
    DRAW,
    ENGINEPOSTDRAW,

    ENGINEDRAWUI,
    DRAWUI,
    ENGINEPOSTDRAWUI,

    ENGINEDEBUG,
    DEBUG,
    ENGINEPOSTDEBUG,

    ENGINEPOSTFRAME,

    EXIT,
    ENGINEEXIT,
    */

    INITIALIZE,
    READY,
    PHYSICSUPDATE,
    UPDATE,
    ONEVENT,
    DRAW,
    DRAWUI,
    DEBUG,
    EXIT,

    COUNT
};

struct Stage
{
    std::string name;
    ExecutionStep step;
    std::vector<SystemHandle> systems;
};

class EcsPipeline
{
  public:
    EcsPipeline(World world);

    /**
     * @brief Register a new stage under the given execution step.
     * @param step The execution step the stage belongs to.
     * @param name Name identifier for the stage.
     * @return Reference to this pipeline for chaining.
     */
    EcsPipeline &AddStage(ExecutionStep step, const std::string &name);

    /**
     * @brief Register several stages under the given execution step in bulk.
     * @param step The execution step the stages belong to.
     * @param stages Name identifiers for the stages, added in order.
     * @return Reference to this pipeline for chaining.
     */
    EcsPipeline &AddStages(ExecutionStep step, const std::vector<std::string> &stages);

    /**
     * @brief Register a system into an existing stage.
     * @param step The execution step the stage belongs to.
     * @param stageName Name of the stage to add the system to.
     * @param system Handle to the system to add.
     */
    EcsPipeline &AddSystem(ExecutionStep step, const std::string &stageName, const SystemHandle &system);

    /**
     * @brief Run every execution step in enum order.
     * @param deltaTime Delta time forwarded to each system.
     */
    void RunAll(float deltaTime = 0.0f);

    /**
     * @brief Run every stage registered to the given execution step.
     * @param step The execution step to run.
     * @param deltaTime Delta time forwarded to each system.
     */
    void RunStep(ExecutionStep step, float deltaTime = 0.0f);

    /**
     * @brief Run every system in a single stage.
     * @param stage The stage to run.
     * @param deltaTime Delta time forwarded to each system.
     */
    void RunStage(const Stage &stage, float deltaTime = 0.0f);

    /**
     * @brief Run a single system once.
     * @param system Handle to the system to run.
     * @param deltaTime Delta time forwarded to the system.
     */
    void RunSystem(const SystemHandle &system, float deltaTime = 0.0f);

    /**
     * @brief Get the stages registered to a given execution step.
     */
    const std::vector<Stage> &GetStages(ExecutionStep step) const;

    /**
     * @brief Fetch a copy of a stage by name within a given execution step.
     *
     * Returns by value: stages live in a vector and a stored pointer/reference
     * would be invalidated when the vector reallocates, so callers must not hold
     * on to one. To mutate a stage, fetch a copy, modify it, then SetStage() it
     * back.
     *
     * Uses a simple linear search: there are relatively few stages per step and
     * lookups are infrequent, so this is intentionally not indexed.
     *
     * @param step The execution step to search within.
     * @param name Name identifier of the stage to find.
     * @return The matching stage by value, or std::nullopt if none exists.
     */
    std::optional<Stage> GetStage(ExecutionStep step, const std::string &name) const;

    /**
     * @brief Overwrite the stage matching the given name within an execution step.
     *
     * Linear search for the stage whose name matches stage.name and replaces it.
     *
     * @param step The execution step the stage belongs to.
     * @param stage The new stage value (matched against existing stages by name).
     * @return true if a matching stage was found and replaced, false otherwise.
     */
    bool SetStage(ExecutionStep step, const Stage &stage);

  private:
    World world;
    std::array<std::vector<Stage>, static_cast<size_t>(ExecutionStep::COUNT)> stages;

  public:
    std::shared_ptr<ScopedConnection> onReadyConnection;
    std::shared_ptr<ScopedConnection> onEventConnection;
    std::shared_ptr<ScopedConnection> onUpdateConnection;
    std::shared_ptr<ScopedConnection> onPhysicsUpdateConnection;
    std::shared_ptr<ScopedConnection> onDrawConnection;
    std::shared_ptr<ScopedConnection> onDrawUIConnection;
    std::shared_ptr<ScopedConnection> onDebugConnection;
    std::shared_ptr<ScopedConnection> onExitConnection;

    /*
    StepA
        - Stage 1
            SystemX
            SystemY
            SystemZ
        - Stage 2
            SystemH
            SystemJ
            SystemL
    StepB
        - Stage 3
            SystemT
            SystemP
            SystemO
        - Stage 4
            SystemG
            SystemN
            SystemM
    */
};

} // namespace duin