/* DN_HEADLESS */

static ::ImGuiContext *PRIMARY_IMGUI_CONTEXT = nullptr;
static std::array<bool, duin::RHI_MAX_WINDOWS> VIEW_SLOTS_IN_USE{};
static std::shared_ptr<duin::ScopedConnection> WINDOW_CREATED_CONNECTION;
static std::shared_ptr<duin::ScopedConnection> WINDOW_DESTROYING_CONNECTION;

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
            return (::HWND) nullptr;

        return (::HWND)windowManager.GetPrimaryWindow()->GetNativeHandle();
    };

    RHIStart(
        RHI_VIEW_3D,
        headlessMode ? 0 : WINDOW_WIDTH,
        headlessMode ? 0 : WINDOW_HEIGHT,
        renderThreadCapture,
        headlessMode);
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
        std::shared_ptr<WindowCtx> window = windowManager.CreateNewWindow(windowName, WINDOW_WIDTH, WINDOW_HEIGHT, sdlWindowFlags);
        windowManager.SetPrimaryWindow(window);
        if (window == nullptr)
        {
            ::SDL_Log("Window could not be created! SDL error: %s\n", ::SDL_GetError());
        }
    }
}

void duin::Application::InitImGui()
{
    if (headlessMode)
        return;

    PRIMARY_IMGUI_CONTEXT = ImGui::CreateContext();
    ImGui::SetCurrentContext(PRIMARY_IMGUI_CONTEXT);
    RHIImGuiInit(RHI_VIEW_IMGUI);
    ::ImGui_ImplSDL3_InitForD3D(windowManager.GetPrimarySDLWindow());
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    WINDOW_CREATED_CONNECTION = windowManager.OnWindowCreated.ConnectScoped(
        [this](std::shared_ptr<WindowCtx> windowCtx) {
            if (windowCtx != windowManager.GetPrimaryWindow())
            {
                InitWindowRenderResources(*windowCtx);
            }
        });
    WINDOW_DESTROYING_CONNECTION = windowManager.OnWindowDestroying.ConnectScoped(
        [this](std::shared_ptr<WindowCtx> windowCtx) { ShutdownWindowRenderResources(*windowCtx); });

    for (const std::shared_ptr<WindowCtx> &windowCtx : windowManager.GetSecondaryWindows())
    {
        InitWindowRenderResources(*windowCtx);
    }
}

void duin::Application::ShutdownImGui()
{
    if (headlessMode)
        return;

    windowManager.ReleasePendingWindows();
    for (const std::shared_ptr<WindowCtx> &windowCtx : windowManager.GetSecondaryWindows())
    {
        ShutdownWindowRenderResources(*windowCtx);
    }
    WINDOW_CREATED_CONNECTION.reset();
    WINDOW_DESTROYING_CONNECTION.reset();

    ImGui::SetCurrentContext(PRIMARY_IMGUI_CONTEXT);
    ::ImGui_ImplSDL3_Shutdown();
    RHIImGuiShutdown();
    ImGui::DestroyContext(PRIMARY_IMGUI_CONTEXT);
    PRIMARY_IMGUI_CONTEXT = nullptr;
}

void duin::Application::InitWindowRenderResources(WindowCtx &windowCtx)
{
    if (windowCtx.GetImGuiContext() != nullptr)
    {
        return;
    }

    int slot = -1;
    for (int i = 0; i < RHI_MAX_WINDOWS; ++i)
    {
        if (!VIEW_SLOTS_IN_USE[i])
        {
            slot = i;
            break;
        }
    }
    if (slot < 0)
    {
        DN_CORE_ERROR("No free render view slot for window '{}' (max {}).", windowCtx.GetTitle(), RHI_MAX_WINDOWS);
        return;
    }

    int width, height;
    windowCtx.GetSizeInPixels(width, height);
    RHIFrameBufferHandle frameBuffer =
        RHICreateWindowFrameBuffer(windowCtx.GetNativeHandle(), (uint16_t)width, (uint16_t)height);
    if (!frameBuffer.IsValid())
    {
        DN_CORE_ERROR("Failed to create swap chain for window '{}'.", windowCtx.GetTitle());
        return;
    }

    VIEW_SLOTS_IN_USE[slot] = true;
    windowCtx.SetViewSlot(slot);
    windowCtx.SetFrameBuffer(frameBuffer, width, height);

    ::ImGuiContext *previousContext = ImGui::GetCurrentContext();
    ImGui::SetCurrentContext(PRIMARY_IMGUI_CONTEXT);
    ImFontAtlas *sharedAtlas = ImGui::GetIO().Fonts;
    ImGuiStyle sharedStyle = ImGui::GetStyle();

    ::ImGuiContext *imguiContext = ImGui::CreateContext(sharedAtlas);
    ImGui::SetCurrentContext(imguiContext);
    ImGui::GetStyle() = sharedStyle;
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    RHIImGuiInit(windowCtx.GetUIView());
    ::ImGui_ImplSDL3_InitForD3D(windowCtx.GetSDLWindow());
    windowCtx.SetImGuiContext(imguiContext);

    ImGui::SetCurrentContext(previousContext);
}

