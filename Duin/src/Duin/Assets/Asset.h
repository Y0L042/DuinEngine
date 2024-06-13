#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Core/UUID.h"

namespace Duin
{
	class AssetManager;

	/*
	* A wrapper for the _Asset class.
	* The client will interact with this class, and children.
	*/
	class DUIN_API Asset
	{
	public:
		Asset();
		Asset(AssetManager* assetManager);
		~Asset();

		void SetAssetManager(AssetManager* assetManager);
		AssetManager* GetAssetManager();

	private:
		UUID uuid;
		AssetManager* assetManager = nullptr;
	};
}