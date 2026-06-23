#include "doctest.h"
#include "TestPipeline.h"
#include <Duin/ECS/DECS/World.h>
#include <Duin/ECS/Pipeline.h>
#include <string>

namespace TestPipeline
{
TEST_SUITE("Pipeline")
{
    TEST_CASE("AddStage registers a stage under the given execution step")
    {
        duin::World w;
        duin::EcsPipeline pipeline(&w);

        pipeline.AddStage(duin::ExecutionStep::UPDATE, "Movement");

        std::optional<duin::Stage> stage = pipeline.GetStage(duin::ExecutionStep::UPDATE, "Movement");
        REQUIRE(stage.has_value());
        CHECK(stage->name == "Movement");
        CHECK(stage->step == duin::ExecutionStep::UPDATE);
    }

    TEST_CASE("AddStage appends multiple stages to the same step")
    {
        duin::World w;
        duin::EcsPipeline pipeline(&w);

        pipeline.AddStage(duin::ExecutionStep::UPDATE, "Input");
        pipeline.AddStage(duin::ExecutionStep::UPDATE, "Move");

        CHECK(pipeline.GetStage(duin::ExecutionStep::UPDATE, "Input").has_value());
        CHECK(pipeline.GetStage(duin::ExecutionStep::UPDATE, "Move").has_value());
    }

    TEST_CASE("AddStages registers stages in bulk")
    {
        duin::World w;
        duin::EcsPipeline pipeline(&w);

        pipeline.AddStages(duin::ExecutionStep::UPDATE, {"Input", "Move", "Resolve"});

        CHECK(pipeline.GetStage(duin::ExecutionStep::UPDATE, "Input").has_value());
        CHECK(pipeline.GetStage(duin::ExecutionStep::UPDATE, "Move").has_value());
        CHECK(pipeline.GetStage(duin::ExecutionStep::UPDATE, "Resolve").has_value());
    }

    TEST_CASE("GetStage returns a copy of the matching stage by name")
    {
        duin::World w;
        duin::EcsPipeline pipeline(&w);

        pipeline.AddStages(duin::ExecutionStep::UPDATE, {"Input", "Move"});

        std::optional<duin::Stage> stage = pipeline.GetStage(duin::ExecutionStep::UPDATE, "Move");
        REQUIRE(stage.has_value());
        CHECK(stage->name == "Move");
        CHECK(stage->step == duin::ExecutionStep::UPDATE);
    }

    TEST_CASE("GetStage returns nullopt when no stage matches")
    {
        duin::World w;
        duin::EcsPipeline pipeline(&w);

        pipeline.AddStage(duin::ExecutionStep::UPDATE, "Input");

        CHECK_FALSE(pipeline.GetStage(duin::ExecutionStep::UPDATE, "Missing").has_value());
    }

    TEST_CASE("SetStage overwrites the matching stage by name")
    {
        duin::World w;
        duin::EcsPipeline pipeline(&w);

        pipeline.AddStage(duin::ExecutionStep::UPDATE, "Move");

        duin::Entity systemEntity = w.Entity("MoveSystem");
        duin::SystemHandle handle{&w, systemEntity};
        duin::Stage updated{"Move", duin::ExecutionStep::UPDATE, {handle}};

        REQUIRE(pipeline.SetStage(duin::ExecutionStep::UPDATE, updated));

        std::optional<duin::Stage> stage = pipeline.GetStage(duin::ExecutionStep::UPDATE, "Move");
        REQUIRE(stage.has_value());

        bool found = false;
        for (const duin::SystemHandle &s : stage->systems)
        {
            if (s.system == systemEntity)
                found = true;
        }
        CHECK(found);
    }

    TEST_CASE("SetStage returns false when no stage matches")
    {
        duin::World w;
        duin::EcsPipeline pipeline(&w);

        pipeline.AddStage(duin::ExecutionStep::UPDATE, "Move");

        duin::Stage missing{"Missing", duin::ExecutionStep::UPDATE, {}};
        CHECK_FALSE(pipeline.SetStage(duin::ExecutionStep::UPDATE, missing));
    }

    TEST_CASE("Stages registered to different steps stay separate")
    {
        duin::World w;
        duin::EcsPipeline pipeline(&w);

        pipeline.AddStage(duin::ExecutionStep::UPDATE, "UpdateStage");
        pipeline.AddStage(duin::ExecutionStep::DRAW, "DrawStage");

        CHECK(pipeline.GetStage(duin::ExecutionStep::UPDATE, "UpdateStage").has_value());
        CHECK(pipeline.GetStage(duin::ExecutionStep::DRAW, "DrawStage").has_value());
        CHECK_FALSE(pipeline.GetStage(duin::ExecutionStep::DRAW, "UpdateStage").has_value());
        CHECK_FALSE(pipeline.GetStage(duin::ExecutionStep::UPDATE, "DrawStage").has_value());
    }

    TEST_CASE("AddSystem adds a system handle to the named stage")
    {
        duin::World w;
        duin::EcsPipeline pipeline(&w);

        pipeline.AddStage(duin::ExecutionStep::UPDATE, "Movement");

        duin::Entity systemEntity = w.Entity("MoveSystem");
        duin::SystemHandle handle(&w, systemEntity);
        pipeline.AddSystem(duin::ExecutionStep::UPDATE, "Movement", handle);

        std::optional<duin::Stage> stage = pipeline.GetStage(duin::ExecutionStep::UPDATE, "Movement");
        REQUIRE(stage.has_value());

        bool found = false;
        for (const duin::SystemHandle &s : stage->systems)
        {
            if (s.system == systemEntity)
                found = true;
        }
        CHECK(found);
    }

    TEST_CASE("RunStep executes without affecting registered stages")
    {
        duin::World w;
        duin::EcsPipeline pipeline(&w);

        pipeline.AddStage(duin::ExecutionStep::UPDATE, "Movement");
        pipeline.RunStep(duin::ExecutionStep::UPDATE, 0.016f);

        CHECK(pipeline.GetStage(duin::ExecutionStep::UPDATE, "Movement").has_value());
    }

    TEST_CASE("RunAll runs every execution step without throwing")
    {
        duin::World w;
        duin::EcsPipeline pipeline(&w);

        pipeline.AddStage(duin::ExecutionStep::INITIALIZE, "Init");
        pipeline.AddStage(duin::ExecutionStep::UPDATE, "Update");
        pipeline.AddStage(duin::ExecutionStep::DRAW, "Draw");

        pipeline.RunAll(0.016f);

        CHECK(pipeline.GetStage(duin::ExecutionStep::INITIALIZE, "Init").has_value());
        CHECK(pipeline.GetStage(duin::ExecutionStep::UPDATE, "Update").has_value());
        CHECK(pipeline.GetStage(duin::ExecutionStep::DRAW, "Draw").has_value());
    }
}
} // namespace TestPipeline
