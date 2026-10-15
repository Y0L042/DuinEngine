#include "MultiWindowSandbox.h"
#include <Duin/Core/Application.h>
#include <Duin/Core/Debug/DNLog.h>
#include <Duin/Core/Window/WindowManager.h>
#include <Duin/Render/Renderer.h>
#include <cstdlib>
#include <format>

MultiWindowSandbox::MultiWindowSandbox(duin::GameStateMachine &sm) : duin::GameState(sm)
{
}

MultiWindowSandbox::~MultiWindowSandbox()
{
}

void MultiWindowSandbox::Enter()
{
    Tests();
}

void MultiWindowSandbox::OnEvent(duin::Event e)
{
}

void MultiWindowSandbox::Update(double delta)
{
}

void MultiWindowSandbox::PhysicsUpdate(double delta)
{
}

void MultiWindowSandbox::Draw()
{
}

void MultiWindowSandbox::DrawUI()
{
    if (ImGui::Button("Create Window"))
    {
        OpenWindow(std::format("Sandbox #{:02}", windows.size() + 1));
    }
}

void MultiWindowSandbox::Exit()
{
    connections.clear();
    windows.clear();
}

void MultiWindowSandbox::SetPause()
{
}

void MultiWindowSandbox::SetUnpause()
{
}

void MultiWindowSandbox::Tests()
{
    DN_INFO("Hello!");
    OpenWindow("Sandbox #01");
}

void MultiWindowSandbox::OpenWindow(const std::string &title)
{
    std::shared_ptr<duin::WindowCtx> win = duin::CreateNewWindow(title, 640, 480, SDL_WINDOW_RESIZABLE);
    if (win == nullptr)
    {
        return;
    }

    std::shared_ptr<float> boxHeight = std::make_shared<float>(0.5f);
    std::weak_ptr<duin::WindowCtx> weakWin = win;

    connections.push_back(win->OnDraw.ConnectScoped([boxHeight]() {
        duin::DrawDebugGrid(10.0f);
        duin::DrawBox(duin::Vector3(0.0f, *boxHeight, 0.0f));
    }));
    connections.push_back(win->OnDrawUI.ConnectScoped([title, boxHeight, weakWin]() {
        ImGui::Begin(title.c_str());
        ImGui::Text("Own ImGui context, own swap chain.");
        ImGui::SliderFloat("Box height", boxHeight.get(), -2.0f, 2.0f);
        if (ImGui::Button("Random clear color"))
        {
            if (std::shared_ptr<duin::WindowCtx> w = weakWin.lock())
            {
                w->SetClearColor(((uint32_t)std::rand() << 8) | 0xff);
            }
        }
        ImGui::End();
    }));
    windows.push_back(win);
}
