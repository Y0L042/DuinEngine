#include "FileTree.h"

FileTree::FileTree()
{
}

FileTree::~FileTree()
{
}

void FileTree::Init()
{
    imguiWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse;
}

void FileTree::PhysicsUpdate(double delta)
{
}

void FileTree::Draw()
{
}

void FileTree::DrawUI()
{
    ImGui::Begin("FileTree", nullptr, imguiWindowFlags);

    ImGui::Text("This is my custom FileTree window content!");

    ImGui::End();
}
