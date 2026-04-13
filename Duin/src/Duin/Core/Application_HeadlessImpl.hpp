/* DN_HEADLESS */

void duin::Application::Run()
{
    double physicsCurrentTime = duin::GetTicks();
    double physicsPreviousTime = 0.0;
    double physicsAccumTime = 0.0;
    double deltaTime = 0.0;

    EngineInitialize();
    Initialize();

    DN_CORE_INFO("Set render FPS {}", TARGET_RENDER_FRAMERATE);

    InitSDL();

    std::function<HWND(void)> renderThreadCapture = [&]() -> HWND {
        if (headlessMode)
            return (::HWND)nullptr;

        return (::HWND)::SDL_GetPointerProperty(
            ::SDL_GetWindowProperties(sdlWindow),
            SDL_PROP_WINDOW_WIN32_HWND_POINTER,
            NULL
        );
    };

    RHIStart(
        RHI_VIEW_3D,
        headlessMode ? 0 : WINDOW_WIDTH,
        headlessMode ? 0 : WINDOW_HEIGHT,
        renderThreadCapture,
        headlessMode
    );
    InitImGui();

    if (!headlessMode)
    {
        duin::InitRenderer();
        duin::SetRenderContextAvailable(true);
    }

    EngineReady();
    Ready();
    EnginePostReady();

    while (ProcessFrame(deltaTime, physicsCurrentTime, physicsPreviousTime, physicsAccumTime))
    {
    }

    EngineExit();
    Exit();

    DN_CORE_INFO("Shutting down Rendering dependencies...");

    if (!headlessMode)
        duin::SetRenderContextAvailable(false);

    ShutdownImGui();
    ShutdownRHI();
    ShutdownSDL();
}

void duin::Application::InitSDL()
{
    if (headlessMode)
    {
        if (!::SDL_Init(SDL_INIT_EVENTS))
            ::SDL_Log("SDL (events) could not initialize! SDL error: %s\n", ::SDL_GetError());
        return;
    }

    if (!::SDL_Init(SDL_INIT_VIDEO))
    {
        ::SDL_Log("SDL could not initialize! SDL error: %s\n", ::SDL_GetError());
    }
    else
    {
        sdlWindow = ::SDL_CreateWindow(windowName.c_str(), WINDOW_WIDTH, WINDOW_HEIGHT, sdlWindowFlags);
        if (sdlWindow == nullptr)
        {
            ::SDL_Log("Window could not be created! SDL error: %s\n", ::SDL_GetError());
        }
        else
        {
            sdlSurface = ::SDL_GetWindowSurface(sdlWindow);
        }
    }
}

