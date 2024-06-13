#pragma once

#include <Duin.h>

#include "Components/Components.h"

class AstroidCluster : public Duin::Node
{
public:
	AstroidCluster(Duin::Registry* registry);
	~AstroidCluster();

private:
	Duin::Registry* registry;
};