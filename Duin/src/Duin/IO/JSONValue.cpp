/**
 * @file DataValue.cpp
 * @brief Implements the duin::JSONValue class for JSON manipulation using RapidJSON.
 * @author DuinEngine Contributors
 * @date 2024
 *
 * This file contains the implementation of the JSONValue class, which wraps RapidJSON's Document and Value types
 * to provide convenient and type-safe manipulation of JSON objects, arrays, and primitive values. It also implements
 * iterator classes for traversing JSON arrays and objects.
 */

#include "dnpch.h"
#include "JSONValue.h"
#include <Duin/Core/Debug/DNLog.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <sstream>
#include <fstream>

namespace duin
{
/* DATAVALUE */
/**
 * @brief Parses a JSON file into a JSONValue object.
 * @param filePath Path to the JSON file.
 * @return Parsed JSONValue object.
 */
JSONValue JSONValue::ParseFromFile(const std::string &filePath)
{
    DN_CORE_INFO("Attempting to parse {} into JSONValue...", filePath);
    JSONValue dv;

    std::ifstream ifs(filePath);
    if (!ifs)
    {
        DN_CORE_ERROR("Cannot open {}!", filePath);
        dv.SetObject();
        return dv;
    }

    std::stringstream buffer;
    buffer << ifs.rdbuf();
    std::string content = buffer.str();
    DN_CORE_INFO("Content:\n{}", content);

    return Parse(content); // Reuse Parse logic
}

/**
 * @brief Parses a JSON string into a JSONValue object.
 * @param string JSON string.
 * @return Parsed JSONValue object.
 */
JSONValue JSONValue::Parse(const std::string &string)
{
    JSONValue dv;

    dv.jdoc_ = std::make_shared<rapidjson::Document>();
    dv.jdoc_->Parse(string.c_str());
    dv.jvalue_ = dv.jdoc_.get();

    // Check for parse errors
    if (dv.jdoc_->HasParseError())
    {
        DN_CORE_ERROR("JSON parsing failed at offset {}: Error code {}", dv.jdoc_->GetErrorOffset(),
                      static_cast<int>(dv.jdoc_->GetParseError()));
        dv.SetObject(); // Default to empty object on error
        return dv;
    }

    // Accept any valid JSON type (object, array, primitive, etc.)
    if (dv.jvalue_->IsObject())
    {
        DN_CORE_TRACE("Parsed JSON object");
    }
    else if (dv.jvalue_->IsArray())
    {
        DN_CORE_TRACE("Parsed JSON array");
    }
    else if (dv.jvalue_->IsString())
    {
        DN_CORE_TRACE("Parsed JSON string");
    }
    else if (dv.jvalue_->IsNumber())
    {
        DN_CORE_TRACE("Parsed JSON number");
    }
    else if (dv.jvalue_->IsBool())
    {
        DN_CORE_TRACE("Parsed JSON boolean");
    }
    else if (dv.jvalue_->IsNull())
    {
        DN_CORE_TRACE("Parsed JSON null");
    }

    return dv;
}

/**
 * @brief Returns underlying RapidJSON value reference.
 * @return Reference to RapidJSON value.
 */
rapidjson::Value &JSONValue::GetRJSONValue()
{
    return *jvalue_;
}

/**
 * @brief Serializes a JSONValue to a string.
 * @param value JSONValue to serialize.
 * @param prettyWrite If true, output is pretty-printed.
 * @return JSON string.
 */
std::string JSONValue::Write(const JSONValue &value, bool prettyWrite)
{
    rapidjson::StringBuffer buffer;

    if (prettyWrite)
    {
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        writer.SetIndent('\t', 1);

        value.jvalue_->Accept(writer);
        std::string out = buffer.GetString();

        return out;
    }
    else
    {
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

        value.jvalue_->Accept(writer);
        std::string out = buffer.GetString();

        return out;
    }
}

/**
 * @brief Returns an invalid JSONValue instance.
 * @return Invalid JSONValue.
 */
JSONValue JSONValue::Invalid()
{
    JSONValue v;
    v.INVALID_ = true;
    return v;
}

/**
 * @brief Default constructor. Creates an empty JSON object.
 */
JSONValue::JSONValue()
{
    jdoc_ = std::make_shared<rapidjson::Document>();
    jdoc_->SetObject();
    jvalue_ = jdoc_.get();
}

/**
 * @brief Constructs a JSONValue holding a boolean.
 * @param b Boolean value.
 */
JSONValue::JSONValue(bool b) : JSONValue()
{
    jvalue_->SetBool(b);
}

/**
 * @brief Constructs a JSONValue holding an integer.
 * @param x Integer value.
 */
JSONValue::JSONValue(int x) : JSONValue()
{
    jvalue_->SetInt(x);
}

/**
 * @brief Constructs a JSONValue holding a string.
 * @param text String value.
 */
JSONValue::JSONValue(const std::string &text) : JSONValue()
{
    jvalue_->SetString(text.c_str(), static_cast<rapidjson::SizeType>(text.size()));
}

/**
 * @brief Constructs a JSONValue holding a double.
 * @param x Double value.
 */
JSONValue::JSONValue(double x) : JSONValue()
{
    jvalue_->SetDouble(x);
}

/**
 * @brief Internal constructor for subvalue referencing.
 * @param document Shared pointer to RapidJSON document.
 * @param value Pointer to RapidJSON value.
 */
JSONValue::JSONValue(std::shared_ptr<rapidjson::Document> document, rapidjson::Value *value)
    : jdoc_(document), jvalue_(value)
{
    // If no subvalue is passed, use document as value
    if (jvalue_ == nullptr)
    {
        jvalue_ = jdoc_.get();
    }
}

/**
 * @brief Internal constructor for const subvalue referencing.
 * @param document Shared pointer to RapidJSON document.
 * @param value Pointer to const RapidJSON value.
 */
JSONValue::JSONValue(std::shared_ptr<rapidjson::Document> document, const rapidjson::Value *value)
    : jdoc_(document), jvalue_(const_cast<rapidjson::Value *>(value)) // Cast const to non-const
{
    // If no subvalue is passed, use document as value
    if (jvalue_ == nullptr)
    {
        jvalue_ = jdoc_.get();
        DN_CORE_ERROR("Cannot create JSONValue with const nullptr rapidjson::Value!");
    }
}

/**
 * @brief Serializes this JSONValue to a string.
 * @return JSON string.
 */
std::string JSONValue::Write() const
{
    return JSONValue::Write(*this);
}

/**
 * @brief Creates a deep copy of this JSONValue and its underlying document.
 * @return Cloned JSONValue.
 */
JSONValue JSONValue::Clone() const
{
    auto newDoc = std::make_shared<rapidjson::Document>();
    newDoc->CopyFrom(*jvalue_, newDoc->GetAllocator());

    return JSONValue(newDoc, newDoc.get());
}

/**
 * @brief Checks if the JSONValue is valid for reading.
 * @return True if valid.
 */
bool JSONValue::IsReadValid() const
{
    if (jdoc_ && jvalue_)
    {
        return true;
    }
    else
    {
        return false;
    }
    DN_CORE_WARN("JSONValue is empty!");
    return false;
}

/**
 * @brief Checks if this object has a member with the given name.
 * @param member Member name.
 * @return True if member exists.
 */
bool JSONValue::HasMember(const std::string &member) const
{
    if (jvalue_)
    {
        if (!jvalue_->IsObject())
        {
            DN_CORE_WARN("JSONValue is not an Object! Cannot search for key.");
            return false;
        }
        else if (jvalue_->HasMember(member.c_str()))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    DN_CORE_WARN("JSONValue is empty!");
    return false;
}

/**
 * @brief Gets the value of a member by name.
 * @param member Member name.
 * @return JSONValue of the member, or empty if not found.
 */
JSONValue JSONValue::GetMember(const std::string &member) const
{
    if (IsReadValid())
    {
        if (!jvalue_->IsObject())
        {
            DN_CORE_WARN("JSONValue is not an Object!");
            return JSONValue();
        }
        if (!jvalue_->HasMember(member.c_str()))
        {
            DN_CORE_WARN("JSONValue does not contain member {}!", member);
            return JSONValue();
        }
        rapidjson::Value &value = (*jvalue_)[member.c_str()];
        return JSONValue(jdoc_, &value);
    }
    DN_CORE_WARN("JSONValue is empty!");
    return JSONValue();
}

/**
 * @brief Adds a member to the object.
 * @param key Member name.
 * @param dv Value to add.
 * @param allowDuplicates If false, removes existing member first.
 * @return Reference to this JSONValue.
 */
JSONValue &JSONValue::AddMember(const std::string &key, JSONValue dv, bool allowDuplicates)
{
    if (!jvalue_->IsObject())
    {
        DN_CORE_WARN("JSONValue not an Object, cannot add nested member!");
        return *this;
    }

    if (!allowDuplicates)
    {
        RemoveMember(key.c_str());
    }

    rapidjson::Value name(key.c_str(), static_cast<rapidjson::SizeType>(key.size()), jdoc_->GetAllocator());

    rapidjson::Value nodeCopy;
    nodeCopy.CopyFrom(dv.GetRJSONValue(), jdoc_->GetAllocator());

    jvalue_->AddMember(std::move(name), std::move(nodeCopy), jdoc_->GetAllocator());

    return *this;
}

/**
 * @brief Removes a member from the object.
 * @param keyStr Member name.
 * @return Reference to this JSONValue.
 */
JSONValue &JSONValue::RemoveMember(const std::string &keyStr)
{
    if (jvalue_ && jvalue_->IsObject() && jvalue_->HasMember(keyStr.c_str()))
    {
        jvalue_->RemoveMember(keyStr.c_str());
    }

    return *this;
}

/**
 * @brief Checks if this JSONValue is valid (not marked invalid).
 * @return True if valid.
 */
bool JSONValue::IsValid()
{
    return !INVALID_;
}

/**
 * @brief Checks if this value is null.
 * @return True if null.
 */
bool JSONValue::IsNull() const
{
    if (jvalue_)
    {
        bool v = jvalue_->IsNull();
        return v;
    }
    DN_CORE_WARN("JSONValue is empty!");
    return false;
}

/**
 * @brief Checks if this value is an object.
 * @return True if object.
 */
bool JSONValue::IsObject() const
{
    if (jvalue_)
    {
        bool v = jvalue_->IsObject();
        return v;
    }
    DN_CORE_WARN("JSONValue is empty!");
    return false;
}

/**
 * @brief Checks if this value is a string.
 * @return True if string.
 */
bool JSONValue::IsString() const
{
    if (jvalue_)
    {
        bool v = jvalue_->IsString();
        return v;
    }
    DN_CORE_WARN("JSONValue is empty!");
    return false;
}

/**
 * @brief Gets the string value.
 * @return String value, or empty string if not a string.
 */
std::string JSONValue::GetString() const
{
    if (jvalue_)
    {
        if (jvalue_->IsString())
        {
            std::string s(jvalue_->GetString());
            return s;
        }
        else
        {
            DN_CORE_WARN("JSONValue not string!");
            return std::string();
        }
    }
    DN_CORE_WARN("JSONValue is empty!");
    return std::string();
}

/**
 * @brief Checks if this value is a boolean.
 * @return True if boolean.
 */
bool JSONValue::IsBool() const
{
    if (jvalue_)
    {
        bool v = jvalue_->IsBool();
        return v;
    }
    DN_CORE_WARN("JSONValue is empty!");
    return false;
}

/**
 * @brief Gets the boolean value.
 * @return Boolean value, or false if not a boolean.
 */
bool JSONValue::GetBool() const
{
    if (jvalue_)
    {
        if (jvalue_->IsBool())
        {
            bool v = jvalue_->GetBool();
            return v;
        }
        else
        {
            DN_CORE_WARN("JSONValue not boolean!");
        }
    }
    DN_CORE_WARN("JSONValue is empty!");
    return false;
}

/**
 * @brief Checks if this value is a number.
 * @return True if number.
 */
bool JSONValue::IsNumber() const
{
    if (jvalue_)
    {
        bool v = jvalue_->IsNumber();
        return v;
    }
    DN_CORE_WARN("JSONValue is empty!");
    return false;
}

/**
 * @brief Checks if this value is an integer.
 * @return True if integer.
 */
bool JSONValue::IsInt() const
{
    if (jvalue_)
    {
        bool v = jvalue_->IsInt();
        return v;
    }
    DN_CORE_WARN("JSONValue is empty!");
    return false;
}

/**
 * @brief Gets the integer value.
 * @return Integer value, or 0 if not an integer.
 */
int JSONValue::GetInt() const
{
    if (jvalue_)
    {
        if (jvalue_->IsInt())
        {
            int v = jvalue_->GetInt();
            return v;
        }
        else
        {
            DN_CORE_WARN("JSONValue not integer!");
        }
    }
    DN_CORE_WARN("JSONValue is empty!");
    return 0;
}

/**
 * @brief Checks if this value is a double.
 * @return True if double.
 */
bool JSONValue::IsDouble() const
{
    if (jvalue_)
    {
        bool v = jvalue_->IsDouble();
        return v;
    }
    DN_CORE_WARN("JSONValue is empty!");
    return false;
}

/**
 * @brief Gets the double value.
 * @return Double value, or 0.0 if not a double.
 */
double JSONValue::GetDouble() const
{
    if (jvalue_)
    {
        if (jvalue_->IsDouble())
        {
            double v = jvalue_->GetDouble();
            return v;
        }
        else
        {
            DN_CORE_WARN("JSONValue not double!");
        }
    }
    DN_CORE_WARN("JSONValue is empty!");
    return 0.0;
}

/**
 * @brief Checks if this value is an array.
 * @return True if array.
 */
bool JSONValue::IsArray() const
{
    if (jvalue_)
    {
        bool v = jvalue_->IsArray();
        return v;
    }
    DN_CORE_WARN("JSONValue is empty!");
    return false;
}

/**
 * @brief Checks if the array is empty.
 * @return True if empty.
 */
bool JSONValue::Empty() const
{
    if (jvalue_)
    {
        if (jvalue_->IsArray())
        {
            bool v = jvalue_->Empty();
            return v;
        }
        else
        {
            DN_CORE_WARN("JSONValue not array!");
        }
    }
    DN_CORE_WARN("JSONValue is empty!");
    return false;
}

/**
 * @brief Gets the capacity of the array.
 * @return Array capacity.
 */
size_t JSONValue::Capacity() const
{
    if (jvalue_)
    {
        if (jvalue_->IsArray())
        {
            rapidjson::SizeType v = jvalue_->Capacity();
            size_t size = static_cast<size_t>(v);
            return size;
        }
        else
        {
            DN_CORE_WARN("JSONValue not array!");
        }
    }
    DN_CORE_WARN("JSONValue is empty!");
    return size_t();
}

JSONValue::ConstDataIterator JSONValue::Begin()
{
    if (jvalue_)
    {
        if (jvalue_->IsArray())
        {
            rapidjson::Value::ConstValueIterator it = jvalue_->Begin();
            return ConstDataIterator(jdoc_, it);
        }
        else if (jvalue_->IsObject())
        {
            rapidjson::Value::ConstMemberIterator mit = jvalue_->MemberBegin();
            // Note: For objects, we'd need a different iterator type
            // For now, warn and return invalid iterator
            DN_CORE_WARN("Begin() called on object - use FindMember() or begin() for member iteration!");
            return ConstDataIterator();
        }
        else
        {
            DN_CORE_WARN("JSONValue not array or object!");
        }
    }
    DN_CORE_WARN("JSONValue is empty!");
    return ConstDataIterator();
}

JSONValue::ConstDataIterator JSONValue::End()
{
    if (jvalue_)
    {
        if (jvalue_->IsArray())
        {
            rapidjson::Value::ConstValueIterator it = jvalue_->End();
            return ConstDataIterator(jdoc_, it);
        }
        else if (jvalue_->IsObject())
        {
            DN_CORE_WARN("End() called on object - use FindMember() or end() for member iteration!");
            return ConstDataIterator();
        }
        else
        {
            DN_CORE_WARN("JSONValue not array or object!");
        }
    }
    DN_CORE_WARN("JSONValue is empty!");
    return ConstDataIterator();
}

/**
 * @brief Returns iterator to beginning of array (mutable).
 * @return DataIterator to beginning.
 */
JSONValue::DataIterator JSONValue::begin()
{
    if (jvalue_)
    {
        if (jvalue_->IsArray())
        {
            rapidjson::Value::ValueIterator it = jvalue_->Begin();
            return DataIterator(jdoc_, it);
        }
        else
        {
            DN_CORE_WARN("JSONValue not array!");
        }
    }
    DN_CORE_WARN("JSONValue is empty!");
    return DataIterator();
}

/**
 * @brief Returns iterator to end of array (mutable).
 * @return DataIterator to end.
 */
JSONValue::DataIterator JSONValue::end()
{
    if (jvalue_)
    {
        if (jvalue_->IsArray())
        {
            rapidjson::Value::ValueIterator it = jvalue_->End();
            return DataIterator(jdoc_, it);
        }
        else
        {
            DN_CORE_WARN("JSONValue not array!");
        }
    }
    DN_CORE_WARN("JSONValue is empty!");
    return DataIterator();
}

/**
 * @brief Returns iterator to beginning of array (const).
 * @return ConstDataIterator to beginning.
 */
JSONValue::ConstDataIterator JSONValue::begin() const
{
    if (jvalue_)
    {
        if (jvalue_->IsArray())
        {
            rapidjson::Value::ConstValueIterator it = jvalue_->Begin();
            return ConstDataIterator(jdoc_, it);
        }
        else
        {
            DN_CORE_WARN("JSONValue not array!");
        }
    }
    DN_CORE_WARN("JSONValue is empty!");
    return ConstDataIterator();
}

/**
 * @brief Returns iterator to end of array (const).
 * @return ConstDataIterator to end.
 */
JSONValue::ConstDataIterator JSONValue::end() const
{
    if (jvalue_)
    {
        if (jvalue_->IsArray())
        {
            rapidjson::Value::ConstValueIterator it = jvalue_->End();
            return ConstDataIterator(jdoc_, it);
        }
        else
        {
            DN_CORE_WARN("JSONValue not array!");
        }
    }
    DN_CORE_WARN("JSONValue is empty!");
    return ConstDataIterator();
}

/**
 * @brief Finds a member in the object by name.
 * @param member Member name.
 * @return ConstDataIterator to member, or end if not found.
 */
JSONValue::ConstDataIterator JSONValue::FindMember(const std::string &member) const
{
    if (jvalue_)
    {
        if (jvalue_->IsObject())
        {
            auto mit = jvalue_->FindMember(member.c_str());
            if (mit != jvalue_->MemberEnd())
            {
                // FindMember returns a member iterator, but ConstDataIterator expects value iterator
                // Since these are incompatible types, we return the value as a ConstDataIterator
                // pointing to the member's value
                return ConstDataIterator(jdoc_, &mit->value);
            }
            DN_CORE_TRACE("Member '{}' not found in object", member);
            return ConstDataIterator();
        }
        else
        {
            DN_CORE_WARN("JSONValue is not an object, cannot find member!");
        }
    }
    DN_CORE_WARN("JSONValue is empty!");
    return ConstDataIterator();
}

/**
 * @brief Sets this value to an object type.
 * @return Reference to this JSONValue.
 */
JSONValue JSONValue::SetObject()
{
    jvalue_->SetObject();
    return *this;
}

/**
 * @brief Sets this value to an integer.
 * @param x Integer value.
 * @return Reference to this JSONValue.
 */
JSONValue JSONValue::SetInt(int x)
{
    jvalue_->SetInt(x);
    return *this;
}

/**
 * @brief Sets this value to a string.
 * @param text String value.
 * @return Reference to this JSONValue.
 */
JSONValue JSONValue::SetString(const std::string &text)
{
    jvalue_->SetString(text.c_str(), static_cast<rapidjson::SizeType>(text.size()), jdoc_->GetAllocator());
    return *this;
}

/**
 * @brief Sets this value to a double.
 * @param x Double value.
 * @return Reference to this JSONValue.
 */
JSONValue JSONValue::SetDouble(double x)
{
    jvalue_->SetDouble(x);
    return *this;
}

/**
 * @brief Sets this value to a boolean.
 * @param b Boolean value.
 * @return Reference to this JSONValue.
 */
JSONValue JSONValue::SetBool(bool b)
{
    jvalue_->SetBool(b);
    return *this;
}

/**
 * @brief Sets this value to an array type.
 * @return Reference to this JSONValue.
 */
JSONValue JSONValue::SetArray()
{
    jvalue_->SetArray();
    return *this;
}

/**
 * @brief Adds a shallow copy of dv to array-type JSONValue.
 * @param dv Value to add.
 * @return Reference to this JSONValue.
 */
JSONValue &JSONValue::PushBack(JSONValue dv)
{
    if (!jvalue_->IsArray())
    {
        DN_CORE_WARN("JSONValue is not an array!");
        return *this;
    }
    auto &alloc = jdoc_->GetAllocator();
    rapidjson::Value node;
    node.CopyFrom(dv.GetRJSONValue(), alloc);
    jvalue_->PushBack(std::move(node), alloc);
    return *this;
}

/**
 * @brief Accesses a member by name.
 * @param member Member name.
 * @return JSONValue of member, or empty if not found.
 */
JSONValue JSONValue::operator[](const std::string &member)
{
    if (IsReadValid())
    {
        if (!jvalue_->IsObject())
        {
            DN_CORE_WARN("JSONValue is not an Object!");
            return JSONValue();
        }
        if (!jvalue_->HasMember(member.c_str()))
        {
            DN_CORE_WARN("JSONValue does not contain member {}!", member);
            return JSONValue();
        }
        rapidjson::Value &value = (*jvalue_)[member.c_str()];
        return JSONValue(jdoc_, &value);
    }
    DN_CORE_WARN("JSONValue is empty!");
    return JSONValue();
}

/**
 * @brief Accesses an array element by index.
 * @param idx Array index.
 * @return JSONValue of element, or empty if not found.
 */
JSONValue JSONValue::operator[](int idx)
{
    if (IsReadValid())
    {
        if (jvalue_->IsArray())
        {
            rapidjson::SizeType rjIdx = static_cast<rapidjson::SizeType>(idx);
            rapidjson::Value &value = (*jvalue_)[rjIdx];
            return JSONValue(jdoc_, &value);
        }
        else
        {
            DN_CORE_WARN("JSONValue not array, cannot dereference!");
            return JSONValue();
        }
    };
    DN_CORE_WARN("JSONValue is empty!");
    return JSONValue();
}

/**
 * @brief Dereference operator.
 * @return Reference to this JSONValue.
 */
const JSONValue &JSONValue::operator*() const
{
    if (jdoc_ && jvalue_)
    {
        return *this;
    }
    DN_CORE_WARN("ConstDataIterator invalid, cannot dereference!");
    return *this;
}

/**
 * @brief Equality comparison operator.
 * @param other JSONValue to compare with.
 * @return True if equal.
 */
bool JSONValue::operator==(const JSONValue &other) const
{
    if (IsReadValid())
    {
        if (other.IsReadValid())
        {
            return jvalue_ == other.jvalue_;
        }
        DN_CORE_WARN("Second JSONValue invalid, cannot test equality!");
        return false;
    }
    DN_CORE_WARN("First JSONValue invalid, cannot test equality!");
    return false;
}

/**
 * @brief Inequality comparison operator.
 * @param other JSONValue to compare with.
 * @return True if not equal.
 */
bool JSONValue::operator!=(const JSONValue &other) const
{
    if (IsReadValid())
    {
        if (other.IsReadValid())
        {
            return jvalue_ != other.jvalue_;
        }
        DN_CORE_WARN("Second JSONValue invalid, cannot test inequality!");
        return false;
    }
    DN_CORE_WARN("First JSONValue invalid, cannot test inequality!");
    return false;
}

/* ITERATOR */
/**
 * @class DataIterator
 * @brief Iterator for mutable traversal of JSON arrays.
 */
JSONValue::DataIterator::DataIterator() : isReadValid(false), readDocument_(nullptr)
{
}

/**
 * @brief Internal constructor for valid iterator.
 * @param document Shared pointer to RapidJSON document.
 * @param it RapidJSON value iterator.
 */
JSONValue::DataIterator::DataIterator(std::shared_ptr<rapidjson::Document> document, rapidjson::Value::ValueIterator it)
    : isReadValid(true), readDocument_(document), it_(it)
{
}

/**
 * @brief Gets the JSONValue at the current iterator position.
 * @return JSONValue at iterator position.
 */
JSONValue JSONValue::DataIterator::GetValue()
{
    if (isReadValid)
    {
        JSONValue value(readDocument_, it_);
        return value;
    }
    DN_CORE_WARN("DataIterator not valid!");
    return JSONValue();
}

/**
 * @brief Dereference operator.
 * @return JSONValue at iterator position.
 */
JSONValue JSONValue::DataIterator::operator*() const
{
    if (isReadValid && readDocument_)
    {
        return JSONValue(readDocument_, &(*it_));
    }
    DN_CORE_WARN("DataIterator invalid, cannot dereference!");
    return JSONValue();
}

/**
 * @brief Prefix increment operator.
 * @return Reference to incremented iterator.
 */
JSONValue::DataIterator &JSONValue::DataIterator::operator++()
{
    if (isReadValid && readDocument_)
    {
        ++it_;
        return *this;
    }
    DN_CORE_WARN("DataIterator invalid, cannot increment!");
    return *this;
}

/**
 * @brief Equality comparison operator.
 * @param other Iterator to compare with.
 * @return True if iterators are equal.
 */
bool JSONValue::DataIterator::operator==(const DataIterator other)
{
    if (isReadValid && readDocument_)
    {
        if (other.isReadValid && other.readDocument_)
        {
            return it_ == other.it_;
        }
        DN_CORE_WARN("Second DataIterator invalid, cannot test equality!");
        return false;
    }
    DN_CORE_WARN("First DataIterator invalid, cannot test equality!");
    return false;
}

/**
 * @brief Inequality comparison operator.
 * @param other Iterator to compare with.
 * @return True if iterators are not equal.
 */
bool JSONValue::DataIterator::operator!=(const DataIterator other)
{
    if (isReadValid && readDocument_)
    {
        if (other.isReadValid && other.readDocument_)
        {
            return it_ != other.it_;
        }
        DN_CORE_WARN("Second DataIterator invalid, cannot test equality!");
        return false;
    }
    DN_CORE_WARN("First DataIterator invalid, cannot test equality!");
    return false;
}

/* CONSTIT */
/**
 * @class ConstDataIterator
 * @brief Iterator for const traversal of JSON arrays.
 */
JSONValue::ConstDataIterator::ConstDataIterator() : isReadValid(false), readDocument_(nullptr)
{
}

/**
 * @brief Internal constructor for valid iterator.
 * @param document Shared pointer to RapidJSON document.
 * @param it RapidJSON const value iterator.
 */
JSONValue::ConstDataIterator::ConstDataIterator(std::shared_ptr<rapidjson::Document> document,
                                                const rapidjson::Value::ConstValueIterator it)
    : isReadValid(true), readDocument_(document), it_(it)
{
}

/**
 * @brief Gets the JSONValue at the current iterator position.
 * @return JSONValue at iterator position.
 */
const JSONValue JSONValue::ConstDataIterator::GetValue()
{
    if (isReadValid)
    {
        const JSONValue value(readDocument_, it_);
        return value;
    }
    DN_CORE_WARN("ConstDataIterator not valid!");
    return JSONValue();
}

/**
 * @brief Dereference operator.
 * @return JSONValue at iterator position.
 */
JSONValue JSONValue::ConstDataIterator::operator*() const
{
    if (isReadValid && readDocument_)
    {
        return JSONValue(readDocument_, &(*it_));
    }
    DN_CORE_WARN("ConstDataIterator invalid, cannot dereference!");
    return JSONValue();
}

/**
 * @brief Prefix increment operator.
 * @return Incremented iterator.
 */
JSONValue::ConstDataIterator JSONValue::ConstDataIterator::operator++()
{
    if (isReadValid && readDocument_)
    {
        ++it_;
        return *this;
    }
    DN_CORE_WARN("ConstDataIterator invalid, cannot increment!");
    return *this;
}

/**
 * @brief Equality comparison operator.
 * @param other Iterator to compare with.
 * @return True if iterators are equal.
 */
bool JSONValue::ConstDataIterator::operator==(const ConstDataIterator other)
{
    if (isReadValid && readDocument_)
    {
        if (other.isReadValid && other.readDocument_)
        {
            return it_ == other.it_;
        }
        DN_CORE_WARN("Second ConstDataIterator invalid, cannot test equality!");
        return false;
    }
    DN_CORE_WARN("First ConstDataIterator invalid, cannot test equality!");
    return false;
}

/**
 * @brief Inequality comparison operator.
 * @param other Iterator to compare with.
 * @return True if iterators are not equal.
 */
bool JSONValue::ConstDataIterator::operator!=(const ConstDataIterator other)
{
    if (isReadValid && readDocument_)
    {
        if (other.isReadValid && other.readDocument_)
        {
            return it_ != other.it_;
        }
        DN_CORE_WARN("Second ConstDataIterator invalid, cannot test equality!");
        return false;
    }
    DN_CORE_WARN("First ConstDataIterator invalid, cannot test equality!");
    return false;
}
} // namespace duin
