#include "ViewportPanel.h"
#include "PanelManager.h"
#include "../EditorState_GameEditor.h"

#include <Duin/Core/Application.h>

float windowX = 0.0f;
float windowY = 0.0f;

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
    // if (target.IsValid()) {
    //     target.Destroy();
    // }
}

void ViewportPanel::Init()
{
    ConnectToGameEditorSignals();
    CreateRenderTexture();

    duin::ClearBackground(duin::PINK);
}

void ViewportPanel::Deserialise(duin::DataValue value)
{
    DN_INFO("DOTO");
}

void ViewportPanel::DrawContent()
{
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

void ViewportPanel::OnEvent(duin::Event e)
{}

void ViewportPanel::SimulatePhysics(double delta)
{}

void ViewportPanel::Render()
{
    ::ImVec2 avail(windowX, windowY);
    int newW = int(avail.x);
    int newH = int(avail.y);

    duin::BeginTextureMode(target);
    duin::ClearBackground(duin::PINK);

    duin::DrawBox();

    duin::EndTextureMode();
}

void ViewportPanel::RenderUI()
{
}

void ViewportPanel::ConnectToGameEditorSignals()
{
    if (isGameEditorValid) {
        onEventSignal.Connect([this](duin::Event e) { OnEvent(e); });
        onPhysicsUpdateSignal.Connect([this](double delta) { SimulatePhysics(delta); });
        onDrawSignal.Connect([this]() { Render(); });
        onDrawUISignal.Connect([this]() { RenderUI(); });
    }
}

void ViewportPanel::CreateRenderTexture()
{
    target = duin::RenderTexture(duin::GetWindowWidth(), duin::GetWindowHeight(), 0);
}
