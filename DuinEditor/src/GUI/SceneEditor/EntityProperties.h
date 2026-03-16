#pragma once
#include <external/imgui.h>
#include <memory>
#include <any>
#include <Duin/ECS/DECS/World.h>
#include <string>
#include <Duin/ECS/DECS/Entity.h>
#include <Duin/Objects/GameObject.h>
#include <Duin/ECS/ECSComponents.h>
#include "GUI/UIObject.h"
#include <Scene.h>

class CommandManager;

struct FieldEditState
{
    bool changed = false;
    bool activated = false;
    bool deactivatedAfterEdit = false;
};

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

    bool AddComponent();
    bool RemoveComponent();

    bool DrawComponent(const std::string& typeName, void* rawPtr);

    template<typename T>
    bool DrawComponentTyped(void* rawPtr);

    template<typename T>
    static FieldEditState DrawField(std::string_view name, T& value);

    void CacheActiveScene(std::weak_ptr<Scene> scene);
    void SetFocusedEntity(duin::Entity entity);
    void SetCommandManager(std::weak_ptr<CommandManager> mgr);

  private:
    std::weak_ptr<Scene> cachedActiveScene;
    ImGuiWindowFlags imguiWindowFlags;
    duin::Entity focusedEntity;
    std::weak_ptr<CommandManager> commandManager;
    std::any pendingOldValue;
    bool pendingEditActive = false;
    bool drawAddNewComponentMenu = false;
};