#pragma once

#include <Duin.h>

#include <memory>

struct Component_Renderable
{
    std::shared_ptr<Duin::TextureResource> texture;
    Duin::Vector2 size;

    Component_Renderable(std::shared_ptr<Duin::TextureResource> texture, Duin::Vector2 size)
        : texture(texture), size(size)
    {
        texture->SetTextureSize(size);
    }
};