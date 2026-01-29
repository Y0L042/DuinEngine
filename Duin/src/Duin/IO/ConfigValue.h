#pragma once

#include "Duin/Core/Debug/DebugModule.h"
#include <toml.hpp>
#include <string>

namespace duin
{
class ConfigValue
{
  public:
    static ConfigValue Parse(const std::string &data);
    static std::string Format(const ConfigValue &value);

    ConfigValue();

    ~ConfigValue() = default;

    toml::value &GetTOMLValue();

    bool IsEmpty() const;

    bool Contains(const std::string &key);
    ConfigValue At(const std::string &key);

    std::string AsString() const;
    int AsInteger() const;

    bool IsTable() const;
    bool IsArray() const;

    template <typename T>
    T Find(const std::string &key)
    {
        try
        {
            return toml::find<T>(modValue_, key);
        }
        catch (const std::exception &e)
        {
            DN_CORE_FATAL("TOML find failed for key {}: {}", key, e.what());
            return T();
        }
    }

    template <typename T>
    T As()
    {
        try
        {
            return toml::get<T>(modValue_);
        }
        catch (const std::exception &e)
        {
            DN_CORE_FATAL("TOML get failed: {}", e.what());
            return T();
        }
    }

    template <typename T>
    static T Get(ConfigValue value)
    {
        return toml::get<T>(value.modValue_);
    }

    template <typename T>
    void Set(const std::string &key, const T &val)
    {
        if (modValue_.is_table())
        {
            modValue_.as_table()->insert_or_assign(key, val);
        }
    }

    const ConfigValue operator[](const std::string &key) const;

  private:
    toml::value modValue_;

    ConfigValue(const toml::value &value);
    ConfigValue(std::initializer_list<std::pair<const std::string, toml::basic_value<toml::type_config>>> init);
};
} // namespace duin
