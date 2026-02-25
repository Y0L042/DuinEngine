#pragma once
#include <external/imgui.h>
#include <memory>
#include <vector>
#include <Duin/ECS/DECS/World.h>
#include <string>
#include <Duin/ECS/DECS/Entity.h>
#include <Duin/Objects/GameObject.h>

class State_SceneEditor;
class SceneTree : public duin::GameObject
{
  public:
      struct EntityNode
      {
          uint64_t entityId = 0;
          std::string entityName = "Entity";
          std::string entitySupertype = "None";
          std::vector<EntityNode> children;
      };

    SceneTree();
    ~SceneTree();

    void Init() override;
    void PhysicsUpdate(double delta) override;
    void Draw() override;
    void DrawUI() override;

    void UpdateTree(duin::World *world);

  private:
    ImGuiWindowFlags imguiWindowFlags;
    std::shared_ptr<State_SceneEditor> sceneEditor;
    std::vector<EntityNode> entityTree;
    duin::World *activeWorld = nullptr;
    uint64_t selectedEntityId = 0;

    void UpdateTreeImpl(std::vector<duin::Entity> entities);
    EntityNode UpdateChild(duin::Entity& child);
    void DrawEntityNode(const EntityNode& node);
};