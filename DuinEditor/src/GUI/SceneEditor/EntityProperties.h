#pragma once
#include <external/imgui.h>
#include <memory>
#include <Duin/ECS/DECS/World.h>
#include <string>
#include <Duin/ECS/DECS/Entity.h>
#include <Duin/Objects/GameObject.h>

class State_SceneEditor;
class EntityProperties : public duin::GameObject
{
  public:
    EntityProperties();
    ~EntityProperties();

    void Init() override;
    void PhysicsUpdate(double delta) override;
    void Draw() override;
    void DrawUI() override;

    void SetFocusedEntity(duin::Entity entity);

  private:
    ImGuiWindowFlags imguiWindowFlags;
    duin::Entity focusedEntity;
};