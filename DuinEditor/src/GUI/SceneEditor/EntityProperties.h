#pragma once
#include <external/imgui.h>
#include <memory>
#include <vector>
#include <Duin/ECS/DECS/World.h>
#include <string>
#include <Duin/ECS/DECS/Entity.h>
#include <Duin/Objects/GameObject.h>

class State_SceneEditor;
class EntityProperties : public duin::GameObject
{
  public:
    struct ComponentNode
    {
        uint64_t cmpID;
        std::string cmpName;
    };

    EntityProperties();
    ~EntityProperties();

    void Init() override;
    void PhysicsUpdate(double delta) override;
    void Draw() override;
    void DrawUI() override;

    void SetFocusedEntity(duin::Entity entity);

  private:
    ImGuiWindowFlags imguiWindowFlags;
    std::shared_ptr<State_SceneEditor> sceneEditor;
    duin::Entity focusedEntity;

    std::vector<ComponentNode> componentNodeList;
};