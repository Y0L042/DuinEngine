#pragma once

#include "Duin/Core/Core.h"

#include <cinttypes>

namespace Duin
{
	class DUIN_API UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		~UUID();

		bool operator==(const UUID& uuid) const
		{
			return this->mUUID == uuid.mUUID;
		}
		operator uint64_t() const { return mUUID; }

	private:
		uint64_t mUUID;
	};
}