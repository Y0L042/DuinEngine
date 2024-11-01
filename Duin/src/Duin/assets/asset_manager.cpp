#include "dnpch.h"
#include "asset_manager.h"

namespace Duin {

AssetManager::AssetManager()
{
    uuid = uuid_create_random();
}

AssetManager::~AssetManager()
{
}

}

