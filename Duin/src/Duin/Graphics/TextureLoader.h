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
        
    private:
        SDL_Renderer* gRenderer = NULL;
    };
}