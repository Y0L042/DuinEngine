#include "doctest.h"
#include "TestPipeline.h"
#include <Duin/ECS/DECS/World.h>
#include <Duin/ECS/Pipeline.h>
#include <Duin/ECS/DECS/System.h>
#include <Duin/Core/Application.h>
#include <memory>
#include <string>
#include <vector>

namespace TestPipeline
{
TEST_SUITE("Pipeline")
{
    TEST_CASE("AddStage registers a stage under the given execution step")
    {
        duin::World w;
        duin::EcsPipeline pipeline(w);

        pipeline.AddStage(duin::ExecutionStep::UPDATE, "Movement");

        std::optional<duin::Stage> stage = pipeline.GetStage(duin::ExecutionStep::UPDATE, "Movement");
        REQUIRE(stage.has_value());
        CHECK(stage->name == "Movement");
        CHECK(stage->step == duin::ExecutionStep::UPDATE);
    }

    TEST_CASE("AddStage appends multiple stages to the same step")
    {
        duin::World w;
        duin::EcsPipeline pipeline(w);

        pipeline.AddStage(duin::ExecutionStep::UPDATE, "Input");
        pipeline.AddStage(duin::ExecutionStep::UPDATE, "Move");

        CHECK(pipeline.GetStage(duin::ExecutionStep::UPDATE, "Input").has_value());
        CHECK(pipeline.GetStage(duin::ExecutionStep::UPDATE, "Move").has_value());
    }

    TEST_CASE("AddStages registers stages in bulk")
    {
        duin::World w;
        duin::EcsPipeline pipeline(w);

        pipeline.AddStages(duin::ExecutionStep::UPDATE, {"Input", "Move", "Resolve"});

        CHECK(pipeline.GetStage(duin::ExecutionStep::UPDATE, "Input").has_value());
        CHECK(pipeline.GetStage(duin::ExecutionStep::UPDATE, "Move").has_value());
        CHECK(pipeline.GetStage(duin::ExecutionStep::UPDATE, "Resolve").has_value());
    }

    TEST_CASE("GetStage returns a copy of the matching stage by name")
    {
        duin::World w;
        duin::EcsPipeline pipeline(w);

        pipeline.AddStages(duin::ExecutionStep::UPDATE, {"Input", "Move"});

        std::optional<duin::Stage> stage = pipeline.GetStage(duin::ExecutionStep::UPDATE, "Move");
        REQUIRE(stage.has_value());
        CHECK(stage->name == "Move");
        CHECK(stage->step == duin::ExecutionStep::UPDATE);
    }

    TEST_CASE("GetStage returns nullopt when no stage matches")
    {
        duin::World w;
        duin::EcsPipeline pipeline(w);

        pipeline.AddStage(duin::ExecutionStep::UPDATE, "Input");

        CHECK_FALSE(pipeline.GetStage(duin::ExecutionStep::UPDATE, "Missing").has_value());
    }

    TEST_CASE("SetStage overwrites the matching stage by name")
    {
        duin::World w;
        duin::EcsPipeline pipeline(w);

        pipeline.AddStage(duin::ExecutionStep::UPDATE, "Move");

        duin::Entity systemEntity = w.Entity("MoveSystem");
        duin::SystemHandle handle{w, systemEntity};
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
        duin::EcsPipeline pipeline(w);

        pipeline.AddStage(duin::ExecutionStep::UPDATE, "Move");

        duin::Stage missing{"Missing", duin::ExecutionStep::UPDATE, {}};
        CHECK_FALSE(pipeline.SetStage(duin::ExecutionStep::UPDATE, missing));
    }

    TEST_CASE("Stages registered to different steps stay separate")
    {
        duin::World w;
        duin::EcsPipeline pipeline(w);

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
        duin::EcsPipeline pipeline(w);

        pipeline.AddStage(duin::ExecutionStep::UPDATE, "Movement");

        duin::Entity systemEntity = w.Entity("MoveSystem");
        duin::SystemHandle handle{w, systemEntity};
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
        duin::EcsPipeline pipeline(w);

        pipeline.AddStage(duin::ExecutionStep::UPDATE, "Movement");
        pipeline.RunStep(duin::ExecutionStep::UPDATE, 0.016f);

        CHECK(pipeline.GetStage(duin::ExecutionStep::UPDATE, "Movement").has_value());
    }

    TEST_CASE("RunAll runs every execution step without throwing")
    {
        duin::World w;
        duin::EcsPipeline pipeline(w);

        pipeline.AddStage(duin::ExecutionStep::INITIALIZE, "Init");
        pipeline.AddStage(duin::ExecutionStep::UPDATE, "Update");
        pipeline.AddStage(duin::ExecutionStep::DRAW, "Draw");

        pipeline.RunAll(0.016f);

        CHECK(pipeline.GetStage(duin::ExecutionStep::INITIALIZE, "Init").has_value());
        CHECK(pipeline.GetStage(duin::ExecutionStep::UPDATE, "Update").has_value());
        CHECK(pipeline.GetStage(duin::ExecutionStep::DRAW, "Draw").has_value());
    }

    TEST_CASE("RunStep runs a real system added with AddSystem")
    {
        duin::World w;
        struct Counter
        {
            int value = 0;
        };
        w.Component<Counter>();
        duin::Entity e = w.Entity("Counted");
        e.Set<Counter>({0});

        duin::System sys = w.System<Counter>("IncrementSystem").Each([](duin::Entity, Counter &c) { c.value++; });

        duin::EcsPipeline pipeline(w);
        pipeline.AddStage(duin::ExecutionStep::UPDATE, "Logic");
        pipeline.AddSystem(duin::ExecutionStep::UPDATE, "Logic", sys.GetHandle());

        pipeline.RunStep(duin::ExecutionStep::UPDATE, 0.016f);
        CHECK(e.TryGet<Counter>()->value == 1);

        pipeline.RunStep(duin::ExecutionStep::DRAW, 0.016f);
        CHECK(e.TryGet<Counter>()->value == 1);

        pipeline.RunAll(0.016f);
        CHECK(e.TryGet<Counter>()->value == 2);
    }

    TEST_CASE("RunStage runs only the given stage")
    {
        duin::World w;
        std::vector<std::string> order;

        duin::System a = w.System<>("SysA").Run([&](duin::Iter &) { order.push_back("A"); });
        duin::System b = w.System<>("SysB").Run([&](duin::Iter &) { order.push_back("B"); });

        duin::EcsPipeline pipeline(w);
        pipeline.AddStages(duin::ExecutionStep::UPDATE, {"First", "Second"});
        pipeline.AddSystem(duin::ExecutionStep::UPDATE, "First", a.GetHandle());
        pipeline.AddSystem(duin::ExecutionStep::UPDATE, "Second", b.GetHandle());

        std::optional<duin::Stage> second = pipeline.GetStage(duin::ExecutionStep::UPDATE, "Second");
        REQUIRE(second.has_value());
        pipeline.RunStage(*second);
        REQUIRE(order.size() == 1);
        CHECK(order[0] == "B");
    }

    TEST_CASE("RunStep runs stages in registration order and systems in insertion order")
    {
        duin::World w;
        std::vector<std::string> order;

        duin::System a = w.System<>("OrderA").Run([&](duin::Iter &) { order.push_back("A"); });
        duin::System b = w.System<>("OrderB").Run([&](duin::Iter &) { order.push_back("B"); });
        duin::System c = w.System<>("OrderC").Run([&](duin::Iter &) { order.push_back("C"); });

        duin::EcsPipeline pipeline(w);
        pipeline.AddStages(duin::ExecutionStep::UPDATE, {"Early", "Late"});
        pipeline.AddSystem(duin::ExecutionStep::UPDATE, "Late", c.GetHandle());
        pipeline.AddSystem(duin::ExecutionStep::UPDATE, "Early", b.GetHandle());
        pipeline.AddSystem(duin::ExecutionStep::UPDATE, "Early", a.GetHandle());

        pipeline.RunStep(duin::ExecutionStep::UPDATE);

        REQUIRE(order.size() == 3);
        CHECK(order[0] == "B");
        CHECK(order[1] == "A");
        CHECK(order[2] == "C");
    }

    TEST_CASE("RunAll runs steps in enum order")
    {
        duin::World w;
        std::vector<std::string> order;

        duin::System draw = w.System<>("DrawSys").Run([&](duin::Iter &) { order.push_back("DRAW"); });
        duin::System init = w.System<>("InitSys").Run([&](duin::Iter &) { order.push_back("INITIALIZE"); });
        duin::System update = w.System<>("UpdateSys").Run([&](duin::Iter &) { order.push_back("UPDATE"); });

        duin::EcsPipeline pipeline(w);
        pipeline.AddStage(duin::ExecutionStep::DRAW, "Draw");
        pipeline.AddStage(duin::ExecutionStep::INITIALIZE, "Init");
        pipeline.AddStage(duin::ExecutionStep::UPDATE, "Update");
        pipeline.AddSystem(duin::ExecutionStep::DRAW, "Draw", draw.GetHandle());
        pipeline.AddSystem(duin::ExecutionStep::INITIALIZE, "Init", init.GetHandle());
        pipeline.AddSystem(duin::ExecutionStep::UPDATE, "Update", update.GetHandle());

        pipeline.RunAll();

        REQUIRE(order.size() == 3);
        CHECK(order[0] == "INITIALIZE");
        CHECK(order[1] == "UPDATE");
        CHECK(order[2] == "DRAW");
    }

    TEST_CASE("RunSystem forwards delta time")
    {
        duin::World w;
        float received = -1.0f;
        duin::System sys = w.System<>("DeltaSys").Run([&](duin::Iter &it) { received = it.DeltaTime(); });

        duin::EcsPipeline pipeline(w);
        pipeline.RunSystem(sys.GetHandle(), 0.25f);

        CHECK(received == doctest::Approx(0.25f));
    }

    TEST_CASE("RunSystem with an empty handle is a no-op")
    {
        duin::World w;
        duin::EcsPipeline pipeline(w);

        duin::SystemHandle empty{};
        pipeline.RunSystem(empty, 0.016f);

        duin::SystemHandle nullWorld{duin::World(static_cast<ecs_world_t *>(nullptr)), duin::Entity()};
        pipeline.RunSystem(nullWorld, 0.016f);

        CHECK(pipeline.GetStages(duin::ExecutionStep::UPDATE).empty());
    }

    TEST_CASE("AddSystem to a missing stage does nothing")
    {
        duin::World w;
        duin::EcsPipeline pipeline(w);
        pipeline.AddStage(duin::ExecutionStep::UPDATE, "Existing");

        duin::Entity systemEntity = w.Entity("Orphan");
        pipeline.AddSystem(duin::ExecutionStep::UPDATE, "Missing", duin::SystemHandle{w, systemEntity});

        CHECK_FALSE(pipeline.GetStage(duin::ExecutionStep::UPDATE, "Missing").has_value());
        CHECK(pipeline.GetStage(duin::ExecutionStep::UPDATE, "Existing")->systems.empty());
        CHECK(pipeline.GetStages(duin::ExecutionStep::UPDATE).size() == 1);
    }

    TEST_CASE("Mutating a GetStage copy does not change the pipeline")
    {
        duin::World w;
        duin::EcsPipeline pipeline(w);
        pipeline.AddStage(duin::ExecutionStep::UPDATE, "Stage");

        std::optional<duin::Stage> copy = pipeline.GetStage(duin::ExecutionStep::UPDATE, "Stage");
        REQUIRE(copy.has_value());
        copy->systems.push_back(duin::SystemHandle{w, w.Entity("Extra")});
        copy->name = "Renamed";

        CHECK(pipeline.GetStage(duin::ExecutionStep::UPDATE, "Stage")->systems.empty());
        CHECK_FALSE(pipeline.GetStage(duin::ExecutionStep::UPDATE, "Renamed").has_value());
    }
}

class PipelineTestApp : public duin::Application
{
};

static void ReleasePipelineConnections(const std::shared_ptr<duin::EcsPipeline> &pipeline)
{
    pipeline->onReadyConnection.reset();
    pipeline->onEventConnection.reset();
    pipeline->onUpdateConnection.reset();
    pipeline->onPhysicsUpdateConnection.reset();
    pipeline->onDrawConnection.reset();
    pipeline->onDrawUIConnection.reset();
    pipeline->onDebugConnection.reset();
    pipeline->onExitConnection.reset();
}

TEST_SUITE("Pipeline - CreateEcsPipeline")
{
    TEST_CASE("Application post callbacks drive the matching execution steps")
    {
        PipelineTestApp app;
        duin::World w;
        std::vector<std::string> ran;

        std::shared_ptr<duin::EcsPipeline> pipeline = duin::CreateEcsPipeline(w);
        auto addStep = [&](duin::ExecutionStep step, const char *name) {
            duin::System sys = w.System<>(name).Run([&ran, name](duin::Iter &) { ran.push_back(name); });
            pipeline->AddStage(step, name);
            pipeline->AddSystem(step, name, sys.GetHandle());
        };
        addStep(duin::ExecutionStep::INITIALIZE, "INITIALIZE");
        addStep(duin::ExecutionStep::READY, "READY");
        addStep(duin::ExecutionStep::UPDATE, "UPDATE");
        addStep(duin::ExecutionStep::PHYSICSUPDATE, "PHYSICSUPDATE");
        addStep(duin::ExecutionStep::DRAW, "DRAW");
        addStep(duin::ExecutionStep::DRAWUI, "DRAWUI");
        addStep(duin::ExecutionStep::DEBUG, "DEBUG");

        SUBCASE("Ready")
        {
            app.EnginePostReady();
            REQUIRE(ran.size() == 1);
            CHECK(ran[0] == "READY");
        }
        SUBCASE("Update")
        {
            app.EnginePostUpdate(0.016);
            REQUIRE(ran.size() == 1);
            CHECK(ran[0] == "UPDATE");
        }
        SUBCASE("PhysicsUpdate")
        {
            app.EnginePostPhysicsUpdate(0.016);
            REQUIRE(ran.size() == 1);
            CHECK(ran[0] == "PHYSICSUPDATE");
        }
        SUBCASE("Draw")
        {
            app.EnginePostDraw();
            REQUIRE(ran.size() == 1);
            CHECK(ran[0] == "DRAW");
        }
        SUBCASE("DrawUI")
        {
            app.EnginePostDrawUI();
            REQUIRE(ran.size() == 1);
            CHECK(ran[0] == "DRAWUI");
        }
        SUBCASE("Debug")
        {
            app.EnginePostDebug();
            REQUIRE(ran.size() == 1);
            CHECK(ran[0] == "DEBUG");
        }
        SUBCASE("INITIALIZE is never run automatically")
        {
            app.EnginePostReady();
            app.EnginePostUpdate(0.016);
            app.EnginePostPhysicsUpdate(0.016);
            app.EnginePostDraw();
            app.EnginePostDrawUI();
            app.EnginePostDebug();
            for (const std::string &name : ran)
            {
                CHECK(name != "INITIALIZE");
            }
        }

        ReleasePipelineConnections(pipeline);
    }

    TEST_CASE("Released connections stop driving the pipeline")
    {
        PipelineTestApp app;
        duin::World w;
        int count = 0;

        std::shared_ptr<duin::EcsPipeline> pipeline = duin::CreateEcsPipeline(w);
        duin::System sys = w.System<>("CountUpdate").Run([&](duin::Iter &) { count++; });
        pipeline->AddStage(duin::ExecutionStep::UPDATE, "Update");
        pipeline->AddSystem(duin::ExecutionStep::UPDATE, "Update", sys.GetHandle());

        app.EnginePostUpdate(0.016);
        CHECK(count == 1);

        ReleasePipelineConnections(pipeline);
        app.EnginePostUpdate(0.016);
        CHECK(count == 1);
    }

    TEST_CASE("Pipeline is freed once the caller drops it" * doctest::may_fail())
    {
        duin::World w;
        std::shared_ptr<duin::EcsPipeline> pipeline = duin::CreateEcsPipeline(w);
        std::weak_ptr<duin::EcsPipeline> weak = pipeline;

        pipeline.reset();
        bool expired = weak.expired();

        if (std::shared_ptr<duin::EcsPipeline> leaked = weak.lock())
        {
            ReleasePipelineConnections(leaked);
        }

        CHECK(expired);
    }
}
} // namespace TestPipeline
