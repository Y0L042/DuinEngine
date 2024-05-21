#include "dnpch.h"

#include "Node2D.h"

namespace Duin
{
	Node2D::Node2D()
	{
	}

	Node2D::Node2D(Vector2 i_globalPosition2D)
	{
		localTransform2D.origin = i_globalPosition2D;
	}

	Node2D::~Node2D()
	{
	}

	Vector2 Node2D::GetLocalPosition2D()
	{
		return localTransform2D.origin;
	}

	void Node2D::SetLocalPosition2D(Vector2 pos)
	{
		localTransform2D.origin = pos;
	}

	Vector2 Node2D::GetGlobalPosition2D()
	{
		return Vector2();
	}

	void Node2D::SetGlobalPosition2D(Vector2 pos)
	{
	}

	Transform2D Node2D::GetLocalTransform2D()
	{
		return Transform2D();
	}

	Transform2D Node2D::GetGlobalTransform2D()
	{
		return Transform2D();
	}

	void Node2D::UpdateGlobalTransform2DCache()
	{
	}

}