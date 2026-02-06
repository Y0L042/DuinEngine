#include "dnpch.h"
#include "ConfigValue.h"
#include <Duin/Core/Debug/DNLog.h>

#define VALID true
#define INVALID false

namespace duin
{
ConfigValue ConfigValue::Parse(const std::string &string)
{
    if (string.empty())
        return ConfigValue();
    try
    {
        return ConfigValue(toml::parse_str(string));
    }
    catch (const std::exception &e)
    {
        DN_CORE_WARN("ConfigValue::Parse failed: {}", e.what());
        return ConfigValue();
    }
}

ConfigValue ConfigValue::ParseFile(const std::string &file)
{
    const auto input = toml::try_parse(file.c_str());
    if (input.is_ok())
    {
        ConfigValue value(input.unwrap());
        return value;
    }
    DN_CORE_ERROR("ConfigValue failed to parse {}", file);
    return ConfigValue();
}

std::string ConfigValue::Format(const ConfigValue &value)
{
    if (value.IsEmpty())
        return "";
    return toml::format(value.modValue_);
}

ConfigValue::ConfigValue()
{
}

toml::value &ConfigValue::GetTOMLValue()
{
    return modValue_;
}

bool ConfigValue::IsEmpty() const
{
    bool e = modValue_.is_empty();
    return e;
}

ConfigValue::ConfigValue(const toml::value &value) : modValue_(value)
{
}

ConfigValue::ConfigValue(std::initializer_list<std::pair<const std::string, toml::basic_value<toml::type_config>>> init)
    : modValue_(toml::table(init.begin(), init.end()))
{
}

bool ConfigValue::Contains(const std::string &key)
{
    if (!modValue_.is_table())
        return false;
    return modValue_.contains(key);
}

ConfigValue ConfigValue::At(const std::string &key)
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
    return modValue_.is_table();
}

bool ConfigValue::IsArray() const
{
    return modValue_.is_array();
}

const ConfigValue ConfigValue::operator[](const std::string &key) const
{
    if (!modValue_.is_table() || !modValue_.contains(key))
    {
        return ConfigValue();
    }
    return ConfigValue(modValue_.at(key));
}

} // namespace duin
