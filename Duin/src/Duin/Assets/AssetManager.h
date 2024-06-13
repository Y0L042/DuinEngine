#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Core/UUID.h"
#include "Duin/Core/Structures/RenderStructs.h"
#include "Duin/Assets/GUID.h"
#include "Duin/Assets/AssetStructs.h"

#include "RLImGuiComponent.h"

#include <unordered_map>
#include <string>

namespace Duin
{
	class DUIN_API AssetManager
	{
	public:
		AssetManager();
		~AssetManager();

		template<typename ASSET, typename FACTORY>
		std::shared_ptr<ASSET> LoadAsset(const char* path);

		void AddToAssetRegistry(std::shared_ptr<_Asset> asset);
		void RemoveFromAssetRegistry(GUID guid);

	private:
		std::unordered_map<GUID, std::shared_ptr<_Asset>> assetRegistry;
	};

	template<typename ASSET, typename FACTORY>
	inline std::shared_ptr<ASSET> AssetManager::LoadAsset(const char* path)
	{
		static_assert(std::is_base_of<_Asset, ASSET>::value, "ASSET must be a _Asset derived struct!");
		static_assert(std::is_base_of<_AssetFactory, FACTORY>::value, "FACTORY must be a _AssetFactory derived class!");

		// Check for existing Asset
		GUID guid(path);
		auto it = assetRegistry.find(guid);
		if (it != assetRegistry.end())
		{
			return std::dynamic_pointer_cast<ASSET>(it->second);
		}

		// Else load/create Asset 
		FACTORY* factory = new FACTORY();
		std::shared_ptr<_Asset> asset = factory->CreateAsset(this, path);
		AddToAssetRegistry(asset);
		return std::dynamic_pointer_cast<ASSET>(asset);
	}
}