void duin::Application::ShutdownWindowRenderResources(WindowCtx &windowCtx)
{
    if (::ImGuiContext *imguiContext = windowCtx.GetImGuiContext())
    {
        ::ImGuiContext *previousContext = ImGui::GetCurrentContext();
        ImGui::SetCurrentContext(imguiContext);
        ::ImGui_ImplSDL3_Shutdown();
        RHIImGuiShutdown();
        ImGui::DestroyContext(imguiContext);
        ImGui::SetCurrentContext(previousContext != imguiContext ? previousContext : PRIMARY_IMGUI_CONTEXT);
        windowCtx.SetImGuiContext(nullptr);
    }

    const int slot = windowCtx.GetViewSlot();
    if (slot >= 0)
    {
        RHISetViewFrameBuffer(windowCtx.GetSceneView(), RHIFrameBufferHandle{});
        RHISetViewFrameBuffer(windowCtx.GetUIView(), RHIFrameBufferHandle{});
        VIEW_SLOTS_IN_USE[slot] = false;
        windowCtx.SetViewSlot(-1);
    }

    RHIDestroyFrameBuffer(windowCtx.GetFrameBuffer());
    windowCtx.SetFrameBuffer(RHIFrameBufferHandle{}, 0, 0);
}

void duin::Application::RenderSecondaryWindow(WindowCtx &windowCtx)
{
    ::ImGuiContext *imguiContext = windowCtx.GetImGuiContext();
    if (imguiContext == nullptr || windowCtx.IsPendingDestroy())
    {
        windowCtx.ClearDrawQueue();
        windowCtx.ClearDrawUIQueue();
        return;
    }
    if (windowCtx.GetSDLWindowFlags() & SDL_WINDOW_MINIMIZED)
    {
        windowCtx.ClearDrawQueue();
        windowCtx.ClearDrawUIQueue();
        return;
    }

    int width, height;
    windowCtx.GetSizeInPixels(width, height);
    if (width <= 0 || height <= 0)
    {
        windowCtx.ClearDrawQueue();
        windowCtx.ClearDrawUIQueue();
        return;
    }

    RHIFrameBufferHandle frameBuffer = windowCtx.GetFrameBuffer();
    int frameBufferWidth, frameBufferHeight;
    windowCtx.GetFrameBufferSize(frameBufferWidth, frameBufferHeight);
    if (frameBuffer.IsValid() && (width != frameBufferWidth || height != frameBufferHeight))
    {
        RHIDestroyFrameBuffer(frameBuffer);
        windowCtx.SetFrameBuffer(RHIFrameBufferHandle{}, 0, 0);
        windowCtx.ClearDrawQueue();
        windowCtx.ClearDrawUIQueue();
        return;
    }
    if (!frameBuffer.IsValid())
    {
        frameBuffer = RHICreateWindowFrameBuffer(windowCtx.GetNativeHandle(), (uint16_t)width, (uint16_t)height);
        windowCtx.SetFrameBuffer(frameBuffer, width, height);
        if (!frameBuffer.IsValid())
        {
            windowCtx.ClearDrawQueue();
            windowCtx.ClearDrawUIQueue();
            return;
        }
    }

    const RHIViewId sceneView = windowCtx.GetSceneView();
    const RHIViewId uiView = windowCtx.GetUIView();
    RHISetViewFrameBuffer(sceneView, frameBuffer);
    RHISetViewFrameBuffer(uiView, frameBuffer);
    RHISetViewRect(sceneView, 0, 0, (uint16_t)width, (uint16_t)height);
    RHISetViewClear(sceneView, windowCtx.GetClearColor(), 1.0f, 0);
    RHITouch(sceneView);

    ImGui::SetCurrentContext(imguiContext);
    RHIImGuiNewFrame();
    ::ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    Camera *camera = windowCtx.GetCamera() != nullptr ? windowCtx.GetCamera() : duin::GetActiveCamera();
    if (camera != nullptr)
    {
        duin::BeginDraw3D(*camera, sceneView, (uint16_t)width, (uint16_t)height);
        windowCtx.ExecuteDrawQueue();
        duin::EndDraw3D();
    }

    windowCtx.ExecuteDrawUIQueue();

    ImGui::Render();
    RHIImGuiRenderDrawLists(ImGui::GetDrawData(), uiView);
    ImGui::SetCurrentContext(PRIMARY_IMGUI_CONTEXT);
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
        windowManager.DestroyAllWindows();
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
        {
            ::ImGuiContext *eventContext = PRIMARY_IMGUI_CONTEXT;
            if (std::shared_ptr<WindowCtx> windowCtx = windowManager.GetWindowCtx(::SDL_GetWindowFromEvent(&e)))
            {
                if (windowCtx->GetImGuiContext() != nullptr)
                {
                    eventContext = windowCtx->GetImGuiContext();
                }
            }
            ImGui::SetCurrentContext(eventContext);
            ::ImGui_ImplSDL3_ProcessEvent(&e);
            ImGui::SetCurrentContext(PRIMARY_IMGUI_CONTEXT);
        }
    }
    duin::Input::UpdateMouseFrameDelta();
    gameShouldQuit = gameShouldQuit || eventHandler.IsCloseRequested();
}

