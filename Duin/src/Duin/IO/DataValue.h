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

                DataValue GetValue();

                DataValue operator*() const;
                DataIterator& operator++();
                bool operator==(const DataIterator other);
                bool operator!=(const DataIterator other);

            private:
                friend class DataValue;
                bool isReadValid;
                bool isModValid;
                std::shared_ptr<rapidjson::Document> readDocument_;
                rapidjson::Value::ValueIterator it_;

                DataIterator(std::shared_ptr<rapidjson::Document> document, rapidjson::Value::ValueIterator it);
            };

            class ConstDataIterator
            {
                public:
                    ConstDataIterator();
                    ~ConstDataIterator() = default;

                    const DataValue GetValue();

                    DataValue operator*() const;
                    ConstDataIterator& operator++();
                    bool operator==(const ConstDataIterator other);
                    bool operator!=(const ConstDataIterator other);

                private:
                    friend class DataValue;
                    bool isReadValid;
                    std::shared_ptr<rapidjson::Document> readDocument_;
                    rapidjson::Value::ConstValueIterator it_;

                    ConstDataIterator(std::shared_ptr<rapidjson::Document> document, const rapidjson::Value::ConstValueIterator it);
            };



            // TODO change this so that it only parses json string no loading
            static DataValue Parse(const std::string& data);
            /* TODO
            static void WriteToFile(const std::string& filePath);
            static void WriteToFile(const std::string& filePath);
            */

            DataValue();
            DataValue(bool b);
            DataValue(int x);
            DataValue(const std::string& text);
            DataValue(double x);
            DataValue(const DataValue& other);

            ~DataValue() = default;

            rapidjson::Value& GetRJSONValue() { return *readValue_; }

            bool IsReadValid() const;
            bool HasMember(const std::string& member) const;

            bool IsNull() const;
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
            // TODO GetArray();
            bool Empty() const;
            size_t Capacity() const;
            ConstDataIterator Begin();
            ConstDataIterator End();
            ConstDataIterator FindMember(const std::string& member) const;

            void SetObject();
            void SetInt(int x);
            void SetString(const std::string& text);
            void SetDouble(double x);
            void SetBool(bool b);
            void SetArray();


            const DataValue operator[](const std::string& member);
            const DataValue operator[](int idx); // Array access
            const DataValue operator*() const;
            bool operator==(const DataValue& other);
            bool operator!=(const DataValue& other);
            void operator=(DataValue& other);

        private:
            std::shared_ptr<rapidjson::Document> readDocument_;
            rapidjson::Value *readValue_; // This value is read, not meant for setting
                                      // It is ultimately owned by readDocument_

            rapidjson::Value modValue_;

            DataValue(std::shared_ptr<rapidjson::Document> document, rapidjson::Value *value = nullptr);
            DataValue(std::shared_ptr<rapidjson::Document> document, const rapidjson::Value* value);

    };
}
