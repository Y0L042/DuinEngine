#include "dnpch.h"
#include "DataValue.h"
#include <Duin/Core/Debug/DNLog.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <sstream>
#include <fstream>

namespace duin {
    /* DATAVALUE */
    JSONValue JSONValue::Parse(const std::string& filePath)
    {
        DN_CORE_INFO("Attempting to parse {} into JSONValue...", filePath);
        JSONValue dv;

        std::ifstream ifs(filePath);
        if (!ifs) {
            DN_CORE_FATAL("Cannot open {}!", filePath);
        }
        std::stringstream buffer;
        buffer << ifs.rdbuf();
        std::string content = buffer.str();
        DN_CORE_INFO("Content:\n{}", content);

        dv.jdoc_ =  std::make_shared<rapidjson::Document>();
        dv.jdoc_->Parse(content.c_str());
        dv.jvalue_ = dv.jdoc_.get();

        if (dv.jvalue_->IsObject()) {
            DN_CORE_INFO("Document parsed into JSONValue.");
        } else {
            DN_CORE_WARN("Document parsing failed, JSONValue not an Object!");
            dv.SetObject(); // Ensure that value is valid Object, even when parsing invalid files
        }

        return dv;
    }

    std::string JSONValue::Write(const JSONValue& value, bool prettyWrite)
    {
        rapidjson::StringBuffer buffer;

        if (prettyWrite) {
            rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
            writer.SetIndent('\t', 1);

            // rapidjson::Document& d = *value.jdoc_;
            // d.Accept(writer);
            value.jvalue_->Accept(writer);
            std::string out = buffer.GetString();

            return out;
        } else {
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

            // rapidjson::Document& d = *value.jdoc_;
            // d.Accept(writer);
            value.jvalue_->Accept(writer);
            std::string out = buffer.GetString();

            return out;
        }


    }

    JSONValue JSONValue::Invalid()
    {
        return JSONValue();
    }

    JSONValue::JSONValue()
    {
        jdoc_ = std::make_shared<rapidjson::Document>();
        jdoc_->SetObject();
        jvalue_ = jdoc_.get();
    }

    JSONValue::JSONValue(bool b)
        : JSONValue()
    {
        jvalue_->SetBool(b);
    }

    JSONValue::JSONValue(int x)
        : JSONValue()
    {
        jvalue_->SetInt(x);
    }

    JSONValue::JSONValue(const std::string& text)
        : JSONValue()
    {
        jvalue_->SetString(text.c_str(), static_cast<rapidjson::SizeType>(text.size()));
    }

    JSONValue::JSONValue(double x)
        : JSONValue()
    {
        jvalue_->SetDouble(x);
    }

    JSONValue::JSONValue(std::shared_ptr<rapidjson::Document> document, rapidjson::Value *value)
        : jdoc_(document), jvalue_(value)
    {
        // If no subvalue is passed, use document as value
        if (jvalue_ == nullptr) {
            jvalue_ = jdoc_.get();
        }
    }

    JSONValue::JSONValue(std::shared_ptr<rapidjson::Document> document, const rapidjson::Value* value)
        : jdoc_(document), jvalue_(const_cast<rapidjson::Value*>(value)) // Cast const to non-const
    {
        // If no subvalue is passed, use document as value
        if (jvalue_ == nullptr) {
            jvalue_ = jdoc_.get();
            DN_CORE_ERROR("Cannot create JSONValue with const nullptr rapidjson::Value!");
        }
    }

    std::string JSONValue::Write() const
    {
        return JSONValue::Write(*this);
    }

    JSONValue JSONValue::Clone() const
    {
        auto newDoc = std::make_shared<rapidjson::Document>();
        newDoc->CopyFrom(*jvalue_, newDoc->GetAllocator()); 

        return JSONValue(newDoc, newDoc.get());
    }

    bool JSONValue::IsReadValid() const
    {
        if (jdoc_ && jvalue_) {
            return true;
        } else {
            return false;
        }
        DN_CORE_WARN("JSONValue is empty!");
        return false;
    }

    bool JSONValue::HasMember(const std::string& member) const
    {
        if (jvalue_) {
            if (!jvalue_->IsObject()) {
                DN_CORE_WARN("JSONValue is not an Object! Cannot search for key.");
                return false;
            } else
            if (jvalue_->HasMember(member.c_str())) {
                return true;
            } else {
                return false;
            }
        }
        DN_CORE_WARN("JSONValue is empty!");
        return false;
    }

