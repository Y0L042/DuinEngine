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

	void Node2D::InternalInitialize()
	{
		if (!parentPtr) return;

		std::shared_ptr<Node2D> parent2D = std::dynamic_pointer_cast<Node2D>(parentPtr);
		if (parent2D)
		{
			parent2D->ConnectToTransformUpdate(
				[this](Transform2D parentGlobalTransform2D)
				{
					this->GetParentGlobalTransform2D(parentGlobalTransform2D);
				}
			);
			DN_CORE_INFO("Connected to parentPtr [{0}] transformUpdate!", parentPtr->GetNodeAlias());
		}
		else
		{
			DN_CORE_ERROR("Cannot convert parentPtr [{0}] to Node2D!", parentPtr->GetNodeAlias());
		}
	}

	Vector2 Node2D::GetLocalPosition2D()
	{
		return localTransform2D.origin;
	}

	void Node2D::SetLocalPosition2D(float x, float y)
	{
		SetLocalPosition2D({ x, y });
	}

	void Node2D::SetLocalPosition2D(Vector2 pos)
	{
		localTransform2D.origin = pos;
		onAnyTransformUpdate.Emit(globalTransform2DCache + localTransform2D);
	}

	Vector2 Node2D::GetGlobalPosition2D()
	{
		return globalTransform2DCache.origin;
	}

	void Node2D::SetGlobalPosition2D(Vector2 pos)
	{
		// TODO
	}

	Transform2D Node2D::GetLocalTransform2D()
	{
		return localTransform2D;
	}

	void Node2D::SetLocalTransform2D(Transform2D transform)
	{
		localTransform2D = transform;
		onAnyTransformUpdate.Emit(globalTransform2DCache + localTransform2D);
	}

	Transform2D Node2D::GetGlobalTransform2D()
	{
		return globalTransform2DCache;
	}

	void Node2D::ConnectToTransformUpdate(std::function<void(Transform2D)> callback)
	{
		onAnyTransformUpdate.Connect(callback);
	}

	void Node2D::GetParentGlobalTransform2D(Transform2D parentGlobalTransform2D)
	{
		globalTransform2DCache = localTransform2D + parentGlobalTransform2D;
		onAnyTransformUpdate.Emit(globalTransform2DCache);
	}

}