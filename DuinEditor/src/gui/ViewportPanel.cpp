#include "ViewportPanel.h"
#include "PanelManager.h"
#include "../EditorState_GameEditor.h"

#include <Duin/Core/Application.h>
#include <Duin/Core/Events/EventsModule.h>



ViewportPanel::ViewportPanel(const std::string& name, PanelManager *panelManager)
    : Panel(name, panelManager)
{
    type = Panel::VIEWPORT;
    Init();
}

ViewportPanel::ViewportPanel(duin::DataValue value)
    : Panel(value)
{
    Deserialise(value);
    Init();
}

ViewportPanel::~ViewportPanel()
{
    if (target.IsValid()) {
        target.Destroy();
    }
    DisconnectFromGameEditorSignals();
}

void ViewportPanel::Init()
{
    ConnectToGameEditorSignals();
    CreateRenderTexture();

    duin::ClearBackground(duin::PINK);

    mainCamera.Enable(true);
    mainCamera.UpdateTarget({ 0.0f, 0.0f, 0.0f });
}

void ViewportPanel::Deserialise(duin::DataValue value)
{
    DN_INFO("DOTO");
}

void ViewportPanel::DrawContent()
{
    if (!isTabOpen) return;

    // ImGui::GetCurrentWindow()->TitleBarHeight()
    ImGui::Text("This is my custom window content!");

    ::ImVec2 avail = ImGui::GetWindowSize();
    windowX = avail.x;
    windowY = avail.y;

    float dispW, dispH = 0.0f;
    float srcAR = (float)target.width/(float)target.height;
    float winAR = windowX / windowY;
    if (winAR <= srcAR) {
        dispH = avail.y;
        dispW = dispH * srcAR;
    }
    else {
        dispW = avail.x;
        dispH = dispW / srcAR;
    }

    float centerX = (ImGui::GetWindowSize().x - dispW)*0.5f;
    float centerY = (ImGui::GetWindowSize().y - barHeight - dispH)*0.5f + barHeight;

    ::ImVec2 center(centerX, centerY);
    ImGui::SetCursorPos(center);

    duin::DrawIMGUITexture(target, { dispW, dispH });
}

void ViewportPanel::OnPanelEvent(PanelEvent e)
{
    if (e.isTabFocussed || e.isFocussed || e.isHovered) {
        mainCamera.Enable(true);
    }
}

void ViewportPanel::OnEvent(duin::Event e)
{}

void ViewportPanel::SimulatePhysics(double delta)
{
    if (!isFocussed) return;

    MoveMainCamera();
}

void ViewportPanel::Render()
{
    if (!isTabOpen) return;
    DN_INFO("Drawing panel {}", uniqueWindowName);

    ::ImVec2 avail(windowX, windowY);
    int newW = int(avail.x);
    int newH = int(avail.y);


    duin::BeginTextureMode(target);
    duin::ClearBackground(duin::GRAY);

    duin::DrawGrid(100.0f);

    duin::EndTextureMode();
}

void ViewportPanel::RenderUI()
{
}

void ViewportPanel::ConnectToGameEditorSignals()
{
    if (isGameEditorValid) {
        eventSignalTag = onEventSignal.Connect([this](duin::Event e) { OnEvent(e); });
        physicsSignalTag = onPhysicsUpdateSignal.Connect([this](double delta) { SimulatePhysics(delta); });
        drawSignalTag = onDrawSignal.Connect([this]() { Render(); });
        drawuiSignalTag = onDrawUISignal.Connect([this]() { RenderUI(); });
    }
}

void ViewportPanel::DisconnectFromGameEditorSignals()
{
    onEventSignal.Disconnect(eventSignalTag);
    onPhysicsUpdateSignal.Disconnect(physicsSignalTag);
    onDrawSignal.Disconnect(drawSignalTag);
    onDrawUISignal.Disconnect(drawuiSignalTag);
}

void ViewportPanel::CreateRenderTexture()
{
    target = duin::RenderTexture(duin::GetWindowWidth(), duin::GetWindowHeight(), 0);
}

void ViewportPanel::MoveMainCamera()
{
    const float sensitivityX = 0.10f;
    const float sensitivityY = 0.10f;

    // if (duin::Input::IsMouseButtonDown(DN_BUTTON_MIDDLE)) {
    if (duin::Input::IsMouseButtonDown(DN_BUTTON_RIGHT)) {
        duin::Input::CaptureMouse(true);
        // float mouseXDelta = duin::Input::GetMouseDelta().x;
        // float angleXDelta = mouseXDelta * sensitivityX;
        // mainCamera.Yaw(angleXDelta, true);

        float mouseYDelta = duin::Input::GetMouseDelta().y;
        float angleYDelta = mouseYDelta * sensitivityY;
        DN_INFO("PitchAngle: {}", angleYDelta);
        mainCamera.Pitch(-angleYDelta, true);
    } else {
        duin::Input::CaptureMouse(false);
    }
}
