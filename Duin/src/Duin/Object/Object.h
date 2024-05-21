#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Core/UUID.h"
#include "Duin/Core/Signal.h"
#include "Duin/Events/InputEvent.h"

#include <map>
#include <any>

namespace Duin
{
	class DUIN_API Object
	{
	public:
		Object();
		~Object();

		const UUID& GetUUID() { return objectUUID; }

	protected:
		UUID objectUUID;

	private:

	};
}