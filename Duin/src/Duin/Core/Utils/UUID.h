#pragma once

#include <functional>
#include <cstdint>
#include <string>

namespace duin {

    class UUID
    {
        public:
            static const UUID INVALID;

            UUID();
            UUID(uint64_t uuid);
            ~UUID();

            static std::string ToStringDec(UUID uuid);
            static std::string ToStringHex(UUID uuid);
            static UUID FromStringDec(const std::string& string);
            static UUID FromStringHex(const std::string& string);

            bool operator==(const UUID& uuid) const
            {
                return this->uuid_ == uuid.uuid_;
            }

            bool operator!=(const UUID& uuid) const
            {
                return this->uuid_ != uuid.uuid_;
            }

            operator uint64_t() const
            {
                return uuid_;
            }

        private:
            uint64_t uuid_;
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