    JSONValue& JSONValue::AddMember(const std::string& key, JSONValue dv, bool allowDuplicates)
    {
        if (!jvalue_->IsObject()) {
            DN_CORE_WARN("JSONValue not an Object, cannot add nested member!");
            return *this;
        }

        if (!allowDuplicates) {
            RemoveMember(key.c_str());
        }

        rapidjson::Value name(
            key.c_str(),
            static_cast<rapidjson::SizeType>(key.size()),
            jdoc_->GetAllocator());

        rapidjson::Value nodeCopy;
        nodeCopy.CopyFrom(dv.GetRJSONValue(), jdoc_->GetAllocator());

        jvalue_->AddMember(std::move(name), std::move(nodeCopy), jdoc_->GetAllocator());

        return *this;
    }

    JSONValue& JSONValue::RemoveMember(const std::string& keyStr)
    {
        if (jvalue_ && jvalue_->IsObject() && jvalue_->HasMember(keyStr.c_str())) {
            jvalue_->RemoveMember(keyStr.c_str());
        }

        return *this;
    }

    bool JSONValue::IsValid()
    {
        if (jdoc_ && jvalue_) {
            return true;
        } else {
            return false;
        }
        DN_CORE_WARN("JSONValue is empty!");
        return false;
	}

    bool JSONValue::IsNull() const
    {
        if (jvalue_) {
            bool v = jvalue_->IsNull();
            return v;
        }
        DN_CORE_WARN("JSONValue is empty!");
        return false;
    }

    bool JSONValue::IsObject() const
    {
        if (jvalue_) {
            bool v = jvalue_->IsObject();
            return v;
        }
        DN_CORE_WARN("JSONValue is empty!");
        return false;
    }

    bool JSONValue::IsString() const
    {
        if (jvalue_) {
            bool v = jvalue_->IsString();
            return v;
        }
        DN_CORE_WARN("JSONValue is empty!");
        return false;
    }

    std::string JSONValue::GetString() const
    {
        if (jvalue_) {
            if (jvalue_->IsString()) {
                std::string s = jvalue_->GetString();
                return s;
            } else {
                DN_CORE_WARN("JSONValue not string!");
                return std::string();
            }
        }
        DN_CORE_WARN("JSONValue is empty!");
        return std::string();
    }

    bool JSONValue::IsBool() const
    {
        if (jvalue_) {
            bool v = jvalue_->IsBool();
            return v;
        }
        DN_CORE_WARN("JSONValue is empty!");
        return false;
    }

    bool JSONValue::GetBool() const
    {
        if (jvalue_) {
            if (jvalue_->IsBool()) {
                bool v = jvalue_->GetBool();
                return v;
            } else {
                DN_CORE_WARN("JSONValue not boolean!");
            }
        }
        DN_CORE_WARN("JSONValue is empty!");
        return false;
    }

    bool JSONValue::IsNumber() const
    {
        if (jvalue_) {
            bool v = jvalue_->IsNumber();
            return v;
        }
        DN_CORE_WARN("JSONValue is empty!");
        return false;
    }

    bool JSONValue::IsInt() const
    {
        if (jvalue_) {
            bool v = jvalue_->IsInt();
            return v;
        }
        DN_CORE_WARN("JSONValue is empty!");
        return false;
    }

    int JSONValue::GetInt() const
    {
        if (jvalue_) {
            if (jvalue_->IsInt()) {
                int v = jvalue_->GetInt();
                return v;
            } else {
                DN_CORE_WARN("JSONValue not integer!");
            }
        }
        DN_CORE_WARN("JSONValue is empty!");
        return 0;
    }

    bool JSONValue::IsDouble() const
    {
        if (jvalue_) {
            bool v = jvalue_->IsDouble();
            return v;
        }
        DN_CORE_WARN("JSONValue is empty!");
        return false;
    }

    double JSONValue::GetDouble() const
    {
        if (jvalue_) {
            if (jvalue_->IsDouble()) {
                double v = jvalue_->GetDouble();
                return v;
            } else {
                DN_CORE_WARN("JSONValue not double!");
            }
        }
        DN_CORE_WARN("JSONValue is empty!");
        return 0.0;
    }

    bool JSONValue::IsArray() const
    {
        if (jvalue_) {
            bool v = jvalue_->IsArray();
            return v;
        }
        DN_CORE_WARN("JSONValue is empty!");
        return false;
    }

    bool JSONValue::Empty() const
    {
        if (jvalue_) {
            if (jvalue_->IsArray()) {
                bool v = jvalue_->Empty();
                return v;
            } else {
                DN_CORE_WARN("JSONValue not array!");
            }
        }
        DN_CORE_WARN("JSONValue is empty!");
        return false;
    }

