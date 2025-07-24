#include "dnpch.h"
#include "SceneBuilder.h"
#include "ECSManager.h"
#include "Duin/IO/DataValue.h"
#include <flecs.h>

/**
 * Reads an ECS scenetree from file and generates the scenetree.
 */
void duin::SceneBuilder::ReadScene(const std::string& file, ECSManager& ecs)
{
    DataValue root(file);

    flecs::entity e_root = ecs.world.entity();
    e_root.is_a(ecs.world.lookup("TYPE"));
}

void duin::SceneBuilder::WriteScene(std::string& file, ECSManager& ecs)
{
    DataValue sceneTree;
    DataValue children;
    children.SetArray();

    /* Loop through each entity in world root */
    ecs.world.children([&](flecs::entity e) {
        DataValue childData;
        RecursiveWriter(e, childData);
        children.PushBack(childData);
        });
    sceneTree.AddMember("Children", children);

    file = DataValue::Write(sceneTree, true);
}

void duin::SceneBuilder::WriteEntity(std::string& file, flecs::entity entity)
{
    DataValue tree;
    DataValue children;

    /* Loop through root entity's children */
    children.SetArray();
    entity.children([&](flecs::entity e) {
        DataValue childData;
        RecursiveWriter(e, childData);
        children.PushBack(childData);
        });
    tree.AddMember("Children", children);

    file = DataValue::Write(tree, true);
}

void duin::SceneBuilder::RecursiveWriter(flecs::entity entity, DataValue& entityData)
{
    // Serialise entity
    DataValue ecsID((int)entity);
    DataValue children;

    entityData.AddMember("ECS_ID", ecsID);

    // Loop through children
    children.SetArray();
    entity.children([&](flecs::entity e) {
        DataValue childData;
        RecursiveWriter(e, childData);
        children.PushBack(childData);
        }); 
    entityData.AddMember("Children", children);
}
