#include "dnpch.h"
#include "Application.h"

#include "Duin/Core/Events/EventHandler.h"
#include "Duin/Core/Events/Keys.h"

#include "Duin/Render/Renderer.h"

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#ifndef BX_CONFIG_DEBUG
#define BX_CONFIG_DEBUG 0
#endif
#include <bx/bx.h>
#include <bx/math.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include "external/backends/imgui_impl_bgfx.h"
#include "external/backends/imgui_impl_sdl3.h"


#include "Duin/Render/RenderShape.h"
#include "Duin/Render/Shader.h"

#include <filesystem>


static int debugIsGamePaused_ = 0;

static bool gameShouldQuit = false;

static size_t physicsFrameCount = 0;
static size_t renderFrameCount = 0;

static double physicsFrameTime = 0.0;
static double renderFrameTime = 0.0;

static int TARGET_RENDER_FRAMERATE = 60;
static int TARGET_PHYSICS_FRAMERATE = 60;

static int WINDOW_WIDTH = 1280;
static int WINDOW_HEIGHT = 720;

// Color backgroundColor = WHITE;
// ::Camera3D activeCamera3D;

static SDL_Window *sdlWindow = NULL;
static SDL_Surface* sdlSurface = NULL;
static SDL_Renderer *sdlRenderer = NULL;

namespace duin {

    static std::vector<std::function<void(double)>> postUpdateCallbacks;
    static std::vector<std::function<void(double)>> postPhysicsUpdateCallbacks;
    static std::vector<std::function<void(void)>> postDrawCallbacks;
    static std::vector<std::function<void(void)>> postDrawUICallbacks;
    static std::vector<std::function<void(void)>> preFrameCallbacks;
    static std::vector<std::function<void(void)>> postFrameCallbacks;
    static std::vector<std::function<void(void)>> postDebugCallbacks;

    std::string GetRootDirectory()
    {
        std::filesystem::path currentPath = std::filesystem::current_path();
        return currentPath.string();
    }

    void DebugPauseGame()
    {
        debugIsGamePaused_ = 1;
    }

    void DebugResumeGame()
    {
        debugIsGamePaused_ = 0;
    }

    int DebugIsGamePaused()
    {
        return debugIsGamePaused_;
    }

    // void SetActiveCamera3D(::Camera3D camera3d)
    // {
    //     activeCamera3D = camera3d;
    // }

    // void SetBackgroundColor(::Color color)
    // {
    //     backgroundColor = color;
    // }

    void SetFramerate(int framerate)
    {
        TARGET_RENDER_FRAMERATE = framerate;
    }

    void DrawPhysicsFPS(float x, float y) {
        static constexpr size_t bufferSize = 60; // Buffer size (approx. 1 second at 60 FPS)
        static std::array<float, bufferSize> physicsFPSBuffer = {};
        static size_t bufferIndex = 0;
        static float physicsFPSAverage = 0.0f;
        static float totalFPS = 0.0f; // Running total for quick average calculation

        // Get current frame's physics FPS
        float currentPhysicsFPS = GetPhysicsFPS();

        // Update circular buffer
        totalFPS -= physicsFPSBuffer[bufferIndex];     // Remove old value from total
        physicsFPSBuffer[bufferIndex] = currentPhysicsFPS; // Add new value to buffer
        totalFPS += currentPhysicsFPS;                // Add new value to total
        bufferIndex = (bufferIndex + 1) % bufferSize; // Move to next index (circular)

        // Calculate average FPS
        physicsFPSAverage = totalFPS / bufferSize;

        // Format and draw the average
        char buffer[16];
        std::snprintf(buffer, sizeof(buffer), "P: %.2f", physicsFPSAverage);
        // ::DrawText(buffer, x, y, 30, ::GREEN);
    }

    void DrawRenderFPS(float x, float y) {
        static constexpr size_t bufferSize = 60; // Buffer size (approx. 1 second at 60 FPS)
        static std::array<float, bufferSize> renderFPSBuffer = {};
        static size_t bufferIndex = 0;
        static float renderFPSAverage = 0.0f;
        static float totalFPS = 0.0f; // Running total for quick average calculation

        // Get current frame's render FPS
        float currentRenderFPS = GetRenderFPS();

        // Update circular buffer
        totalFPS -= renderFPSBuffer[bufferIndex];     // Remove old value from total
        renderFPSBuffer[bufferIndex] = currentRenderFPS; // Add new value to buffer
        totalFPS += currentRenderFPS;                // Add new value to total
        bufferIndex = (bufferIndex + 1) % bufferSize; // Move to next index (circular)

        // Calculate average FPS
        renderFPSAverage = totalFPS / bufferSize;

        // Format and draw the average
        char buffer[16];
        std::snprintf(buffer, sizeof(buffer), "R: %.2f", renderFPSAverage);
        // ::DrawText(buffer, x, y, 30, ::GREEN);
    }

