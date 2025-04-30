#include "dnpch.h"
#include "ConfigValue.h"
#include <Duin/Core/Debug/DNLog.h>


namespace duin {
    ConfigValue::ConfigValue()
    {
    }
    ConfigValue::ConfigValue(const std::string& path)
    {
        // TODO
    }

    toml::value& ConfigValue::GetTOMLValue()
    {
        return modValue_;
    }

	ConfigValue::ConfigValue(const toml::value& value)
		: modValue_(value)
    {
    }

    ConfigValue::ConfigValue(std::initializer_list<std::pair<const std::string, toml::basic_value<toml::type_config>>> init)
        : modValue_(toml::table(init.begin(), init.end()))
    {
    }

    bool ConfigValue::IsValid() const
    {
        return false;
    }

    bool ConfigValue::Contains(const std::string& key)
	{
		return modValue_.contains(key);
	}

    ConfigValue ConfigValue::At(const std::string& key)
    {
        return ConfigValue(modValue_.at(key));
    }

    std::string ConfigValue::AsString() const
    {
        return std::string();
    }

    int ConfigValue::AsInteger() const
    {
        return 0;
    }

    bool ConfigValue::IsTable() const
    {
        return false;
    }

    bool ConfigValue::IsArray() const
    {
        return false;
    }

    const ConfigValue ConfigValue::operator[](const std::string& key) const {
        if (!modValue_.contains(key)) {
            return ConfigValue(); // Return empty value or throw
        }
        return ConfigValue(modValue_.at(key));
    }

}
