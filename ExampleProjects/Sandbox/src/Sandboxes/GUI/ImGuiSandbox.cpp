#include "ImGuiSandbox.h"
#include <Duin/Core/Application.h>
#include <Duin/Render/RenderModule.h>
#include <external/imgui.h>
#include <ImGuizmo.h>

ImGuiSandbox::ImGuiSandbox(duin::GameStateMachine &sm) : duin::GameState(sm)
{
}

ImGuiSandbox::~ImGuiSandbox()
{
}

void ImGuiSandbox::Enter()
{
    renderTarget = duin::RenderTexture(duin::GetWindowWidth(), duin::GetWindowHeight(), 0);

    camera.SetAxis(
        { 5.0f, 5.0f, 5.0f },   // position
        { 0.0f, 0.0f, 0.0f },   // target
        { 0.0f, 1.0f, 0.0f }    // up
    );
    camera.fovy = 60.0f;
    duin::SetActiveCamera(&camera);
}

void ImGuiSandbox::OnEvent(duin::Event e)
{
}

void ImGuiSandbox::Update(double delta)
{
}

void ImGuiSandbox::PhysicsUpdate(double delta)
{
}

void ImGuiSandbox::Draw()
{
    duin::BeginTextureMode(renderTarget);
    duin::ClearBackground(duin::Color(80, 80, 80));
    duin::DrawGrid(10.0f);
    duin::EndTextureMode();
}

void ImGuiSandbox::DrawUI()
{
    Test_ImGuizmo();
}

void ImGuiSandbox::Exit()
{
    duin::SetActiveCamera(nullptr);
}

void ImGuiSandbox::SetPause()
{
}

void ImGuiSandbox::SetUnpause()
{
}

void ImGuiSandbox::Tests()
{
    Test_ImGuizmo();
}

void ImGuiSandbox::Test_ImGuizmo()
{
    ImGui::Begin("ImGuizmo Sandbox");

    // --- Operation selector ---
    if (ImGui::RadioButton("Translate", gizmoOperation == ImGuizmo::TRANSLATE))
        gizmoOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", gizmoOperation == ImGuizmo::ROTATE))
        gizmoOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", gizmoOperation == ImGuizmo::SCALE))
        gizmoOperation = ImGuizmo::SCALE;

    // --- Mode selector (not applicable to scale) ---
    if (gizmoOperation != ImGuizmo::SCALE)
    {
        ImGui::SameLine();
        ImGui::Separator();
        ImGui::SameLine();
        if (ImGui::RadioButton("Local", gizmoMode == ImGuizmo::LOCAL))
            gizmoMode = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", gizmoMode == ImGuizmo::WORLD))
            gizmoMode = ImGuizmo::WORLD;
    }

    // --- Snap toggle ---
    ImGui::Checkbox("Snap", &useSnap);
    if (useSnap)
    {
        ImGui::SameLine();
        if (gizmoOperation == ImGuizmo::TRANSLATE)
            ImGui::InputFloat3("##snap", snapTranslate);
        else if (gizmoOperation == ImGuizmo::ROTATE)
            ImGui::InputFloat("##snap", &snapRotate);
        else
            ImGui::InputFloat("##snap", &snapScale);
    }

    // --- Matrix decompose display ---
    float matTr[3], matRot[3], matSc[3];
    ImGuizmo::DecomposeMatrixToComponents(objectMatrix, matTr, matRot, matSc);
    ImGui::InputFloat3("Translation", matTr);
    ImGui::InputFloat3("Rotation",    matRot);
    ImGui::InputFloat3("Scale",       matSc);
    ImGuizmo::RecomposeMatrixFromComponents(matTr, matRot, matSc, objectMatrix);

    // --- Viewport image ---
    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    if (viewportSize.x < 1) viewportSize.x = 1;
    if (viewportSize.y < 1) viewportSize.y = 1;

    ImVec2 viewportPos = ImGui::GetCursorScreenPos();
    duin::DrawIMGUITexture(renderTarget, { viewportSize.x, viewportSize.y });

    // --- ImGuizmo overlay ---
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::BeginFrame();
    ImGuizmo::SetRect(viewportPos.x, viewportPos.y, viewportSize.x, viewportSize.y);
    ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());

    duin::Matrix viewMtx = duin::GetCameraViewMatrix(&camera);
    duin::Matrix projMtx = duin::GetCameraProjectionMatrix(&camera);

    // duin::Matrix is column-major (OpenGL style); ImGuizmo expects the same layout.
    float view[16] = {
        viewMtx.m0,  viewMtx.m1,  viewMtx.m2,  viewMtx.m3,
        viewMtx.m4,  viewMtx.m5,  viewMtx.m6,  viewMtx.m7,
        viewMtx.m8,  viewMtx.m9,  viewMtx.m10, viewMtx.m11,
        viewMtx.m12, viewMtx.m13, viewMtx.m14, viewMtx.m15,
    };
    float proj[16] = {
        projMtx.m0,  projMtx.m1,  projMtx.m2,  projMtx.m3,
        projMtx.m4,  projMtx.m5,  projMtx.m6,  projMtx.m7,
        projMtx.m8,  projMtx.m9,  projMtx.m10, projMtx.m11,
        projMtx.m12, projMtx.m13, projMtx.m14, projMtx.m15,
    };

    const float *snap = nullptr;
    if (useSnap)
    {
        if (gizmoOperation == ImGuizmo::TRANSLATE)      snap = snapTranslate;
        else if (gizmoOperation == ImGuizmo::ROTATE)    snap = &snapRotate;
        else                                            snap = &snapScale;
    }

    ImGuizmo::Manipulate(view, proj, gizmoOperation, gizmoMode, objectMatrix, nullptr, snap);

    ImGui::End();
}