    float GetPhysicsFPS()
    {
        float fps = 1.0f / (float)physicsFrameTime;
        return fps;
    }

    float GetPhysicsFrameTime()
    {
        float frametime = (float)physicsFrameTime;
        return frametime;
    }

    float GetRenderFPS()
    {
        float fps = 0.0f;
        if (renderFrameTime < 0.00001) {
            fps = 9999.9f;
        } else {
            fps = 1.0f / (float)renderFrameTime;
        }
        return fps;
    }

    float GetRenderFrameTime()
    {
        float frametime = (float)renderFrameTime;
        return frametime;
    }

    size_t GetPhysicsFrameCount()
    {
        return physicsFrameCount;
    }

    size_t GetRenderFrameCount()
    {
        return renderFrameCount;
    }

    double GetTicks()
    {
        return ::SDL_GetTicks() / 1000.0;
    }

    double GetTicksMilli()
    {
        return (double)::SDL_GetTicks();
    }
    double GetTicksNano()
    {
        return (double)::SDL_GetTicksNS();
    }

    void DelayProcess(float seconds)
    {
        ::SDL_Delay((uint32_t)seconds * 1000);
    }
    void DelayProcessMilli(float milliseconds)
    {
        ::SDL_Delay((uint32_t)milliseconds);
    }

    int GetWindowWidth()
    {
        return WINDOW_WIDTH;
    }

    int GetWindowHeight()
    {
        return WINDOW_HEIGHT;
    }

    void QueuePostUpdateCallback(std::function<void(double)> f)
    {
        postUpdateCallbacks.push_back(f);
    }

    void QueuePostPhysicsUpdateCallback(std::function<void(double)> f)
    {
        postPhysicsUpdateCallbacks.push_back(f);
    }

    void QueuePostDrawCallback(std::function<void()> f)
    {
        postDrawCallbacks.push_back(f);
    }

    void QueuePostDrawUICallback(std::function<void()> f)
    {
        postDrawUICallbacks.push_back(f);
    }

    void QueuePreFrameCallback(std::function<void()> f)
    {
        preFrameCallbacks.push_back(f);
    }

    void QueuePostFrameCallback(std::function<void()> f)
    {
        postFrameCallbacks.push_back(f);
    }

    void QueuePostDebugCallback(std::function<void()> f)
    {
        postDebugCallbacks.push_back(f);
    }


    Application::Application()
    {
    }

    Application::~Application()
    {
    }

    void Application::SetWindowStartupSize(int width, int height)
    {
        WINDOW_WIDTH = width;
        WINDOW_HEIGHT = height;
    }

    // void Application::SetBackgroundColor(::Color color)
    // {
    //     backgroundColor = color;
    // }

    void Application::SetWindowName(const char* string)
    {
        windowName = std::string(string);
    }





