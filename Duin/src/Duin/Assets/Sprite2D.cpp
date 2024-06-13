#include "dnpch.h"

#include "Sprite2D.h"

namespace Duin
{
    Sprite2D::Sprite2D()
    {
    }

    Sprite2D::Sprite2D(std::shared_ptr<Texture> texture)
        : texture(texture)
    {
    }

    Sprite2D::~Sprite2D()
    {   
    }

    void Sprite2D::SetTexture(std::shared_ptr<Texture> texture)
    {
        this->texture = texture;
    }

    std::shared_ptr<Texture> Sprite2D::GetTexture()
    {
        return texture;
    }
}