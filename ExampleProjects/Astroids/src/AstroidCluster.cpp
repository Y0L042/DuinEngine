#include "AstroidCluster.h"

AstroidCluster::AstroidCluster(std::shared_ptr<Duin::Blackboard> sharedData)
	: 
	registry(sharedData->GetValueCast<std::shared_ptr<Duin::Registry>>("REG")),
	assetManager(sharedData->GetValueCast<std::shared_ptr<Duin::AssetManager>>("ASSETMGR")),
	quadTree(sharedData->GetValueCast<std::shared_ptr<Duin::QuadTree>>("QTREE"))
{
}

AstroidCluster::~AstroidCluster()
{
}
