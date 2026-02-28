#include "SceneViewport.h"
#include <Duin/Core/Application.h>
#include <Duin/Render/RenderModule.h>
#include <Duin/Core/Debug/DebugModule.h>
#include <EditorStates/State_SceneEditor.h>

SceneViewport::SceneViewport()
{
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
    imguiWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse;

    DN_INFO("SceneViewport initialized.");
}

void SceneViewport::PhysicsUpdate(double delta)
{
}

void SceneViewport::Draw()
{
}

void SceneViewport::DrawUI()
{
    ImGui::Begin("SceneViewport", nullptr, imguiWindowFlags);

    ::ImVec2 avail = ImGui::GetWindowSize();
    float dispW = avail.x;
    float dispH = avail.y;

    float centerX = (avail.x - dispW) * 0.5f;
    float centerY = (avail.y - dispH) * 0.5f;

    ::ImVec2 center(centerX, centerY);
    ImGui::SetCursorPos(center);

    duin::DrawIMGUITexture(renderTarget, {dispW, dispH});

    // --- ImGuizmo gizmo overlay ---
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::BeginFrame();

    ::ImVec2 windowPos = ImGui::GetWindowPos();
    ImGuizmo::SetRect(windowPos.x, windowPos.y, dispW, dispH);
    ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());

    duin::Camera &cam = mainCamera->camera;
    duin::Matrix viewMtx = duin::GetCameraViewMatrix(&cam);
    duin::Matrix projMtx = duin::GetCameraProjectionMatrix(&cam);

    // duin::Matrix is column-major (OpenGL style); ImGuizmo expects the same layout.
    // Read the floats in storage order directly.
    float view[16] = {
        viewMtx.m0, viewMtx.m1, viewMtx.m2,  viewMtx.m3,  viewMtx.m4,  viewMtx.m5,  viewMtx.m6,  viewMtx.m7,
        viewMtx.m8, viewMtx.m9, viewMtx.m10, viewMtx.m11, viewMtx.m12, viewMtx.m13, viewMtx.m14, viewMtx.m15,
    };
    float proj[16] = {
        projMtx.m0, projMtx.m1, projMtx.m2,  projMtx.m3,  projMtx.m4,  projMtx.m5,  projMtx.m6,  projMtx.m7,
        projMtx.m8, projMtx.m9, projMtx.m10, projMtx.m11, projMtx.m12, projMtx.m13, projMtx.m14, projMtx.m15,
    };

    ImGuizmo::Manipulate(view, proj, gizmoOperation, ImGuizmo::WORLD, gizmoMatrix);

    ImGui::End();
}

void SceneViewport::StartTextureRender()
{
    duin::BeginTextureMode(renderTarget);
    duin::ClearBackground(duin::GRAY);
    duin::DrawGrid(100.0f);
}

void SceneViewport::EndTextureRender()
{
    mainCamera->Render();
    duin::EndTextureMode();
}

void SceneViewport::SetActiveWorld(std::weak_ptr<duin::GameWorld> world)
{
    activeWorld = world;
}
