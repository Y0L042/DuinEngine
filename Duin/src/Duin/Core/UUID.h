#pragma once

#include "Duin/Core/Core.h"

#include <cinttypes>
#include <functional>

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
		
		bool operator<(const UUID& other) const {
			return mUUID < other.mUUID;
		}

		operator uint64_t() const { return mUUID; }

	private:
		uint64_t mUUID;
	};
}



// Define a specialization of std::hash for Duin::UUID
namespace std {
	template <>
	struct hash<Duin::UUID>
	{
		std::size_t operator()(const Duin::UUID& uuid) const
		{
			return std::hash<uint64_t>()(static_cast<uint64_t>(uuid));
		}
	};
}