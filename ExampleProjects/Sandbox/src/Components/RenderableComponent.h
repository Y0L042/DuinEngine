#pragma once

#include <Duin.h>

#include <memory>

struct RenderableCMP
{
    Duin::Texture* texture;
    Duin::Vector2 size;

    RenderableCMP(Duin::Texture* texture, Duin::Vector2 size)
        : texture(texture), size(size) 
    {
        //texture->SetTextureSize(size);
    }
};