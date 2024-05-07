#pragma once
                    
#include "dnpch.h"

#include "Duin/Core/Core.h"

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
        
    private:

    };
}