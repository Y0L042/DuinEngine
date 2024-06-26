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

		UUID GetUUID() { return objectUUID; }

		bool operator==(Object& other);

	protected:
		UUID objectUUID;

	private:

	};
}