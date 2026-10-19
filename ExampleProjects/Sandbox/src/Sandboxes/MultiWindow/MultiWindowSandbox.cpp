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
    for (const std::shared_ptr<SandboxWindow> &sandboxWindow : windows)
    {
        if (!CanDraw(*sandboxWindow))
        {
            continue;
        }
        sandboxWindow->window->QueueDraw([sandboxWindow]() {
            duin::DrawDebugGrid(10.0f);
            duin::DrawBox(duin::Vector3(0.0f, sandboxWindow->boxHeight, 0.0f));
        });
    }
}

void MultiWindowSandbox::DrawUI()
{
    if (ImGui::Button("Create Window"))
    {
        OpenWindow(std::format("Sandbox #{:02}", windows.size() + 1));
    }

    for (const std::shared_ptr<SandboxWindow> &sandboxWindow : windows)
    {
        if (!CanDraw(*sandboxWindow))
        {
            continue;
        }
        sandboxWindow->window->QueueDrawUI([sandboxWindow]() {
            ImGui::Begin(sandboxWindow->title.c_str());
            ImGui::Text("Own ImGui context, own swap chain.");
            ImGui::SliderFloat("Box height", &sandboxWindow->boxHeight, -2.0f, 2.0f);
            if (ImGui::Button("Random clear color"))
            {
                sandboxWindow->window->SetClearColor(((uint32_t)std::rand() << 8) | 0xff);
            }
            ImGui::End();
        });
    }
}

void MultiWindowSandbox::Exit()
{
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

    std::shared_ptr<SandboxWindow> sandboxWindow = std::make_shared<SandboxWindow>();
    sandboxWindow->window = win;
    sandboxWindow->title = title;
    windows.push_back(sandboxWindow);
}

bool MultiWindowSandbox::CanDraw(const SandboxWindow &sandboxWindow)
{
    duin::WindowCtx &win = *sandboxWindow.window;
    return win.IsWindowValid() && !win.IsPendingDestroy() && !(win.GetSDLWindowFlags() & SDL_WINDOW_MINIMIZED);
}
