#include "dnpch.h"
#include "ConfigValue.h"
#include <Duin/Core/Debug/DNLog.h>

#define VALID true
#define INVALID false


namespace duin {
    ConfigValue ConfigValue::Parse(const std::string& data)
    {
        const auto input = toml::try_parse(data.c_str());
        if (input.is_ok()) {
            ConfigValue value(input.unwrap());
            return value;
        }
        DN_CORE_ERROR("ConfigValue failed to parse {}", data);
        return ConfigValue();
    }

    std::string ConfigValue::Format(const ConfigValue& value)
    {
        std::string out = toml::format(value.modValue_);
        return out;
    }

    ConfigValue::ConfigValue()
    {}

    toml::value& ConfigValue::GetTOMLValue()
    {
        return modValue_;
    }

    bool ConfigValue::IsEmpty() const
    {
        bool e = modValue_.is_empty();
        return e;
    }

	ConfigValue::ConfigValue(const toml::value& value)
		: modValue_(value)
    {}

    ConfigValue::ConfigValue(std::initializer_list<std::pair<const std::string, toml::basic_value<toml::type_config>>> init)
        : modValue_(toml::table(init.begin(), init.end()))
    {}

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
