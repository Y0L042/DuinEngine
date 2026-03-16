#pragma once
#include <external/imgui.h>
#include <memory>
#include <vector>
#include <Duin/ECS/DECS/World.h>
#include <string>
#include <Duin/ECS/DECS/Entity.h>
#include <Duin/Objects/GameObject.h>
#include "Scene.h"
#include "GUI/UIObject.h"

class State_SceneEditor;
class SceneTree : public duin::GameObject, public UIObject
{
  public:
    struct EntityNode
    {
        uint64_t entityId = 0;
        std::string entityName = "Entity";
        std::string entitySupertype = "None";
        std::vector<EntityNode> children;
    };

    duin::Signal<duin::Entity>                  onSelectEntity;
    duin::Signal<>                              onAddEntity;
    duin::Signal<duin::Entity>                  onAddChildEntity;
    duin::Signal<duin::Entity>                  onRemoveEntity;
    duin::Signal<duin::Entity, duin::Entity>    onReparentEntity;
    duin::Signal<duin::Entity>                  onReparentEntityToRoot;

    SceneTree();
    ~SceneTree();

    void Init() override;
    void PhysicsUpdate(double delta) override;
    void Draw() override;
    void DrawUI() override;

    void CacheActiveScene(std::weak_ptr<Scene> scene);

    void UpdateTree();

  private:
    ImGuiWindowFlags imguiWindowFlags;
    std::weak_ptr<Scene> cachedActiveScene;
    std::vector<EntityNode> entityTree;

    uint64_t selectedEntityId = 0;
    uint64_t dragSourceEntityId = 0;

    void UpdateTreeImpl(std::vector<duin::Entity> entities);
    EntityNode UpdateChild(duin::Entity &child);
    void DrawEntityNode(const EntityNode &node);
};