void duin::Application::RunRender()
{
    // TODO change this to run headless with fake rendering
    if (headlessMode)
        return;

    SDL_Window *window = windowManager.GetPrimarySDLWindow();

    // Update render rect on window resizing
    int displayWidth, displayHeight;
    ::SDL_GetWindowSize(window, &displayWidth, &displayHeight);

    WINDOW_WIDTH = displayWidth;
    WINDOW_HEIGHT = displayHeight;

    RHIReset((uint32_t)displayWidth, (uint32_t)displayHeight, RHI_RESET_VSYNC);
    RHISetViewRect(RHI_VIEW_3D, 0, 0, displayWidth, displayHeight);
    RHITouch(RHI_VIEW_3D);

    ++renderFrameCount;
    ImGui::SetCurrentContext(PRIMARY_IMGUI_CONTEXT);
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

    for (const std::shared_ptr<WindowCtx> &windowCtx : windowManager.GetSecondaryWindows())
    {
        RenderSecondaryWindow(*windowCtx);
    }
    windowManager.ReleasePendingWindows();

    duin::ExecuteRenderPipeline();
    windowManager.DestroyPendingWindows();
}

bool duin::Application::ProcessFrame(
    double &deltaTime, double &physicsCurrentTime, double &physicsPreviousTime, double &physicsAccumTime)
{
#ifdef DN_DEBUG
    if (!debugIsGamePaused_)
    {
#endif /* DN_DEBUG */

        SDL_Window *window = windowManager.GetPrimarySDLWindow();

        if (!headlessMode)
        {
            /* Do not run game when minimized */
            if (PAUSE_ON_MINIMIZE && SDL_GetWindowFlags(window) && SDL_WINDOW_MINIMIZED)
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

        // Used for testing and debugging, to force physics step.
        if (physicsAccumTime < 0.0)
        {
            PhysicsStep(physicsCurrentTime);
        }
        else
        {
            RunPhysics(physicsCurrentTime, physicsPreviousTime, physicsAccumTime);
        }

        if (!headlessMode)
        {
            /* Skip rendering when minimized */
            SDL_Event e;
            SDL_zero(e);
            if (SDL_GetWindowFlags(window) && e.type == SDL_WINDOW_MINIMIZED)
            {
                ImGui::Render();
                duin::DelayProcessMilli(100);
                return !gameShouldQuit;
            }

            /* Skip rendering if window size is 0 */
            int displayWidth, displayHeight;
            ::SDL_GetWindowSize(window, &displayWidth, &displayHeight);
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
