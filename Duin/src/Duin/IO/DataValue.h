#pragma once

#include <Duin/Core/Debug/DebugModule.h>
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
                    ConstDataIterator operator++();
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
            static std::string Write(const DataValue& value, bool prettyWrite = false);
            /* TODO
            static void WriteToFile(const std::string& filePath);
            static void WriteToFile(const std::string& filePath);
            */

            DataValue();
            DataValue(bool b);
            DataValue(int x);
            DataValue(const std::string& text);
            DataValue(double x);

            ~DataValue() = default;

            rapidjson::Value& GetRJSONValue() { return *jvalue_; }

            std::string Write() const;
            DataValue Clone() const;

            bool IsReadValid() const;
            bool HasMember(const std::string& member) const;
            DataValue& AddMember(const std::string& key, DataValue dv, bool allowDuplicates = false);
            DataValue& RemoveMember(const std::string& key);

            bool IsNull() const;
            bool IsObject() const;
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


            DataValue SetObject();
            DataValue SetInt(int x);
            DataValue SetString(const std::string& text);
            DataValue SetDouble(double x);
            DataValue SetBool(bool b);
            DataValue SetArray();

            template <typename T>
            DataValue& AddMember(const std::string& key, T val, bool allowDuplicates = false) // Add Key:Value member to object
            {
                if (!jvalue_->IsObject()) {
                    DN_CORE_WARN("DataValue not an Object, cannot add member!");
                    return *this;
                }

                if (!allowDuplicates) {
                    RemoveMember(key.c_str());
                }

                rapidjson::Value name(key.c_str(), 
                                      static_cast<rapidjson::SizeType>(key.size()),
                                      jdoc_->GetAllocator());

                rapidjson::Value v;
                if constexpr (std::is_same_v<T, std::string>) {
                    v.SetString(val.c_str(),
                        static_cast<rapidjson::SizeType>(val.size()),
                        jdoc_->GetAllocator());
                }
                else if constexpr (std::is_same_v<T, char>) {
                    char buf[2] = { val, '\0' };
                    v.SetString(buf,
                        static_cast<rapidjson::SizeType>(sizeof(char)),
                        jdoc_->GetAllocator());

                }
                else {
                    v.Set(val);
                }

                jvalue_->AddMember(std::move(name),
                                   std::move(v),
                                   jdoc_->GetAllocator());

                return *this;
            }

            template <typename T>
            DataValue PushBack(T val)
            {
                if (jvalue_->IsArray()) {
                    rapidjson::Document::AllocatorType& alloc = jdoc_->GetAllocator();
                    rapidjson::Value v;
                    v.Set(val);
                    jvalue_->PushBack(std::move(v), alloc);
                    return *this;
                }
                DN_CORE_WARN("DataValue is not an array!");
                return DataValue();
            }

            DataValue& PushBack(DataValue dv);


            DataValue operator[](const std::string& member);
            DataValue operator[](int idx); // Array access
            const DataValue& operator*() const;
            bool operator==(const DataValue& other);
            bool operator!=(const DataValue& other);

            // For range-based looping
            DataIterator      begin()       { return DataIterator(jdoc_, jvalue_->Begin()); }      
            DataIterator      end()         { return DataIterator(jdoc_, jvalue_->End()); }        
            ConstDataIterator begin() const { return ConstDataIterator(jdoc_, jvalue_->Begin()); } 
            ConstDataIterator end()   const { return ConstDataIterator(jdoc_, jvalue_->End()); }   

        private:
            std::shared_ptr<rapidjson::Document> jdoc_;
            rapidjson::Value *jvalue_;

            DataValue(std::shared_ptr<rapidjson::Document> document, rapidjson::Value *value = nullptr);
            DataValue(std::shared_ptr<rapidjson::Document> document, const rapidjson::Value* value);

    };
}
