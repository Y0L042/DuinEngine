#pragma once

#include "Duin/Core/Debug/DebugModule.h"
#include <toml.hpp>
#include <string>

namespace duin
{
class ConfigValue
{
  public:
    static ConfigValue Parse(const std::string &string);
    static ConfigValue ParseFile(const std::string &data);
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
        // Check if value is a table and key exists
        if (!modValue_.is_table() || !modValue_.contains(key))
        {
            DN_CORE_WARN("ConfigValue::Find key '{}' not found", key);
            return T();
        }

        const auto &value = modValue_.at(key);

        // Special handling for vector<double> and vector<float> to allow mixed int/float arrays
        if constexpr (std::is_same_v<T, std::vector<double>> || std::is_same_v<T, std::vector<float>>)
        {
            if (!value.is_array())
            {
                DN_CORE_WARN("ConfigValue::Find key '{}' is not an array", key);
                return T();
            }

            const auto &arr = value.as_array();
            T result;
            for (const auto &elem : arr)
            {
                if (elem.is_integer())
                    result.push_back(static_cast<typename T::value_type>(elem.as_integer()));
                else if (elem.is_floating())
                    result.push_back(static_cast<typename T::value_type>(elem.as_floating()));
                else
                {
                    DN_CORE_WARN("ConfigValue::Find array element is not numeric");
                    return T();
                }
            }
            return result;
        }
        else
        {
            // For other types, use toml::get which validates the type
            try
            {
                return toml::get<T>(value);
            }
            catch (const std::exception &e)
            {
                DN_CORE_WARN("ConfigValue::Find type mismatch for key '{}': {}", key, e.what());
                return T();
            }
        }
    }

    template <typename T>
    T As()
    {
        if (IsEmpty())
        {
            DN_CORE_WARN("ConfigValue::As called on empty value");
            return T();
        }

        try
        {
            return toml::get<T>(modValue_);
        }
        catch (const std::exception &e)
        {
            DN_CORE_WARN("ConfigValue::As type conversion failed: {}", e.what());
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
        if (modValue_.is_empty())
        {
            modValue_ = toml::table{};
        }
        if (modValue_.is_table())
        {
            modValue_.as_table().insert_or_assign(key, val);
        }
    }

    const ConfigValue operator[](const std::string &key) const;

    ConfigValue(const toml::value &value);

  private:
    toml::value modValue_;

    ConfigValue(std::initializer_list<std::pair<const std::string, toml::basic_value<toml::type_config>>> init);
};
} // namespace duin