    size_t JSONValue::Capacity() const
    {
        if (jvalue_) {
            if (jvalue_->IsArray()) {
                rapidjson::SizeType v = jvalue_->Capacity();
                size_t size = (size_t)v;
                return size_t();
            } else {
                DN_CORE_WARN("JSONValue not array!");
            }
        }
        DN_CORE_WARN("JSONValue is empty!");
        return size_t();
    }

    JSONValue::ConstDataIterator JSONValue::Begin()
    {
        if (jvalue_) {
            if (jvalue_->IsArray()) {
                rapidjson::Value::ConstValueIterator it = jvalue_->Begin();
                return ConstDataIterator(jdoc_, it);
            } else {
                DN_CORE_WARN("JSONValue not array!");
            }
        }
        DN_CORE_WARN("JSONValue is empty!");
        return ConstDataIterator();
    }

    JSONValue::ConstDataIterator JSONValue::End()
    {
        if (jvalue_) {
            if (jvalue_->IsArray()) {
                rapidjson::Value::ConstValueIterator it = jvalue_->End();
                return ConstDataIterator(jdoc_, it);
            } else {
                DN_CORE_WARN("JSONValue not array!");
            }
        }
        DN_CORE_WARN("JSONValue is empty!");
        return ConstDataIterator();
    }

    JSONValue::ConstDataIterator JSONValue::FindMember(const std::string& member) const
    {
        return ConstDataIterator();
    }

    JSONValue JSONValue::SetObject()
    {
        jvalue_->SetObject();
        return *this;
    }

    JSONValue JSONValue::SetInt(int x)
    {
        jvalue_->SetInt(x);
        return *this;
    }

    JSONValue JSONValue::SetString(const std::string& text)
    {
        jvalue_->SetString(text.c_str(), static_cast<rapidjson::SizeType>(text.size()));
        return *this;
    }

    JSONValue JSONValue::SetDouble(double x)
    {
        jvalue_->SetDouble(x);
        return *this;
    }

    JSONValue JSONValue::SetBool(bool b)
    {
        jvalue_->SetBool(b);
        return *this;
    }

    JSONValue JSONValue::SetArray()
    {
        jvalue_->SetArray();
        return *this;
    }

    JSONValue& JSONValue::PushBack(JSONValue dv)
    {
        if (!jvalue_->IsArray()) {
            DN_CORE_WARN("JSONValue is not an array!");
            return *this;
        }
        auto& alloc = jdoc_->GetAllocator();
        rapidjson::Value node;
        node.CopyFrom(dv.GetRJSONValue(), alloc);            
        jvalue_->PushBack(std::move(node), alloc); 
        //jvalue_->PushBack(*dv.jvalue_, alloc);
        return *this;
    }

    JSONValue JSONValue::operator[](const std::string& member)
    {
        if (IsReadValid()) {
            if (!jvalue_->IsObject()) {
                DN_CORE_WARN("JSONValue is not an Object!");
                return JSONValue();
            }
            if (!jvalue_->HasMember(member.c_str())) {
                DN_CORE_WARN("JSONValue does not contain member {}!", member);
                return JSONValue();
            }
            rapidjson::Value& value = (*jvalue_)[member.c_str()];
            return JSONValue(jdoc_, &value);
        }
        DN_CORE_WARN("JSONValue is empty!");
        return JSONValue();
    }

    JSONValue JSONValue::operator[](int idx)
    {
        if (IsReadValid()) {
            if (jvalue_->IsArray()) {
                rapidjson::SizeType rjIdx = static_cast<rapidjson::SizeType>(idx);
                rapidjson::Value& value = jvalue_[rjIdx];
                return JSONValue(jdoc_, &value);
            } else {
                DN_CORE_WARN("JSONValue not array, cannot dereference!");
                return JSONValue();
            }
        };
        DN_CORE_WARN("JSONValue is empty!");
        return JSONValue();
    }

    const JSONValue& JSONValue::operator*() const
    {
        if (jdoc_ && jvalue_) {
            return *this;
        }
        DN_CORE_WARN("ConstDataIterator invalid, cannot dereference!");
        return *this;
    }

    bool JSONValue::operator==(const JSONValue& other)
    {
        if (IsReadValid()) {
            if (other.IsReadValid()) {
                return jvalue_ == other.jvalue_;
            }
            DN_CORE_WARN("Second JSONValue invalid, cannot test equality!");
            return false;
        }
        DN_CORE_WARN("First JSONValue invalid, cannot test equality!");
        return false;
    }

