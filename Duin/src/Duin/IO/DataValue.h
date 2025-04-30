#pragma once

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

#include <string>
#include <memory>

namespace duin {
    class DataValue
    {
        public:
            class DataIterator
            {
            public:
                DataIterator();
                ~DataIterator() = default;

                DataValue operator*() const;
                DataIterator& operator++();
                bool operator==(const DataIterator other);
                bool operator!=(const DataIterator other);

            private:
                friend class DataValue;
                bool isValid;
                std::shared_ptr<rapidjson::Document> document_;
                rapidjson::Value::ValueIterator it_;

                DataIterator(std::shared_ptr<rapidjson::Document> document, rapidjson::Value::ValueIterator it);
            };

            class ConstDataIterator
            {
                public:
                    ConstDataIterator();
                    ~ConstDataIterator() = default;

                    DataValue operator*() const;
                    ConstDataIterator& operator++();
                    bool operator==(const ConstDataIterator other);
                    bool operator!=(const ConstDataIterator other);

                private:
                    friend class DataValue;
                    bool isValid;
                    std::shared_ptr<rapidjson::Document> document_;
                    rapidjson::Value::ConstValueIterator it_;

                    ConstDataIterator(std::shared_ptr<rapidjson::Document> document, const rapidjson::Value::ConstValueIterator it);
            };



            static DataValue Parse(const std::string& filePath);

            DataValue();
            ~DataValue() = default;

            bool IsValid() const;
            bool HasMember(const std::string& member) const;


            bool IsString() const;
            std::string GetString() const;

            bool IsBool() const;
            bool GetBool() const;

            bool IsNumber() const;
            bool IsInt() const;
            int GetInt() const;
            bool IsDouble() const;
            double GetDouble() const;

            bool IsArray() const;
            bool Empty() const;
            size_t Capacity() const;
            ConstDataIterator Begin();
            ConstDataIterator End();

            DataValue operator[](const std::string& member);
            DataValue operator*() const;
            bool operator==(const DataValue& other);
            bool operator!=(const DataValue& other);

        private:
            std::shared_ptr<rapidjson::Document> document_;
            rapidjson::Value *value_;

            DataValue(std::shared_ptr<rapidjson::Document> document, rapidjson::Value *value = nullptr);
            DataValue(std::shared_ptr<rapidjson::Document> document, const rapidjson::Value* value);

    };
}
