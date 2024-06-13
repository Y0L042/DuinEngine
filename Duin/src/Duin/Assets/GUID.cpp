#include "dnpch.h"

#include "GUID.h"

namespace Duin
{
	GUID::GUID()
	{
	}

	GUID::GUID(const std::string& path)
	{
		Generate(path);
	}
	GUID::GUID(const char* path)
	{
		Generate(std::string(path));
	}

	void GUID::Generate(const std::string& path)
	{
		guid = std::hash<std::string>{}(path);
	}

	size_t GUID::Get() const
	{
		return guid;
	}
}