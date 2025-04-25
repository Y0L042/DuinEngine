#pragma once

#include <toml.hpp>
#include <string>
#include <Duin/Core/Debug/DNLog.h>

namespace duin {
    class TOMLValue
    {
        public:
            enum Error {
                VALID = 0,
                INVALID = 1,
            };

            TOMLValue() = default;
            TOMLValue(const std::string& path);
            TOMLValue(const toml::value& value);
            TOMLValue(std::initializer_list<std::pair<const std::string, toml::basic_value<toml::type_config>>> init);
            ~TOMLValue() = default;

            toml::value& GetValue();
            int Load(const std::string& path);
            int Save(const std::string& path);
            int Save();
			int Contains(const std::string& key);
            TOMLValue At(const std::string& key);

            bool IsValid() const { return output == VALID; }
            bool IsTable() const { return value.is_table(); }
            bool IsArray() const { return value.is_array(); }

            std::string AsString() const { return value.as_string(); };
            int AsInteger() const { return value.as_integer(); };

			const TOMLValue operator[](const std::string& key) const;

			template<typename T>
			T Find(const std::string& key)
			{
                try {
                    return toml::find<T>(value, key);
                } catch (const std::exception& e) {
                    DN_CORE_FATAL("TOML find failed for key {}: {}", key, e.what());
                    return T();
                }
			}

            template<typename T>
            T As()
            {
                try {
                    return toml::get<T>(value);
                } catch (const std::exception& e) {
                    DN_CORE_FATAL("TOML get failed: {}", e.what());
                    return T();
                }
            }

            template<typename T>
            static T Get(TOMLValue value)
            {
                return toml::get<T>(value.value);
            }

            template<typename T>
            void Set(const std::string& key, const T& val)
            {
                if (value.is_table()) {
                    value.as_table()->insert_or_assign(key, val);
                }
            }


	    private:
		    toml::value value;
            Error output = VALID;
            std::string path = "";
    };
}
