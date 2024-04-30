#pragma once
                    
#include "dnpch.h"

#include "Duin/Core.h"
#include <SDL.h>

namespace Duin
{
    class DUIN_API TextureLoader
    {
    public:
        static TextureLoader& GetInstance()
        {
            static TextureLoader* instance = new TextureLoader();
            return *instance;
        }

        static TextureLoader& SetRenderer(SDL_Renderer* renderer);
        static SDL_Renderer* GetRenderer() { return GetInstance().gRenderer; }

        static TextureLoader& SetWindow(SDL_Window* window);
        static SDL_Window* GetWindow() { return GetInstance().gWindow; }
        
    private:
        SDL_Renderer* gRenderer = nullptr;
        SDL_Window* gWindow = nullptr;
    };
}