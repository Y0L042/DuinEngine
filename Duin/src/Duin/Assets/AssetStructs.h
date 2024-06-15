#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Assets/GUID.h"
#include "Duin/Core/Maths/DuinMaths.h"

#include <RLImGuiComponent.h>

namespace Duin
{
    class AssetManager;

    /*
    * Base Asset structs
    * 
    * These structs are used by the engine to store the assets loaded from storage.
    * 
    */

	struct DUIN_API _Asset
	{
		virtual ~_Asset() = default;
		GUID guid;				// Base resource GUID

        _Asset() {}
		_Asset(const char* path) : guid(path) {}
    };

    class DUIN_API _AssetFactory
    {
    public:
        _AssetFactory() {};
        ~_AssetFactory() {};

        bool enabled = true;

        virtual std::shared_ptr<_Asset> CreateAsset(AssetManager* assetManager, const char* path) = 0;
    };



    // Texture, tex data stored in GPU memory (VRAM)
    struct DUIN_API _TextureAsset : public _Asset
    {
        unsigned int id;        // OpenGL texture id
        int width;              // Texture base width
        int height;             // Texture base height
        int mipmaps;            // Mipmap levels, 1 by default
        int format;             // Data format (PixelFormat type)
        ::Texture rlTextureCache;
        
        _TextureAsset() {}

        _TextureAsset(const char* path)
            : _Asset(path), id(0), width(0), height(0), mipmaps(1), format(0) {}  // Default constructor

        _TextureAsset(const ::Texture& texture, const char* path)  // Conversion constructor from raylib::Texture
            : _Asset(path), id(texture.id), width(texture.width), height(texture.height),
            mipmaps(texture.mipmaps), format(texture.format), rlTextureCache(texture) {}

        ::Texture ToRaylibTexture() const // Method to convert back to raylib::Texture
        {
            return { id, width, height, mipmaps, format };
        }
    };

    class DUIN_API _TextureAssetFactory : public _AssetFactory
    {
    public:
        std::shared_ptr<_Asset> CreateAsset(AssetManager* assetManager, const char* path) override;
    };



    // Image, pixel data stored in CPU memory (RAM)
    struct DUIN_API _ImageAsset : public _Asset
    {
        void* data;             // Image raw data
        int width;              // Image base width
        int height;             // Image base height
        int mipmaps;            // Mipmap levels, 1 by default
        int format;             // Data format (PixelFormat type)
        ::Image rlImageCache;

        _ImageAsset() {}

        _ImageAsset(const char* path) 
            : _Asset(path), data(nullptr), width(0), height(0), mipmaps(1), format(0) {}  // Default constructor

        _ImageAsset(const ::Image& image, const char* path)  // Conversion constructor from raylib::Image
            : _Asset(path), data(image.data), width(image.width), height(image.height),
            mipmaps(image.mipmaps), format(image.format), rlImageCache(image) {}

        ::Image ToRaylibImage() const // Method to convert back to raylib::Image
        {
            return { data, width, height, mipmaps, format };
        }
    };

    class DUIN_API _ImageAssetFactory : public _AssetFactory
    {
    public:
        std::shared_ptr<_Asset> CreateAsset(AssetManager* assetManager, const char* path) override;
    };

}