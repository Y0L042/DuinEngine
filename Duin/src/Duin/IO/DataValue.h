#pragma once

#include <Duin/Core/Debug/DebugModule.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <string>
#include <memory>

namespace duin {
    class JSONValue
    {
        public:
            class DataIterator
            {
            public:
                DataIterator();
                ~DataIterator() = default;

                JSONValue GetValue();

                JSONValue operator*() const;
                DataIterator& operator++();
                bool operator==(const DataIterator other);
                bool operator!=(const DataIterator other);

            private:
                friend class JSONValue;
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

                    const JSONValue GetValue();

                    JSONValue operator*() const;
                    ConstDataIterator operator++();
                    bool operator==(const ConstDataIterator other);
                    bool operator!=(const ConstDataIterator other);

                private:
                    friend class JSONValue;
                    bool isReadValid;
                    std::shared_ptr<rapidjson::Document> readDocument_;
                    rapidjson::Value::ConstValueIterator it_;

                    ConstDataIterator(std::shared_ptr<rapidjson::Document> document, const rapidjson::Value::ConstValueIterator it);
            };



            // TODO change this so that it only parses json string no loading
            static JSONValue Parse(const std::string& data);
            static std::string Write(const JSONValue& value, bool prettyWrite = false);
            /* TODO
            static void WriteToFile(const std::string& filePath);
            static void WriteToFile(const std::string& filePath);
            */

            /**
             * @brief JSONValue constructors.
             * !TODO These currently do not work properly, and requires the user to correctly cast the parameter
             */
            JSONValue();
            JSONValue(bool b);
            JSONValue(int x);
            JSONValue(const std::string& text);
            JSONValue(double x);

            ~JSONValue() = default;

            /* Returns underlying rapidjson reference*/
            rapidjson::Value& GetRJSONValue() { return *jvalue_; }

            /* Write the JSONValue to a string */
            std::string Write() const;
            /* Createa deep copy of the JSONValue and the underlying rjson document */
            JSONValue Clone() const;

            /* Tests if the JSONValue is valid, with valid underlying data */
            bool IsReadValid() const;
            bool HasMember(const std::string& member) const;
            JSONValue& AddMember(const std::string& key, JSONValue dv, bool allowDuplicates = false);
            JSONValue& RemoveMember(const std::string& key);

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

            /* Array-related methods */
            bool IsArray() const;
            // TODO GetArray();
            bool Empty() const;
            /* Adds a shallow copy of dv to JSONValue (of array type). Only push back after dv is initialised. */
            JSONValue& PushBack(JSONValue dv);
            size_t Capacity() const;
            ConstDataIterator Begin();
            ConstDataIterator End();
            ConstDataIterator FindMember(const std::string& member) const;

            /* Sets the type of the data the JSONValue is storing */
            JSONValue SetObject();
            JSONValue SetInt(int x);
            JSONValue SetString(const std::string& text);
            JSONValue SetDouble(double x);
            JSONValue SetBool(bool b);
            JSONValue SetArray();


            template <typename T>
            JSONValue& AddMember(const std::string& key, T val, bool allowDuplicates = false) // Add Key:Value member to object
            {
                if (!jvalue_->IsObject()) {
                    DN_CORE_WARN("JSONValue not an Object, cannot add member!");
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

            /* Adds data to array-type JSONValue */
            template <typename T>
            JSONValue PushBack(T val)
            {
                if (jvalue_->IsArray()) {
                    rapidjson::Document::AllocatorType& alloc = jdoc_->GetAllocator();
                    rapidjson::Value v;
                    v.Set(val);
                    jvalue_->PushBack(std::move(v), alloc);
                    return *this;
                }
                DN_CORE_WARN("JSONValue is not an array!");
                return JSONValue();
            }



            JSONValue operator[](const std::string& member);
            JSONValue operator[](int idx); // Array access
            const JSONValue& operator*() const;
            bool operator==(const JSONValue& other);
            bool operator!=(const JSONValue& other);

            // For range-based looping
            DataIterator      begin()       { return DataIterator(jdoc_, jvalue_->Begin()); }      
            DataIterator      end()         { return DataIterator(jdoc_, jvalue_->End()); }        
            ConstDataIterator begin() const { return ConstDataIterator(jdoc_, jvalue_->Begin()); } 
            ConstDataIterator end()   const { return ConstDataIterator(jdoc_, jvalue_->End()); }   

        private:
            std::shared_ptr<rapidjson::Document> jdoc_;
            rapidjson::Value *jvalue_;

            JSONValue(std::shared_ptr<rapidjson::Document> document, rapidjson::Value *value = nullptr);
            JSONValue(std::shared_ptr<rapidjson::Document> document, const rapidjson::Value* value);

    };
}
