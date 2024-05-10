#include "dnpch.h"

#include "Sprite2D.h"

namespace Duin
{
    Sprite2D::Sprite2D()
    {
    }

    Sprite2D::Sprite2D(const char* texturePath)
        : texture(std::make_shared<TextureRes>(texturePath))
    {
    }

    Sprite2D::Sprite2D(TextureRes* texturePtr)
        : texture(std::shared_ptr<TextureRes>(texturePtr))
    {
    }

    Sprite2D::Sprite2D(std::shared_ptr<TextureRes> texturePtr)
        : texture(texturePtr)
    {
    }

    Sprite2D::~Sprite2D()
    {
    }

    std::shared_ptr<TextureRes> Sprite2D::GetTexture()
    {
        return texture;
    }

    Sprite2D& Sprite2D::SetTexture(char* texturePath)
    {
        texture = std::make_shared<TextureRes>(texturePath);
        return *this;
    }

    Sprite2D& Sprite2D::SetTexture(TextureRes* texturePtr)
    {
        texture = std::shared_ptr<TextureRes>(texturePtr);
        return *this;
    }

    Sprite2D& Sprite2D::SetTexture(std::shared_ptr<TextureRes> texturePtr)
    {
        texture = texturePtr;
        return *this;
    }

    void Sprite2D::Draw()
    {
        if (texture != nullptr)
        {
            texture->Draw(globalPosition2D.x, globalPosition2D.y);
        }
    }
}