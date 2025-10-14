/**
 * @file JSONValue.h
 * @brief Defines the duin::JSONValue class for manipulating JSON data using RapidJSON.
 * @author DuinEngine Contributors
 * @date 2024
 *
 * This file provides the JSONValue class, a wrapper around RapidJSON's Document and Value types,
 * enabling convenient and type-safe manipulation of JSON objects, arrays, and primitive values.
 * It also provides iterator classes for traversing JSON arrays and objects.
 *
 * Usage example:
 * @code
 * duin::JSONValue json = duin::JSONValue::Parse("{\"key\": 42}");
 * int value = json["key"].GetInt();
 * @endcode
 */

 /**
 * @example
 * @brief Example usage of duin::JSONValue for parsing, accessing, and modifying JSON data.
 *
 * This example demonstrates how to parse a JSON string, access members, modify values,
 * and serialize the result back to a string.
 *
 * @code
 * #include <Duin/IO/JSONValue.h>
 * 
 * // Parse a JSON string
 * duin::JSONValue json = duin::JSONValue::Parse("{\"name\": \"DuinEditor\", \"version\": 1}");
 * 
 * // Access a member by name
 * std::string name = json["name"].GetString();
 * int version = json["version"].GetInt();
 * 
 * // Modify a member
 * json["version"].SetInt(2);
 * 
 * // Add a new member
 * json.AddMember("active", true);
 * 
 * // Serialize back to string
 * std::string output = json.Write(true); // pretty print
 * 
 * // Output: {"name": "DuinEditor", "version": 2, "active": true}
 * @endcode
 */

#pragma once

#include <Duin/Core/Debug/DebugModule.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <string>
#include <memory>

namespace duin {
	class JSONValue;

    /**
    * @class JSONValue
    * @brief Wrapper for RapidJSON Document/Value, providing type-safe JSON manipulation.
    *
    * JSONValue allows parsing, writing, and modifying JSON data, supporting objects, arrays,
    * and primitive types. It manages memory using shared_ptr for the underlying RapidJSON Document.
    */
    class JSONValue
    {
        public:
            /**
             * @class DataIterator
             * @brief Iterator for mutable traversal of JSON arrays.
             */
            class DataIterator
            {
            public:
                /**
                 * @brief Default constructor. Creates an invalid iterator.
                 */
                DataIterator();
                ~DataIterator() = default;

                /**
                 * @brief Gets the JSONValue at the current iterator position.
                 * @return JSONValue at iterator position.
                 */
                JSONValue GetValue();

                /**
                 * @brief Dereference operator.
                 * @return JSONValue at iterator position.
                 */
                JSONValue operator*() const;

                /**
                 * @brief Prefix increment operator.
                 * @return Reference to incremented iterator.
                 */
                DataIterator& operator++();

                /**
                 * @brief Equality comparison operator.
                 * @param other Iterator to compare with.
                 * @return True if iterators are equal.
                 */
                bool operator==(const DataIterator other);

                /**
                 * @brief Inequality comparison operator.
                 * @param other Iterator to compare with.
                 * @return True if iterators are not equal.
                 */
                bool operator!=(const DataIterator other);

            private:
                friend class JSONValue;
                bool isReadValid; ///< True if iterator is valid for reading.
                bool isModValid;  ///< True if iterator is valid for modification.
                std::shared_ptr<rapidjson::Document> readDocument_; ///< Document shared pointer.
                rapidjson::Value::ValueIterator it_; ///< RapidJSON value iterator.

                /**
                 * @brief Internal constructor for valid iterator.
                 * @param document Shared pointer to RapidJSON document.
                 * @param it RapidJSON value iterator.
                 */
                DataIterator(std::shared_ptr<rapidjson::Document> document, rapidjson::Value::ValueIterator it);
            };

            /**
             * @class ConstDataIterator
             * @brief Iterator for const traversal of JSON arrays.
             */
            class ConstDataIterator
            {
                public:
                    /**
                     * @brief Default constructor. Creates an invalid iterator.
                     */
                    ConstDataIterator();
                    ~ConstDataIterator() = default;

                    /**
                     * @brief Gets the JSONValue at the current iterator position.
                     * @return JSONValue at iterator position.
                     */
                    const JSONValue GetValue();

                    /**
                     * @brief Dereference operator.
                     * @return JSONValue at iterator position.
                     */
                    JSONValue operator*() const;

                    /**
                     * @brief Prefix increment operator.
                     * @return Incremented iterator.
                     */
                    ConstDataIterator operator++();

                    /**
                     * @brief Equality comparison operator.
                     * @param other Iterator to compare with.
                     * @return True if iterators are equal.
                     */
                    bool operator==(const ConstDataIterator other);

                    /**
                     * @brief Inequality comparison operator.
                     * @param other Iterator to compare with.
                     * @return True if iterators are not equal.
                     */
                    bool operator!=(const ConstDataIterator other);

