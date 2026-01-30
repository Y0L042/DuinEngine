#include "SceneViewport.h"
#include <Duin/Core/Application.h>
#include <Duin/Render/RenderModule.h>
#include <Duin/Core/Debug/DebugModule.h>

SceneViewport::SceneViewport()
{
    m_windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse;
}

SceneViewport::~SceneViewport()
{
}

void SceneViewport::Init()
{
    renderTarget = duin::RenderTexture(duin::GetWindowWidth(), duin::GetWindowHeight(), 0);
    mainCamera = CreateChildObject<EditorCamera>();
    mainCamera->Enable(true);
    mainCamera->UpdateTarget({0.0f, 0.0f, 0.0f});
    DN_INFO("SceneViewport initialized.");
}

void SceneViewport::PhysicsUpdate(double delta)
{
}

void SceneViewport::Draw()
{
    RenderToTexture();
}

void SceneViewport::DrawUI()
{
    ImGui::Begin("SceneViewport");

    ImGui::Text("This is my custom Viewport window content!");

    ::ImVec2 avail = ImGui::GetWindowSize();
    int windowX = avail.x;
    int windowY = avail.y;

    float dispW, dispH = 0.0f;
    dispH = avail.y;
    dispW = avail.x;

    float centerX = (ImGui::GetWindowSize().x - dispW) * 0.5f;
    float centerY = (ImGui::GetWindowSize().y - dispH) * 0.5f;

    ::ImVec2 center(centerX, centerY);
    ImGui::SetCursorPos(center);

    duin::DrawIMGUITexture(renderTarget, {dispW, dispH});

    ImGui::End();
}

void SceneViewport::RenderToTexture()
{
    duin::BeginTextureMode(renderTarget);
    duin::ClearBackground(duin::GRAY);

    duin::DrawGrid(100.0f);
    duin::DrawDebugSphere({ 15, 0, 0 }, 1);
    duin::DrawDebugSphere({ 0, 15, 0 }, 2);
    duin::DrawDebugSphere({ 0, 0, 15 }, 4);

    mainCamera->Render();

    duin::EndTextureMode();
}