    void Application::Run()
    {    
        const double MAX_ACCUMULATOR = 0.25;
        double physicsCurrentTime = GetTicks();
        double physicsPreviousTime = 0.0;
        double physicsDeltaTime = 0.0;
        double physicsAccumTime = 0.0;

        double frameEndTime = 0.0;
        double frameStartTime = 0.0;
        double deltaDrawTime = 0.0;
        double waitTime = 0.0;
        double deltaTime = 0.0;


        EngineInitialize();
        Initialize();

        DN_CORE_INFO("Set render FPS {}", TARGET_RENDER_FRAMERATE);

        if(!::SDL_Init(SDL_INIT_VIDEO)) {
            ::SDL_Log("SDL could not initialize! SDL error: %s\n", ::SDL_GetError());
        } else {
            sdlWindow = ::SDL_CreateWindow(windowName.c_str(), WINDOW_WIDTH, WINDOW_HEIGHT, 0);
            if(sdlWindow == nullptr) {
                ::SDL_Log("Window could not be created! SDL error: %s\n", ::SDL_GetError());
            } else {
                sdlSurface = ::SDL_GetWindowSurface(sdlWindow);
            }
        }
        ::HWND hwnd = (::HWND)::SDL_GetPointerProperty(::SDL_GetWindowProperties(sdlWindow), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
        if (!hwnd) {
            DN_CORE_FATAL("SDL3 window handle not found!");
        }
        bgfx::renderFrame();
        bgfx::Init bgfxInit;
        bgfxInit.type = bgfx::RendererType::Count; // Automatically choose a renderer.
        bgfxInit.resolution.width = WINDOW_WIDTH;
        bgfxInit.resolution.height = WINDOW_HEIGHT;
        bgfxInit.resolution.reset = BGFX_RESET_VSYNC;
        bgfxInit.platformData.nwh = hwnd;
        const bgfx::ViewId MAIN_DISPLAY = 0;
        bgfx::init(bgfxInit);
        bgfx::setViewClear(MAIN_DISPLAY, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
        bgfx::setViewRect(MAIN_DISPLAY, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        ImGui::CreateContext();
        ::ImGui_Implbgfx_Init(255);
        ::ImGui_ImplSDL3_InitForD3D(sdlWindow);

        EngineReady();
        Ready();

        /* Testing drawing cube */
        // BoxRenderShape cube;
        // bgfx::VertexLayout pcvDecl;
        // pcvDecl.begin()
        //     .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        //     .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        // .end();
        //
        // bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(bgfx::makeRef(cube.vertices, sizeof(cube.vertices)), pcvDecl);
        // bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(bgfx::makeRef(cube.triList, sizeof(cube.triList)));
        // bgfx::ShaderHandle vsh = LoadShader("C:/Projects/CPP_Projects/Duin/Duin/src/Duin/Resources/shaders/dx11/vs_cubes.bin");
        // bgfx::ShaderHandle fsh = LoadShader("C:/Projects/CPP_Projects/Duin/Duin/src/Duin/Resources/shaders/dx11/fs_cubes.bin");
        // bgfx::ProgramHandle program = bgfx::createProgram(vsh, fsh, true);
        // vbh = bgfx::createVertexBuffer(bgfx::makeRef(cube.vertices, sizeof(cube.vertices)), pcvDecl);
        // ibh = bgfx::createIndexBuffer(bgfx::makeRef(cube.triList, sizeof(cube.triList)));
        // bgfx::ProgramHandle program2 = bgfx::createProgram(vsh, fsh, true);
        /* Testing drawing cube */

        while(!gameShouldQuit) {

            #ifdef DN_DEBUG
                if (!debugIsGamePaused_) {
            #endif /* DN_DEBUG */


            frameStartTime = GetTicks();

            EnginePreFrame();

            EventHandler::Get().PollEvents();

            EngineUpdate(deltaTime);
            Update(deltaTime);
            EnginePostUpdate(deltaTime);

            physicsCurrentTime = GetTicks();
            physicsDeltaTime = physicsCurrentTime - physicsPreviousTime;
            physicsAccumTime += physicsDeltaTime;
            double physicsTimeStep = (1.0 / (double)TARGET_PHYSICS_FRAMERATE);
            while (physicsAccumTime >= physicsTimeStep) {
                physicsPreviousTime = physicsCurrentTime;
                physicsAccumTime -= physicsTimeStep;

                physicsFrameTime = physicsTimeStep;
                ++physicsFrameCount;

                EnginePhysicsUpdate(physicsDeltaTime);
                PhysicsUpdate(physicsDeltaTime); 
                EnginePostPhysicsUpdate(physicsDeltaTime);
            } // End of Physics


            Renderer::Get().EmptyStack();
            ++renderFrameCount;
            bgfx::touch(MAIN_DISPLAY);
            ::ImGui_Implbgfx_NewFrame();
            ::ImGui_ImplSDL3_NewFrame();
            ImGui::NewFrame();


        /* Testing drawing cube */

            // static int counter = 0;
            // counter++;
            // const bx::Vec3 at = {0.0f, 0.0f,  0.0f};
            // const bx::Vec3 eye = {0.0f, 0.0f, -5.0f};
            // float view[16];
            // bx::mtxLookAt(view, eye, at);
            // float proj[16];
            // bx::mtxProj(proj, 60.0f, float(WINDOW_WIDTH) / float(WINDOW_HEIGHT), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
            // bgfx::setViewTransform(0, view, proj);
            //
            // float mtx[16];
            // bx::mtxRotateXY(mtx, counter * 0.01f, counter * 0.01f);
            // bgfx::setTransform(mtx);        
            //
            // bgfx::setVertexBuffer(0, vbh);
            // bgfx::setIndexBuffer(ibh);
            //
            // bgfx::submit(0, program);
            //
            // float mtx2[16];
            // bx::mtxRotateXY(mtx, counter * 0.02f, counter * 0.02f);
            // bgfx::setTransform(mtx);        
            //
            // bgfx::setVertexBuffer(0, vbh);
            // bgfx::setIndexBuffer(ibh);
            //
            // bgfx::submit(0, program);
            // bgfx::submit(0, program2);
        /* Testing drawing cube */


            EngineDraw();
            Draw();
            EnginePostDraw();

            EngineDrawUI();
            DrawUI();
            EnginePostDrawUI();

            Renderer::Get().RenderPipeline();

            ImGui::Render();
            ::ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());
            bgfx::frame();

            EnginePostFrame();

            frameEndTime = GetTicks();
            deltaDrawTime = frameEndTime - frameStartTime;

            if (TARGET_RENDER_FRAMERATE > 0) {
                double targetFrameTime = 1.0 / (double)TARGET_RENDER_FRAMERATE;
                waitTime = targetFrameTime - deltaDrawTime;

                // printf("Target FT:\t %.6f\n", targetFrameTime);
                // printf("deltaDrawTime:\t %.6f\n", deltaDrawTime);
                // printf("waittime:\t %.6f\n", waitTime);

                if (waitTime > 0.0) {
                    DelayProcess((float)waitTime);
                }
            } 

            deltaTime = GetTicks() - frameStartTime; 
            renderFrameTime = deltaTime;


            #ifdef DN_DEBUG
                } 
                EngineDebug();
                Debug();
                EnginePostDebug();
            #endif /* DN_DEBUG */

        } // End of Application While

        EngineExit();
        Exit();

        ::ImGui_ImplSDL3_Shutdown();
        ::ImGui_Implbgfx_Shutdown();

        ImGui::DestroyContext();
        bgfx::shutdown();

        ::SDL_DestroySurface(sdlSurface);
        sdlSurface = nullptr;
        ::SDL_DestroyWindow(sdlWindow);
        sdlWindow = nullptr;
        ::SDL_Quit();
    }





    void Application::EngineInitialize()
    {
        EventHandler::Get().RegisterInputEventListener([this](InputEvent e){ EngineHandleInputs(e); });
        EventHandler::Get().RegisterInputEventListener([this](InputEvent e){ HandleInputs(e); });
    }

    void Application::Initialize()
    {
    }

    void Application::EngineReady()
    {
    }

    void Application::Ready()
    {
    }

    void Application::EnginePreFrame()
    {
        for (auto& callback : preFrameCallbacks) {
            callback();
        }
    }

    void Application::EngineHandleInputs(InputEvent e)
    {
        if (e.IsKeyDown(DN_KEY_ESCAPE)) {
            DN_CORE_INFO("Quiting... {}", e.sdlEvent.key.key);
            gameShouldQuit = true;
        }
    }

    void Application::HandleInputs(InputEvent e)
    {
    }

    void Application::EngineUpdate(double delta)
    {
    }

    void Application::Update(double delta)
    {
    }

    void Application::EnginePostUpdate(double delta)
    {
        for (auto& callback : postUpdateCallbacks) {
            callback(delta);
        }
    }

    void Application::EnginePhysicsUpdate(double delta)
    {
    }

    void Application::PhysicsUpdate(double delta)
    {
    }

    void Application::EnginePostPhysicsUpdate(double delta)
    {
        for (auto& callback : postPhysicsUpdateCallbacks) {
            callback(delta);
        }
    }

    void Application::EngineDraw()
    {
    }

    void Application::Draw()
    {
    }

    void Application::EnginePostDraw()
    {
        for (auto& callback : postDrawCallbacks) {
            callback();
        }
    }

    void Application::EngineDrawUI()
    {
    }

    void Application::DrawUI()
    {
    }

    void Application::EnginePostDrawUI()
    {
        for (auto& callback : postDrawUICallbacks) {
            callback();
        }
    }

    void Application::EngineDebug()
    {
#ifdef DN_DEBUG
        // if (IsKeyPressed(KEY_GRAVE)) {
        //     if (DebugIsGamePaused()) {
        //         DebugResumeGame();
        //     } else {
        //         DebugPauseGame();
        //     }
        // }
#endif /* DN_DEBUG */
    }

    void Application::Debug()
    {
    }

    void Application::EnginePostDebug()
    {
        for (auto& callback : postDebugCallbacks) {
            callback();
        }
    }

    void Application::EnginePostFrame()
    {
        for (auto& callback : postFrameCallbacks) {
            callback();
        }
    }

    void Application::EngineExit()
    {
    }

    void Application::Exit()
    {
    }

}
