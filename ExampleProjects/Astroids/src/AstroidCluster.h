#pragma once

#include <Duin.h>

#include "Components/Components.h"

class AstroidCluster : public Duin::Node
{
public:
	AstroidCluster(std::shared_ptr<Duin::Blackboard> sharedData);
	~AstroidCluster();

private:
	std::shared_ptr<Duin::Registry> registry;
	std::shared_ptr<Duin::AssetManager> assetManager;
	std::shared_ptr<Duin::QuadTree> quadTree;
};