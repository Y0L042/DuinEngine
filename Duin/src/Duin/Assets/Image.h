#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Core/Scene/Object.h"
#include "Duin/Assets/AssetStructs.h"

#include <memory>

namespace Duin
{
	class DUIN_API Image : public Object
	{
	public:
		Image();
		~Image();

	private:
		std::shared_ptr<_ImageAsset> imageAsset;
	};
}