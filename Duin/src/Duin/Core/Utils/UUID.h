#pragma once

#include <functional>
#include <cstdint>

namespace duin {

class UUID
{
public:
    UUID();
    UUID(uint64_t uuid);
    ~UUID();

    bool operator==(const UUID& uuid) const
    {
        return this->uuid == uuid.uuid;
    }

    bool operator!=(const UUID& uuid) const
    {
        return this->uuid != uuid.uuid;
    }

    operator uint64_t() const
    {
        return uuid;
    }

private:
    uint64_t uuid;
};

}

namespace std 
{
	template <>
	struct hash<duin::UUID> {
		std::size_t operator()(const duin::UUID& uuid) const
		{
			return std::hash<uint64_t>()(static_cast<uint64_t>(uuid));
		}
	};
}
