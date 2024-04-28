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

        const uint32_t RENDER_TIME = (uint32_t)(1000 / TARGET_RENDER_FRAMERATE);
        const uint32_t PHYSICS_TIME = (uint32_t)(1000 / TARGET_PHYSICS_FRAMERATE);

        uint32_t rDelta = 0;
        uint32_t pDelta = 0;

        //While application is running
        while (!quit)
        {
            //Handle events on queue
            while (SDL_PollEvent(&e) != 0)
            {
                //User requests quit
                if (e.type == SDL_QUIT)
                {
                    quit = true;
                }
            }

            uint32_t currentTicks = SDL_GetTicks();
            rDelta = currentTicks - renderTimeCount;
            pDelta = currentTicks - physicsTimeCount;
            bool executeRenderFrame = rDelta >= RENDER_TIME;
            bool executePhysicsFrame = pDelta >= PHYSICS_TIME;
            if (executeRenderFrame) { renderTimeCount = currentTicks; }
            if (executePhysicsFrame) { physicsTimeCount = currentTicks; }

            if (pDelta >= RENDER_TIME) { HandleEvents(); }
            if (rDelta >= RENDER_TIME) { Process(rDelta); }
            if (pDelta >= PHYSICS_TIME) { PhysicsProcess(pDelta); }
            if (rDelta >= RENDER_TIME) { Render(); }

            //Update the surface

        }

        //Free resources and close SDL
        win->Close();
    }

    void Application::Ready()
    {
    }

    void Application::HandleEvents()
    {
    }

    void Application::Process(double rDelta)
    {
    }

    void Application::PhysicsProcess(double pDelta)
    {
    }

    void Application::Render()
    {
        win->Render();
    }

}