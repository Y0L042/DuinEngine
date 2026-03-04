#pragma once
#include <external/imgui.h>
#include <Duin/Core/Signals/Signal.h>
#include <memory>
#include "Scene.h"
#include <vector>
#include <Duin/Objects/GameObject.h>
#include "GUI/UIObject.h"

class SceneTabs : public duin::GameObject, public UIObject
{
  public:
    duin::Signal<std::weak_ptr<Scene>> onActiveTabChanged;

    SceneTabs();
    ~SceneTabs();

    void Init() override;
    void PhysicsUpdate(double delta) override;
    void Draw() override;
    void DrawUI() override;

    void AddTab(std::weak_ptr<Scene> scene);
    void SetActiveTabIdx(int idx);

    void CacheActiveScene(std::weak_ptr<Scene> scene);

  private:
    ImGuiWindowFlags imguiWindowFlags;
    std::vector<std::weak_ptr<Scene>> tabs;
    int activeTabIdx = -1;
    int pendingActiveIdx = -1;
};