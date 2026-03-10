#pragma once
#include <external/imgui.h>
#include <memory>
#include <Duin/ECS/DECS/World.h>
#include <string>
#include <Duin/ECS/DECS/Entity.h>
#include <Duin/Objects/GameObject.h>
#include "GUI/UIObject.h"
#include <Scene.h>

class State_SceneEditor;
class EntityProperties : public duin::GameObject, public UIObject
{
  public:
    EntityProperties();
    ~EntityProperties();

    void Init() override;
    void PhysicsUpdate(double delta) override;
    void Draw() override;
    void DrawUI() override;

    void CacheActiveScene(std::weak_ptr<Scene> scene);
    void SetFocusedEntity(duin::Entity entity);

  private:
    std::weak_ptr<Scene> cachedActiveScene;
    ImGuiWindowFlags imguiWindowFlags;
    duin::Entity focusedEntity;
};