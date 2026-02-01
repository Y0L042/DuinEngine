#include "SceneTree.h"

SceneTree::SceneTree()
{
}

SceneTree::~SceneTree()
{
}

void SceneTree::Init()
{
        imguiWindowFlags = ImGuiWindowFlags_MenuBar |
                    ImGuiWindowFlags_NoCollapse;
}

void SceneTree::PhysicsUpdate(double delta)
{
}

void SceneTree::Draw()
{
}

void SceneTree::DrawUI()
{
	ImGui::Begin("SceneTree", nullptr, imguiWindowFlags);

    ImGui::Text("This is my custom SceneTree window content!");

    ImGui::End();
}
