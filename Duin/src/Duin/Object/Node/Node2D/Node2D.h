#pragma once
#include "dnpch.h"

#include "Duin/Core/Core.h"
#include "Duin/Core/UUID.h"
#include "Duin/Core/Maths/DuinMaths.h"
#include "Duin/Object/Node/Node.h"

namespace Duin
{
	class DUIN_API Node2D : public Node
	{
	public:
		Node2D();
		Node2D(Vector2 localPos2D);
		~Node2D();

		Vector2 GetLocalPosition2D();
		void SetLocalPosition2D(Vector2 pos);

		Vector2 GetGlobalPosition2D();
		void SetGlobalPosition2D(Vector2 pos);

		Transform2D GetLocalTransform2D();
		Transform2D GetGlobalTransform2D();

		Signal<Transform2D> onLocalTransformDirty;
		bool localTransformDirty;

		Transform2D localTransform2D;
		Transform2D globalTransform2DCache;

		void UpdateGlobalTransform2DCache();

	private:

	};
}