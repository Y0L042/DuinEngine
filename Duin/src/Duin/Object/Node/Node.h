#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Object/Object.h"
#include "Duin/Events/InputEvent.h"
#include "Duin/Events/InputMap.h"

namespace Duin
{
	class DUIN_API Node : public Object
	{
	public:
		Node();
		~Node();

		virtual void Ready() override;
		virtual void HandleInput(InputEvent e) override;
		virtual void Update(double rDelta) override;
		virtual void PhysicsUpdate(double pDelta) override;

	private:

	};
}