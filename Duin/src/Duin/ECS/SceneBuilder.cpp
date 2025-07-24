#include "dnpch.h"
#include "SceneBuilder.h"
#include "ECSManager.h"
#include "Duin/IO/DataValue.h"

void duin::SceneBuilder::ReadScene(const std::string& file, ECSManager& ecs)
{
    DataValue root(file);

    flecs::entity e_root = ecs.world.entity();
    e_root.is_a(ecs.world.lookup("TYPE"));
}

void duin::SceneBuilder::WriteScene(const std::string& file, const std::string& scene)
{

}
