#pragma once
#include "dnpch.h"

#include "Duin/Core/Core.h"
#include "Duin/Core/UUID.h"
#include "Duin/Core/Maths/DuinMaths.h"
#include "Duin/Object/Node/Node.h"

#include <functional>

namespace Duin
{
	class DUIN_API Node2D : public Node
	{
	public:
		Node2D();
		Node2D(Vector2 localPos2D);
		~Node2D();

		void InternalInitialize() override;

		Vector2 GetLocalPosition2D();
		void SetLocalPosition2D(Vector2 pos);
		void SetLocalPosition2D(float x, float y);


		Vector2 GetGlobalPosition2D();
		void SetGlobalPosition2D(Vector2 pos);

		Transform2D GetLocalTransform2D();
		void SetLocalTransform2D(Transform2D transform);

		Transform2D GetGlobalTransform2D();

		void ConnectToTransformUpdate(std::function<void(Transform2D)> callback);
		void GetParentGlobalTransform2D(Transform2D parentTF );

	private:
		Signal<Transform2D> onAnyTransformUpdate;
		Transform2D localTransform2D;
		Transform2D globalTransform2DCache;
		bool globalTransformCacheDirty = true;
	};
}