void duin::Application::InitImGui()
{
    if (headlessMode)
        return;

    ImGui::CreateContext();
    RHIImGuiInit(RHI_VIEW_IMGUI);
    ::ImGui_ImplSDL3_InitForD3D(sdlWindow);
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

void duin::Application::ShutdownImGui()
{
    if (headlessMode)
        return;

    ::ImGui_ImplSDL3_Shutdown();
    RHIImGuiShutdown();
    ImGui::DestroyContext();
}

void duin::Application::ShutdownRHI()
{
    RHIClose();
    DN_CORE_INFO("RHI closed...");
}

void duin::Application::ShutdownSDL()
{
    if (!headlessMode)
    {
        ::SDL_DestroySurface(sdlSurface);
        sdlSurface = nullptr;
        ::SDL_DestroyWindow(sdlWindow);
        sdlWindow = nullptr;
    }

    DN_CORE_INFO("Quitting...");
    ::SDL_Quit();
}

bool duin::Application::PushSDLEvent(::SDL_Event *e)
{
    return ::SDL_PushEvent(e);
}

void duin::Application::ProcessEvents()
{
    duin::Input::CacheCurrentKeyState();
    duin::Input::CacheCurrentMouseKeyState();
    duin::Input::ClearCurrentModKeyState();
    duin::Input::ResetMouseFrameState();

    ::SDL_Event e;
    ::SDL_zero(e);
    while (::SDL_PollEvent(&e))
    {
        duin::Input::ProcessSDLMouseEvent(e);
        duin::Input::ProcessSDLKeyboardEvent(e);
        eventHandler.PollEvent(e);

        if (!headlessMode)
            ::ImGui_ImplSDL3_ProcessEvent(&e);
    }
    duin::Input::UpdateMouseFrameDelta();
    gameShouldQuit = eventHandler.IsCloseRequested();
}

void duin::Application::RunRender()
{
    // TODO change this to run headless with fake rendering
    if (headlessMode)
        return;

    // Update render rect on window resizing
    int displayWidth, displayHeight;
    ::SDL_GetWindowSize(sdlWindow, &displayWidth, &displayHeight);

    WINDOW_WIDTH = displayWidth;
    WINDOW_HEIGHT = displayHeight;

    RHIReset((uint32_t)displayWidth, (uint32_t)displayHeight, RHI_RESET_VSYNC);
    RHISetViewRect(RHI_VIEW_3D, 0, 0, displayWidth, displayHeight);
    RHITouch(RHI_VIEW_3D);

    ++renderFrameCount;
    RHIImGuiNewFrame();
    ::ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    if (ALLOW_DOCKING_IN_MAIN)
    {
        const ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
        ImGui::DockSpaceOverViewport(0, nullptr, dockspace_flags);
    }

    duin::BeginDraw3D(*duin::GetActiveCamera());

    EngineDraw();
    Draw();
    EnginePostDraw();

    EngineDrawUI();
    DrawUI();
    EnginePostDrawUI();

    duin::EndDraw3D();

    ImGui::Render();
    RHIImGuiRenderDrawLists(ImGui::GetDrawData());
    duin::ExecuteRenderPipeline();
}

bool duin::Application::ProcessFrame(double &deltaTime, double &physicsCurrentTime, double &physicsPreviousTime,
                                     double &physicsAccumTime)
{
#ifdef DN_DEBUG
    if (!debugIsGamePaused_)
    {
#endif /* DN_DEBUG */

        if (!headlessMode)
        {
            /* Do not run game when minimized */
            if (PAUSE_ON_MINIMIZE && SDL_GetWindowFlags(sdlWindow) && SDL_WINDOW_MINIMIZED)
            {
                duin::DelayProcessMilli(100);
                return !gameShouldQuit;
            }
        }

        double frameStartTime = duin::GetTicks();

        EnginePreFrame();

        if (!headlessMode)
        {
            /* If custom ImGui path is set, manually save to memory/disk */
            if (USE_CUSTOM_IMGUI_INI_PATH && ImGui::GetIO().WantSaveIniSettings)
            {
                ImGui::SaveIniSettingsToDisk(IMGUI_INI_PATH.c_str());
            }
        }

        ProcessEvents();

        RunUpdate(deltaTime);

        RunPhysics(physicsCurrentTime, physicsPreviousTime, physicsAccumTime);

        if (!headlessMode)
        {
            /* Skip rendering when minimized */
            SDL_Event e;
            SDL_zero(e);
            if (SDL_GetWindowFlags(sdlWindow) && e.type == SDL_WINDOW_MINIMIZED)
            {
                ImGui::Render();
                duin::DelayProcessMilli(100);
                return !gameShouldQuit;
            }

            /* Skip rendering if window size is 0 */
            int displayWidth, displayHeight;
            ::SDL_GetWindowSize(sdlWindow, &displayWidth, &displayHeight);
            if (!(displayWidth && displayHeight))
            {
                duin::DelayProcessMilli(100);
                return !gameShouldQuit;
            }
        }

        RunRender();

        EnginePostFrame();

        double frameEndTime = duin::GetTicks();
        double deltaDrawTime = frameEndTime - frameStartTime;

        if (TARGET_RENDER_FRAMERATE > 0)
        {
            double targetFrameTime = 1.0 / (double)TARGET_RENDER_FRAMERATE;
            double waitTime = targetFrameTime - deltaDrawTime;

            if (waitTime > 0.0)
            {
                duin::DelayProcess((float)waitTime);
            }
        }

        deltaTime = duin::GetTicks() - frameStartTime;
        renderFrameTime = deltaTime;

#ifdef DN_DEBUG
    }
    EngineDebug();
    Debug();
    EnginePostDebug();
#endif /* DN_DEBUG */

    return !gameShouldQuit;
}
