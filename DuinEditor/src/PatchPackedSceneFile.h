#pragma once

#include <string>
#include <Duin/Assets/AssetRef.h>
#include <Duin/Scene/SceneBuilder.h>
#include "FileManager.h"

bool EnsurePackedSceneFile(std::shared_ptr<FSNode> node);
bool EnsurePackedEntity(duin::PackedEntity& e);
bool EnsurePackedComponent(duin::PackedComponent& c);
bool ValidateAssetRef(duin::AssetRef& assetRef);