    bool JSONValue::operator!=(const JSONValue& other)
    {
        if (IsReadValid()) {
            if (other.IsReadValid()) {
                return jvalue_ != other.jvalue_;
            }
            DN_CORE_WARN("Second JSONValue invalid, cannot test inequality!");
            return false;
        }
        DN_CORE_WARN("First JSONValue invalid, cannot test inequality!");
        return false;
    }





    /* ITERATOR */
    JSONValue::DataIterator::DataIterator()
        : isReadValid(false), readDocument_(nullptr)
    {}

    JSONValue::DataIterator::DataIterator(std::shared_ptr<rapidjson::Document> document, rapidjson::Value::ValueIterator it)
        : isReadValid(true), readDocument_(document), it_(it)
    {}

    JSONValue JSONValue::DataIterator::GetValue()
    {
        if (isReadValid) {
            JSONValue value(readDocument_, it_);
            return value;
        }
        DN_CORE_WARN("DataIterator not valid!");
        return JSONValue();
    }

    JSONValue JSONValue::DataIterator::operator*() const
    {
        if (isReadValid && readDocument_) {
            return JSONValue(readDocument_, &(*it_));
        }
        DN_CORE_WARN("DataIterator invalid, cannot dereference!");
        return JSONValue();
    }

    JSONValue::DataIterator& JSONValue::DataIterator::operator++()
    {
        if (isReadValid && readDocument_) {
            ++it_;
            return *this;
        }
        DN_CORE_WARN("DataIterator invalid, cannot increment!");
        return *this;
    }

    bool JSONValue::DataIterator::operator==(const DataIterator other)
    {
        if (isReadValid && readDocument_) {
            if (other.isReadValid && other.readDocument_) {
                return it_ == other.it_;
            }
            DN_CORE_WARN("Second DataIterator invalid, cannot test equality!");
            return false;
        }
        DN_CORE_WARN("First DataIterator invalid, cannot test equality!");
        return false;
    }

    bool JSONValue::DataIterator::operator!=(const DataIterator other)
    {
        if (isReadValid && readDocument_) {
            if (other.isReadValid && other.readDocument_) {
                return it_ != other.it_;
            }
            DN_CORE_WARN("Second DataIterator invalid, cannot test equality!");
            return false;
        }
        DN_CORE_WARN("First DataIterator invalid, cannot test equality!");
        return false;
    }




    /* CONSTIT */
    JSONValue::ConstDataIterator::ConstDataIterator()
        : isReadValid(false), readDocument_(nullptr)
    {}

    JSONValue::ConstDataIterator::ConstDataIterator(std::shared_ptr<rapidjson::Document> document, const rapidjson::Value::ConstValueIterator it)
        : isReadValid(true), readDocument_(document), it_(it)
    {}

    const JSONValue JSONValue::ConstDataIterator::GetValue()
    {
        if (isReadValid) {
            const JSONValue value(readDocument_, it_);
            return value;
        }
        DN_CORE_WARN("ConstDataIterator not valid!");
        return JSONValue();
    }

    JSONValue JSONValue::ConstDataIterator::operator*() const
    {
        if (isReadValid && readDocument_) {
            return JSONValue(readDocument_, &(*it_));
        }
        DN_CORE_WARN("ConstDataIterator invalid, cannot dereference!");
        return JSONValue();
    }

    JSONValue::ConstDataIterator JSONValue::ConstDataIterator::operator++()
    {
        if (isReadValid && readDocument_) {
            ++it_;
            return *this;
        }
        DN_CORE_WARN("ConstDataIterator invalid, cannot increment!");
        return *this;
    }

    bool JSONValue::ConstDataIterator::operator==(const ConstDataIterator other)
    {
        if (isReadValid && readDocument_) {
            if (other.isReadValid && other.readDocument_) {
                return it_ == other.it_;
            }
            DN_CORE_WARN("Second ConstDataIterator invalid, cannot test equality!");
            return false;
        }
        DN_CORE_WARN("First ConstDataIterator invalid, cannot test equality!");
        return false;
    }

    bool JSONValue::ConstDataIterator::operator!=(const ConstDataIterator other)
    {
        if (isReadValid && readDocument_) {
            if (other.isReadValid && other.readDocument_) {
                return it_ != other.it_;
            }
            DN_CORE_WARN("Second ConstDataIterator invalid, cannot test equality!");
            return false;
        }
        DN_CORE_WARN("First ConstDataIterator invalid, cannot test equality!");
        return false;
    }
}
