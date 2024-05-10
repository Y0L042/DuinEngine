#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Core/UUID.h"

namespace Duin
{
	class DUIN_API Object
	{
	public:
		Object();
		~Object();

		const UUID& GetUUID() { return idComponent; }

	private:
		UUID idComponent;
	};
}