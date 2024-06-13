#pragma once

#include "Duin/Core/Core.h"

#include <string>
#include <functional>

namespace Duin
{
	class DUIN_API GUID
	{
	public:
        GUID();
		GUID(const std::string& path);
        GUID(const char* path);

		void Generate(const std::string& path);

        size_t Get() const;         

        bool operator==(const GUID& other) const 
        {
            return guid == other.guid;
        }

        bool operator!=(const GUID& other) const 
        {
            return guid != other.guid;
        }

        bool operator<(const GUID& other) const 
        {
            return guid < other.guid;
        }
	private:
        size_t guid = 0;
	};
}

namespace std 
{
    template <>
    struct hash<Duin::GUID> 
    {
        size_t operator()(const Duin::GUID& guid) const 
        {
            return guid.Get();
        }
    };
}