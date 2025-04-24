#pragma once

#include <string>
#include <toml.hpp>

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
            TOMLValue(toml::value value);

            ~TOMLValue() = default;

            int Load(const std::string& path);
            int Save(const std::string& path);
			int Contains(const std::string& key);
            TOMLValue At(const std::string& key);

			TOMLValue operator[](const std::string& key);

			template<typename T>
			T Find(const std::string& key)
			{
				return toml::find<T>(value, key);
			}

            template<typename T>
            T As()
            {
                return toml::get<T>(this->value);
            }

            template<typename T>
            static T Get(TOMLValue value)
            {
                return toml::get<T>(value.value);
            }


	    private:
		    toml::value value;
            Error output = VALID;
    };
}
