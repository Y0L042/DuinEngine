#pragma once

#include <Duin.h>

#include "Components/Components.h"

class AstroidCluster : public Duin::Node
{
public:
	AstroidCluster(Duin::Registry* registry, Duin::AssetManager* assetManager);
	~AstroidCluster();

private:
	Duin::Registry* registry;
	Duin::AssetManager* assetManager;
};