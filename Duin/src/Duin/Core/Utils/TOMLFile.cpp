#include "dnpch.h"
#include "TOMLFile.h"

#include <Duin/Core/Debug/DebugModule.h>

#include <toml.hpp>

namespace duin {
    TOMLValue::TOMLValue(const std::string& path)
        : value(Load(path))
    {}

	TOMLValue::TOMLValue(toml::value value)
		: value(value)
    {
    }

    int TOMLValue::Load(const std::string& path)
    {
        DN_CORE_INFO("Loading TOML file: {}", path);
		FILE* file = fopen(path.c_str(), "rb");
        if (!file) {
            DN_CORE_ERROR("Failed to open TOML file: {}", path);
			fclose(file);
            return INVALID;
        }
        toml::result result = toml::try_parse(path);
        fclose(file);

		if (result.is_ok()) {
			value = result.unwrap();
			output = VALID;
		}
		else {
			DN_CORE_ERROR("Failed to load TOML file: {}", path);
			output = INVALID;
		}

        return output;
    }

    int TOMLValue::Save(const std::string& path)
    {
        return 0;
    }

	int TOMLValue::Contains(const std::string& key)
	{
		return value.contains(key);
	}

    TOMLValue TOMLValue::At(const std::string& key)
    {
        return TOMLValue(value.at(key));
    }

    TOMLValue TOMLValue::operator[](const std::string& key)
    {
		return value[key];
    }

}
