#include "dnpch.h"

#include "Sprite2D.h"

namespace Duin
{
    Sprite2D::Sprite2D()
    {
    }

    Sprite2D::Sprite2D(const char* i_texturePath)
    {
        texture = std::make_shared<TextureResource>(i_texturePath);
    }

    Sprite2D::~Sprite2D()
    {
    }

    std::shared_ptr<TextureResource> Sprite2D::GetTexture()
    {
        return texture;
    }

    std::shared_ptr<Sprite2D> Sprite2D::SetTexture(char* texturePath)
    {
        texture = std::make_shared<TextureResource>(texturePath);
        return nullptr;
    }

    std::shared_ptr<Sprite2D> Sprite2D::SetTexture(TextureResource* texturePtr)
    {
        texture = std::shared_ptr<TextureResource>(texturePtr);
        return nullptr;
    }

    std::shared_ptr<Sprite2D> Sprite2D::SetTexture(std::shared_ptr<TextureResource> texturePtr)
    {
        texture = texturePtr;
        return nullptr;
    }

    std::shared_ptr<Sprite2D> Duin::Sprite2D::SetTextureSize(Vector2 size)
    {
        if (texture)
        {
            texture->SetTextureSize(size);
        }
        return nullptr;
    }

    std::shared_ptr<Sprite2D> Duin::Sprite2D::SetTextureSize(float width, float height)
    {
        return SetTextureSize(Vector2{ width, height });
    }

    void Sprite2D::Draw()
    {
        if (texture)
        {
            texture->Draw(GetGlobalPosition2D());
        }
    }
}