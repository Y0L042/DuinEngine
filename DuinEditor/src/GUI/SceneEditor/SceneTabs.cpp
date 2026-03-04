#include "SceneTabs.h"

SceneTabs::SceneTabs()
{
}

SceneTabs::~SceneTabs()
{
}

void SceneTabs::Init()
{
    imguiWindowFlags = ImGuiWindowFlags_NoCollapse;
}

void SceneTabs::PhysicsUpdate(double delta)
{
}

void SceneTabs::Draw()
{
}

void SceneTabs::DrawUI()
{
    ImGui::Begin("Scenes", nullptr, imguiWindowFlags);

    if (ImGui::BeginTabBar("SceneTabBar"))
    {
        for (int i = 0; i < (int)tabs.size(); ++i)
        {
            if (tabs[i].expired()) continue;

            const std::string &name = tabs[i].lock()->sceneName;
            ImGuiTabItemFlags flags = (i == pendingActiveIdx) ? ImGuiTabItemFlags_SetSelected : 0;

            if (ImGui::BeginTabItem(name.c_str(), nullptr, flags))
            {
                if (i != activeTabIdx)
                {
                    activeTabIdx = i;
                    onActiveTabChanged.Emit(tabs[i]);
                }
                ImGui::EndTabItem();
            }
        }
        pendingActiveIdx = -1;
        ImGui::EndTabBar();
    }

    ImGui::End();
}

void SceneTabs::AddTab(std::weak_ptr<Scene> scene)
{
    tabs.push_back(scene);
    SetActiveTabIdx((int)tabs.size() - 1);
    onActiveTabChanged.Emit(scene   );
}

void SceneTabs::SetActiveTabIdx(int idx)
{
    activeTabIdx = idx;
    pendingActiveIdx = activeTabIdx;
}

void SceneTabs::CacheActiveScene(std::weak_ptr<Scene> scene)
{
    if (scene.expired())
    {
        DN_WARN("Expired scene passed");
        return;
    }

    bool tabExist = false;
    int tabId = -1;
    size_t i = 0;
    for (auto tab : tabs)
    {
        if (tab.expired())
        {
            DN_WARN("Tab expired! Remove it.");
            i++;
            continue;
        }
        if (scene.lock()->uuid == tab.lock()->uuid)
        {
            tabExist = true;
            tabId = i;
            break;
        }
        i++;
    }
    if (!tabExist)
    {
        AddTab(scene);
    }
    else
    {
        SetActiveTabIdx(tabId);
    }
}
