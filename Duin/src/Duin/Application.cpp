#include "dnpch.h"

#include "Application.h"

namespace Duin
{

    Application::Application()
    {
    }

    Application::~Application()
    {
    }

    void Application::Run()
    {
        win = std::make_unique<Window>(1280, 720);
        //Start up SDL and create window
        if (!win->Init())
        {
            DN_CORE_FATAL("Failed to initialize!\n");
        }

        //Main loop flag
        bool quit = false;

        //Event handler
        SDL_Event e;

        uint32_t renderTimeCount = SDL_GetTicks();
        uint32_t physicsTimeCount = SDL_GetTicks();
        uint32_t previousTimeCount = SDL_GetTicks();
        uint32_t lagTimeCount = 0;

        const uint32_t RENDER_TIME = (uint32_t)(1000 / TARGET_RENDER_FRAMERATE);
        const uint32_t PHYSICS_TIME = (uint32_t)(1000 / TARGET_PHYSICS_FRAMERATE);

        uint32_t nDelta = 0;
        uint32_t rDelta = 0;
        uint32_t pDelta = 0;

        EngineReady();
        Ready();

        //While application is running
        while (!quit)
        {
            //Handle events on queue
            while (SDL_PollEvent(&e) != 0)
            {
                EngineHandleEvents(e);
                HandleEvents(e);

                //User requests quit
                if (e.type == SDL_QUIT)
                {
                    quit = true;
                }
            }

            uint32_t currentTicks = SDL_GetTicks();
            nDelta = currentTicks - previousTimeCount;
            rDelta = currentTicks - renderTimeCount;
            pDelta = currentTicks - physicsTimeCount;
            previousTimeCount = currentTicks;
            lagTimeCount += nDelta;
            bool executeRenderFrame = rDelta >= RENDER_TIME;
            bool executePhysicsFrame = pDelta >= PHYSICS_TIME;
            if (executeRenderFrame) { renderTimeCount = currentTicks; }
            if (executePhysicsFrame) { physicsTimeCount = currentTicks; }

            if (rDelta >= RENDER_TIME)
            {
                EngineProcess(rDelta);
                Process(rDelta);
            }

            if (pDelta >= PHYSICS_TIME) 
            { 
                EnginePhysicsProcess(pDelta);
                PhysicsProcess(pDelta); 
            }

            //// Perhaps a better physics loop, but have to adjust pDelta first
            //while (lagTimeCount >= PHYSICS_TIME)
            //{
            //    EnginePhysicsProcess(pDelta);
            //    PhysicsProcess(pDelta);
            //    lagTimeCount -= PHYSICS_TIME;
            //}

            if (rDelta >= RENDER_TIME) 
            { 
                win->ClearRenderer();
                win->FillWindow(25, 25, 25);
                Draw(); 
                EngineDraw();
                win->Render();
            }
        }

        //Free resources and close SDL
        win->Close();
        SDL_Quit();
    }

    void Application::EngineReady()
    {
    }

    void Application::Ready()
    {
    }

    void Application::EngineHandleEvents(SDL_Event& e)
    {
    }

    void Application::HandleEvents(SDL_Event& e)
    {
    }

    void Application::EngineProcess(double rDelta)
    {
    }

    void Application::Process(double rDelta)
    {
    }

    void Application::EnginePhysicsProcess(double pDelta)
    {
    }

    void Application::PhysicsProcess(double pDelta)
    {
    }

    void Application::EngineDraw()
    {
    }

    void Application::Draw()
    {
    }

}