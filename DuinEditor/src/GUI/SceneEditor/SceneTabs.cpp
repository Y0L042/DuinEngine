#include "SceneTabs.h"

SceneTabs::SceneTabs()
{
}

SceneTabs::~SceneTabs()
{
}

void SceneTabs::Init()
{
    imguiWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse;
}

void SceneTabs::PhysicsUpdate(double delta)
{
}

void SceneTabs::Draw()
{
}

void SceneTabs::DrawUI()
{
    ImGui::Begin("SceneTabs", nullptr, imguiWindowFlags);

    ImGui::Text("This is my custom SceneTabs window content!");

    ImGui::End();
}
