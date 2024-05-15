#include "dnpch.h"

#include "Node2D.h"

namespace Duin
{
	Node2D::Node2D()
	{
	}

	Node2D::Node2D(raylib::Vector2 i_globalPosition2D)
		: globalPosition2D(i_globalPosition2D)
	{

	}

	Node2D::~Node2D()
	{
	}

	raylib::Vector2 Node2D::GetGlobalPosition2D()
	{
		return globalPosition2D;
	}

	void Node2D::SetGlobalPosition2D(raylib::Vector2 pos)
	{
		globalPosition2D = pos;
	}

}