                private:
                    friend class JSONValue;
                    bool isReadValid; ///< True if iterator is valid for reading.
                    std::shared_ptr<rapidjson::Document> readDocument_; ///< Document shared pointer.
                    rapidjson::Value::ConstValueIterator it_; ///< RapidJSON const value iterator.

                    /**
                     * @brief Internal constructor for valid iterator.
                     * @param document Shared pointer to RapidJSON document.
                     * @param it RapidJSON const value iterator.
                     */
                    ConstDataIterator(std::shared_ptr<rapidjson::Document> document, const rapidjson::Value::ConstValueIterator it);
            };

            /**
             * @brief Parses a JSON file into a JSONValue object.
             * @param data JSON filepath to parse.
             * @return Parsed JSONValue object.
             */
            static JSONValue ParseFromFile(const std::string& filePath);

            /**
            * @brief Parses a JSON string into a JSONValue object.
            * @param data JSON string to parse.
            * @return Parsed JSONValue object.
            */
            static JSONValue Parse(const std::string& string);

            /**
             * @brief Serializes a JSONValue to a string.
             * @param value JSONValue to serialize.
             * @param prettyWrite If true, output is pretty-printed.
             * @return JSON string.
             */
            static std::string Write(const JSONValue& value, bool prettyWrite = false);

            /**
             * @brief Returns an invalid JSONValue instance.
             * @return Invalid JSONValue.
             */
			static JSONValue Invalid();

            /**
             * @brief Default constructor. Creates an empty JSON object.
             */
            JSONValue();

            /**
             * @brief Constructs a JSONValue holding a boolean.
             * @param b Boolean value.
             */
            JSONValue(bool b);

            /**
             * @brief Constructs a JSONValue holding an integer.
             * @param x Integer value.
             */
            JSONValue(int x);

            /**
             * @brief Constructs a JSONValue holding a string.
             * @param text String value.
             */
            JSONValue(const std::string& text);

            /**
             * @brief Constructs a JSONValue holding a double.
             * @param x Double value.
             */
            JSONValue(double x);

            ~JSONValue() = default;

            /**
             * @brief Returns underlying RapidJSON value reference.
             * @return Reference to RapidJSON value.
             */
            rapidjson::Value& GetRJSONValue();

            /**
             * @brief Serializes this JSONValue to a string.
             * @return JSON string.
             */
            std::string Write() const;

            /**
             * @brief Creates a deep copy of this JSONValue and its underlying document.
             * @return Cloned JSONValue.
             */
            JSONValue Clone() const;

            /**
             * @brief Checks if the JSONValue is valid for reading.
             * @return True if valid.
             */
            bool IsReadValid() const;

            /**
             * @brief Checks if this object has a member with the given name.
             * @param member Member name.
             * @return True if member exists.
             */
            bool HasMember(const std::string& member) const;

            /**
             * @brief Gets the value of a member by name.
             * @param member Member name.
             * @return JSONValue of the member, or empty if not found.
             */
            JSONValue GetMember(const std::string& member) const;

            /**
             * @brief Adds a member to the object.
             * @param key Member name.
             * @param dv Value to add.
             * @param allowDuplicates If false, removes existing member first.
             * @return Reference to this JSONValue.
             */
            JSONValue& AddMember(const std::string& key, JSONValue dv, bool allowDuplicates = false);

            /**
             * @brief Removes a member from the object.
             * @param key Member name.
             * @return Reference to this JSONValue.
             */
            JSONValue& RemoveMember(const std::string& key);

            /**
             * @brief Checks if this JSONValue is valid (not marked invalid).
             * @return True if valid.
             */
            bool IsValid();

            /**
             * @brief Checks if this value is null.
             * @return True if null.
             */
            bool IsNull() const;

            /**
             * @brief Checks if this value is an object.
             * @return True if object.
             */
            bool IsObject() const;

            /**
             * @brief Checks if this value is a string.
             * @return True if string.
             */
            bool IsString() const;

            /**
             * @brief Gets the string value.
             * @return String value, or empty string if not a string.
             */
            std::string GetString() const;

            /**
             * @brief Checks if this value is a boolean.
             * @return True if boolean.
             */
            bool IsBool() const;

            /**
             * @brief Gets the boolean value.
             * @return Boolean value, or false if not a boolean.
             */
            bool GetBool() const;

            /**
             * @brief Checks if this value is a number.
             * @return True if number.
             */
            bool IsNumber() const;

            /**
             * @brief Checks if this value is an integer.
             * @return True if integer.
             */
            bool IsInt() const;

            /**
             * @brief Gets the integer value.
             * @return Integer value, or 0 if not an integer.
             */
            int GetInt() const;

            /**
             * @brief Checks if this value is a double.
             * @return True if double.
             */
            bool IsDouble() const;

            /**
             * @brief Gets the double value.
             * @return Double value, or 0.0 if not a double.
             */
            double GetDouble() const;

