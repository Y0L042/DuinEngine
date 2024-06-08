#pragma once

#include <Duin.h>

#include <memory>

struct Component_Renderable
{
    std::shared_ptr<Duin::TextureResource> texture;
    Duin::Vector2 size;
    bool pointInInputDir = false;

    Component_Renderable(std::shared_ptr<Duin::TextureResource> texture, Duin::Vector2 size)
        : texture(texture), size(size)
    {
        texture->SetTextureSize(size);
    }

    Component_Renderable(std::shared_ptr<Duin::TextureResource> texture, Duin::Vector2 size, bool pointInInputDir)
        : texture(texture), size(size), pointInInputDir(pointInInputDir)
    {
        texture->SetTextureSize(size);
    }
};