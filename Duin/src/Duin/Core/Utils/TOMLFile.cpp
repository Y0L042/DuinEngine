#include "dnpch.h"
#include "TOMLFile.h"
#include <Duin/Core/Debug/DebugModule.h>


namespace duin {
    TOMLValue::TOMLValue(const std::string& path)
    {
        value = Load(path);
		output = VALID;
    }

	TOMLValue::TOMLValue(const toml::value& value)
		: value(value)
    {
		output = VALID;
    }

    TOMLValue::TOMLValue(std::initializer_list<std::pair<const std::string, toml::basic_value<toml::type_config>>> init)
        : value(toml::table(init.begin(), init.end()))
    {
        output = VALID;
    }

    toml::value& TOMLValue::GetValue()
    {
        return value;
    }

    int TOMLValue::Load(const std::string& path)
    {
        DN_CORE_INFO("Loading TOML file: {}", path);
        this->path = path;
        toml::result result = toml::try_parse(path);

		if (result.is_ok()) {
			value = result.unwrap();
			output = VALID;
		}
		else {
			DN_CORE_FATAL("Failed to load TOML file: {}", path);
			output = INVALID;
		}

        return output;
    }

    int TOMLValue::Save(const std::string& path)
    {
        std::ofstream file(path);
        if (!file.is_open()) {
            DN_CORE_FATAL("Failed to open file for writing: {}", path);
            return INVALID;
        }
        file << value;
        file.close();

        return VALID;
    }

    int TOMLValue::Save()
    {
        return Save(path);
    }

	int TOMLValue::Contains(const std::string& key)
	{
		return value.contains(key);
	}

    TOMLValue TOMLValue::At(const std::string& key)
    {
        return TOMLValue(value.at(key));
    }

    const TOMLValue TOMLValue::operator[](const std::string& key) const {
        if (!value.contains(key)) {
            return TOMLValue(); // Return empty value or throw
        }
        return TOMLValue(value.at(key));
    }

}