            /**
             * @brief Checks if this value is an array.
             * @return True if array.
             */
            bool IsArray() const;

            /**
             * @brief Checks if the array is empty.
             * @return True if empty.
             */
            bool Empty() const;

            /**
             * @brief Adds a shallow copy of dv to array-type JSONValue.
             * @param dv Value to add.
             * @return Reference to this JSONValue.
             */
            JSONValue& PushBack(JSONValue dv);

            /**
             * @brief Gets the capacity of the array.
             * @return Array capacity.
             */
            size_t Capacity() const;

            /**
             * @brief Returns iterator to beginning of array.
             * @return ConstDataIterator to beginning.
             */
            ConstDataIterator Begin();

            /**
             * @brief Returns iterator to end of array.
             * @return ConstDataIterator to end.
             */
            ConstDataIterator End();

            /**
             * @brief Finds a member in the object by name.
             * @param member Member name.
             * @return ConstDataIterator to member, or end if not found.
             */
            ConstDataIterator FindMember(const std::string& member) const;

            /**
             * @brief Sets this value to an object type.
             * @return Reference to this JSONValue.
             */
            JSONValue SetObject();

            /**
             * @brief Sets this value to an integer.
             * @param x Integer value.
             * @return Reference to this JSONValue.
             */
            JSONValue SetInt(int x);

            /**
             * @brief Sets this value to a string.
             * @param text String value.
             * @return Reference to this JSONValue.
             */
            JSONValue SetString(const std::string& text);

            /**
             * @brief Sets this value to a double.
             * @param x Double value.
             * @return Reference to this JSONValue.
             */
            JSONValue SetDouble(double x);

            /**
             * @brief Sets this value to a boolean.
             * @param b Boolean value.
             * @return Reference to this JSONValue.
             */
            JSONValue SetBool(bool b);

            /**
             * @brief Sets this value to an array type.
             * @return Reference to this JSONValue.
             */
            JSONValue SetArray();

            /**
             * @brief Adds a member to the object with a primitive value.
             * @tparam T Type of value.
             * @param key Member name.
             * @param val Value to add.
             * @param allowDuplicates If false, removes existing member first.
             * @return Reference to this JSONValue.
             */
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

            /**
             * @brief Adds a primitive value to array-type JSONValue.
             * @tparam T Type of value.
             * @param val Value to add.
             * @return Reference to this JSONValue.
             */
            template <typename T>
            JSONValue PushBack(T val)
            {
                if (jvalue_->IsArray()) {
                    rapidjson::Document::AllocatorType& alloc = jdoc_->GetAllocator();
                    jvalue_->PushBack(val, alloc);
                    return *this;
                }
                DN_CORE_WARN("JSONValue is not an array!");
                return JSONValue();
            }

            /**
             * @brief Accesses a member by name.
             * @param member Member name.
             * @return JSONValue of member, or empty if not found.
             */
            JSONValue operator[](const std::string& member);

            /**
             * @brief Accesses an array element by index.
             * @param idx Array index.
             * @return JSONValue of element, or empty if not found.
             */
            JSONValue operator[](int idx);

            /**
             * @brief Dereference operator.
             * @return Reference to this JSONValue.
             */
            const JSONValue& operator*() const;

            /**
             * @brief Equality comparison operator.
             * @param other JSONValue to compare with.
             * @return True if equal.
             */
            bool operator==(const JSONValue& other) const;

            /**
             * @brief Inequality comparison operator.
             * @param other JSONValue to compare with.
             * @return True if not equal.
             */
            bool operator!=(const JSONValue& other) const;

            /**
             * @brief Returns iterator to beginning of array (mutable).
             * @return DataIterator to beginning.
             */
            DataIterator      begin();      
            /**
             * @brief Returns iterator to end of array (mutable).
             * @return DataIterator to end.
             */
            DataIterator      end();        
            /**
             * @brief Returns iterator to beginning of array (const).
             * @return ConstDataIterator to beginning.
             */
            ConstDataIterator begin() const;
            /**
             * @brief Returns iterator to end of array (const).
             * @return ConstDataIterator to end.
             */
            ConstDataIterator end()   const;

        private:
            bool INVALID_ = false; ///< True if this JSONValue is invalid.
            std::shared_ptr<rapidjson::Document> jdoc_; ///< Underlying RapidJSON document.
            rapidjson::Value *jvalue_; ///< Pointer to RapidJSON value.

            /**
             * @brief Internal constructor for subvalue referencing.
             * @param document Shared pointer to RapidJSON document.
             * @param value Pointer to RapidJSON value.
             */
            JSONValue(std::shared_ptr<rapidjson::Document> document, rapidjson::Value *value = nullptr);

            /**
             * @brief Internal constructor for const subvalue referencing.
             * @param document Shared pointer to RapidJSON document.
             * @param value Pointer to const RapidJSON value.
             */
            JSONValue(std::shared_ptr<rapidjson::Document> document, const rapidjson::Value* value);
    };
}
