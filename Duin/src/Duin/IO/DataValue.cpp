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
    DataValue DataValue::Parse(const std::string& filePath)
    {
        DN_CORE_INFO("Attempting to parse {} into DataValue...", filePath);
        DataValue dv;

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
            DN_CORE_INFO("Document parsed into DataValue.");
        } else {
            DN_CORE_WARN("Document parsing failed, DataValue not an Object!");
        }
        return dv;
    }

    std::string DataValue::Write(const DataValue& value, bool prettyWrite)
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

    DataValue::DataValue()
    {
        jdoc_ = std::make_shared<rapidjson::Document>();
        jdoc_->SetObject();
        jvalue_ = jdoc_.get();
    }

    DataValue::DataValue(bool b)
        : DataValue()
    {
        jvalue_->SetBool(b);
    }

    DataValue::DataValue(int x)
        : DataValue()
    {
        jvalue_->SetInt(x);
    }

    DataValue::DataValue(const std::string& text)
        : DataValue()
    {
        jvalue_->SetString(text.c_str(), text.size());
    }

    DataValue::DataValue(double x)
        : DataValue()
    {
        jvalue_->SetDouble(x);
    }

    DataValue::DataValue(std::shared_ptr<rapidjson::Document> document, rapidjson::Value *value)
        : jdoc_(document), jvalue_(value)
    {
        // If no subvalue is passed, use document as value
        if (jvalue_ == nullptr) {
            jvalue_ = jdoc_.get();
        }
    }

    DataValue::DataValue(std::shared_ptr<rapidjson::Document> document, const rapidjson::Value* value)
        : jdoc_(document), jvalue_(const_cast<rapidjson::Value*>(value)) // Cast const to non-const
    {
        // If no subvalue is passed, use document as value
        if (jvalue_ == nullptr) {
            jvalue_ = jdoc_.get();
            DN_CORE_ERROR("Cannot create DataValue with const nullptr rapidjson::Value!");
        }
    }

    std::string DataValue::Write() const
    {
        return DataValue::Write(*this);
    }

    DataValue DataValue::Clone() const
    {
        auto newDoc = std::make_shared<rapidjson::Document>();
        newDoc->CopyFrom(*jvalue_, newDoc->GetAllocator()); 

        return DataValue(newDoc, newDoc.get());
    }

    bool DataValue::IsReadValid() const
    {
        if (jdoc_ && jvalue_) {
            return true;
        } else {
            return false;
        }
        DN_CORE_WARN("DataValue is empty!");
        return false;
    }

    bool DataValue::HasMember(const std::string& member) const
    {
        if (jvalue_) {
            if (!jvalue_->IsObject()) {
                DN_CORE_WARN("DataValue is not an Object! Cannot search for key.");
                return false;
            } else
            if (jvalue_->HasMember(member.c_str())) {
                return true;
            } else {
                return false;
            }
        }
        DN_CORE_WARN("DataValue is empty!");
        return false;
    }

    DataValue& DataValue::AddMember(const std::string& key, DataValue dv, bool allowDuplicates)
    {
        if (!jvalue_->IsObject()) {
            DN_CORE_WARN("DataValue not an Object, cannot add nested member!");
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

    DataValue& DataValue::RemoveMember(const std::string& keyStr)
    {
        if (jvalue_ && jvalue_->IsObject() && jvalue_->HasMember(keyStr.c_str())) {
            jvalue_->RemoveMember(keyStr.c_str());
        }

        return *this;
    }

    bool DataValue::IsNull() const
    {
        if (jvalue_) {
            bool v = jvalue_->IsNull();
            return v;
        }
        DN_CORE_WARN("DataValue is empty!");
        return false;
    }

    bool DataValue::IsObject() const
    {
        if (jvalue_) {
            bool v = jvalue_->IsObject();
            return v;
        }
        DN_CORE_WARN("DataValue is empty!");
        return false;
    }

    bool DataValue::IsString() const
    {
        if (jvalue_) {
            bool v = jvalue_->IsString();
            return v;
        }
        DN_CORE_WARN("DataValue is empty!");
        return false;
    }

    std::string DataValue::GetString() const
    {
        if (jvalue_) {
            if (jvalue_->IsString()) {
                std::string s = jvalue_->GetString();
                return s;
            } else {
                DN_CORE_WARN("DataValue not string!");
                return std::string();
            }
        }
        DN_CORE_WARN("DataValue is empty!");
        return std::string();
    }

    bool DataValue::IsBool() const
    {
        if (jvalue_) {
            bool v = jvalue_->IsBool();
            return v;
        }
        DN_CORE_WARN("DataValue is empty!");
        return false;
    }

    bool DataValue::GetBool() const
    {
        if (jvalue_) {
            if (jvalue_->IsBool()) {
                bool v = jvalue_->GetBool();
                return v;
            } else {
                DN_CORE_WARN("DataValue not boolean!");
            }
        }
        DN_CORE_WARN("DataValue is empty!");
        return false;
    }

    bool DataValue::IsNumber() const
    {
        if (jvalue_) {
            bool v = jvalue_->IsNumber();
            return v;
        }
        DN_CORE_WARN("DataValue is empty!");
        return false;
    }

    bool DataValue::IsInt() const
    {
        if (jvalue_) {
            bool v = jvalue_->IsInt();
            return v;
        }
        DN_CORE_WARN("DataValue is empty!");
        return false;
    }

    int DataValue::GetInt() const
    {
        if (jvalue_) {
            if (jvalue_->IsInt()) {
                int v = jvalue_->GetInt();
                return v;
            } else {
                DN_CORE_WARN("DataValue not integer!");
            }
        }
        DN_CORE_WARN("DataValue is empty!");
        return 0;
    }

    bool DataValue::IsDouble() const
    {
        if (jvalue_) {
            bool v = jvalue_->IsDouble();
            return v;
        }
        DN_CORE_WARN("DataValue is empty!");
        return false;
    }

    double DataValue::GetDouble() const
    {
        if (jvalue_) {
            if (jvalue_->IsDouble()) {
                double v = jvalue_->GetDouble();
                return v;
            } else {
                DN_CORE_WARN("DataValue not double!");
            }
        }
        DN_CORE_WARN("DataValue is empty!");
        return 0.0;
    }

    bool DataValue::IsArray() const
    {
        if (jvalue_) {
            bool v = jvalue_->IsArray();
            return v;
        }
        DN_CORE_WARN("DataValue is empty!");
        return false;
    }

    bool DataValue::Empty() const
    {
        if (jvalue_) {
            if (jvalue_->IsArray()) {
                bool v = jvalue_->Empty();
                return v;
            } else {
                DN_CORE_WARN("DataValue not array!");
            }
        }
        DN_CORE_WARN("DataValue is empty!");
        return false;
    }

    size_t DataValue::Capacity() const
    {
        if (jvalue_) {
            if (jvalue_->IsArray()) {
                rapidjson::SizeType v = jvalue_->Capacity();
                size_t size = (size_t)v;
                return size_t();
            } else {
                DN_CORE_WARN("DataValue not array!");
            }
        }
        DN_CORE_WARN("DataValue is empty!");
        return size_t();
    }

    DataValue::ConstDataIterator DataValue::Begin()
    {
        if (jvalue_) {
            if (jvalue_->IsArray()) {
                rapidjson::Value::ConstValueIterator it = jvalue_->Begin();
                return ConstDataIterator(jdoc_, it);
            } else {
                DN_CORE_WARN("DataValue not array!");
            }
        }
        DN_CORE_WARN("DataValue is empty!");
        return ConstDataIterator();
    }

    DataValue::ConstDataIterator DataValue::End()
    {
        if (jvalue_) {
            if (jvalue_->IsArray()) {
                rapidjson::Value::ConstValueIterator it = jvalue_->End();
                return ConstDataIterator(jdoc_, it);
            } else {
                DN_CORE_WARN("DataValue not array!");
            }
        }
        DN_CORE_WARN("DataValue is empty!");
        return ConstDataIterator();
    }

    DataValue::ConstDataIterator DataValue::FindMember(const std::string& member) const
    {
        return ConstDataIterator();
    }

    DataValue DataValue::SetObject()
    {
        jvalue_->SetObject();
        return *this;
    }

    DataValue DataValue::SetInt(int x)
    {
        jvalue_->SetInt(x);
        return *this;
    }

    DataValue DataValue::SetString(const std::string& text)
    {
        jvalue_->SetString(text.c_str(), text.size());
        return *this;
    }

    DataValue DataValue::SetDouble(double x)
    {
        jvalue_->SetDouble(x);
        return *this;
    }

    DataValue DataValue::SetBool(bool b)
    {
        jvalue_->SetBool(b);
        return *this;
    }

    DataValue DataValue::SetArray()
    {
        jvalue_->SetArray();
        return *this;
    }

    DataValue& DataValue::PushBack(DataValue dv)
    {
        if (!jvalue_->IsArray()) {
            DN_CORE_WARN("DataValue is not an array!");
            return *this;
        }
        auto& alloc = jdoc_->GetAllocator();
        rapidjson::Value node;
        node.CopyFrom(dv.GetRJSONValue(), alloc);            
        jvalue_->PushBack(std::move(node), alloc); 
        //jvalue_->PushBack(*dv.jvalue_, alloc);
        return *this;
    }

    DataValue DataValue::operator[](const std::string& member)
    {
        if (IsReadValid()) {
            if (!jvalue_->IsObject()) {
                DN_CORE_WARN("DataValue is not an Object!");
                return DataValue();
            }
            if (!jvalue_->HasMember(member.c_str())) {
                DN_CORE_WARN("DataValue does not contain member {}!", member);
                return DataValue();
            }
            rapidjson::Value& value = (*jvalue_)[member.c_str()];
            return DataValue(jdoc_, &value);
        }
        DN_CORE_WARN("DataValue is empty!");
        return DataValue();
    }

    DataValue DataValue::operator[](int idx)
    {
        if (IsReadValid()) {
            if (jvalue_->IsArray()) {
                rapidjson::SizeType rjIdx = static_cast<rapidjson::SizeType>(idx);
                rapidjson::Value& value = jvalue_[rjIdx];
                return DataValue(jdoc_, &value);
            } else {
                DN_CORE_WARN("DataValue not array, cannot dereference!");
                return DataValue();
            }
        };
        DN_CORE_WARN("DataValue is empty!");
        return DataValue();
    }

    const DataValue& DataValue::operator*() const
    {
        if (jdoc_ && jvalue_) {
            return *this;
        }
        DN_CORE_WARN("ConstDataIterator invalid, cannot dereference!");
        return *this;
    }

    bool DataValue::operator==(const DataValue& other)
    {
        if (IsReadValid()) {
            if (other.IsReadValid()) {
                return jvalue_ == other.jvalue_;
            }
            DN_CORE_WARN("Second DataValue invalid, cannot test equality!");
            return false;
        }
        DN_CORE_WARN("First DataValue invalid, cannot test equality!");
        return false;
    }

    bool DataValue::operator!=(const DataValue& other)
    {
        if (IsReadValid()) {
            if (other.IsReadValid()) {
                return jvalue_ != other.jvalue_;
            }
            DN_CORE_WARN("Second DataValue invalid, cannot test inequality!");
            return false;
        }
        DN_CORE_WARN("First DataValue invalid, cannot test inequality!");
        return false;
    }





    /* ITERATOR */
    DataValue::DataIterator::DataIterator()
        : isReadValid(false), readDocument_(nullptr)
    {}

    DataValue::DataIterator::DataIterator(std::shared_ptr<rapidjson::Document> document, rapidjson::Value::ValueIterator it)
        : isReadValid(true), readDocument_(document), it_(it)
    {}

    DataValue DataValue::DataIterator::GetValue()
    {
        if (isReadValid) {
            DataValue value(readDocument_, it_);
            return value;
        }
        DN_CORE_WARN("DataIterator not valid!");
        return DataValue();
    }

    DataValue DataValue::DataIterator::operator*() const
    {
        if (isReadValid && readDocument_) {
            return DataValue(readDocument_, &(*it_));
        }
        DN_CORE_WARN("DataIterator invalid, cannot dereference!");
        return DataValue();
    }

    DataValue::DataIterator& DataValue::DataIterator::operator++()
    {
        if (isReadValid && readDocument_) {
            ++it_;
            return *this;
        }
        DN_CORE_WARN("DataIterator invalid, cannot increment!");
        return *this;
    }

    bool DataValue::DataIterator::operator==(const DataIterator other)
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

    bool DataValue::DataIterator::operator!=(const DataIterator other)
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
    DataValue::ConstDataIterator::ConstDataIterator()
        : isReadValid(false), readDocument_(nullptr)
    {}

    DataValue::ConstDataIterator::ConstDataIterator(std::shared_ptr<rapidjson::Document> document, const rapidjson::Value::ConstValueIterator it)
        : isReadValid(true), readDocument_(document), it_(it)
    {}

    const DataValue DataValue::ConstDataIterator::GetValue()
    {
        if (isReadValid) {
            const DataValue value(readDocument_, it_);
            return value;
        }
        DN_CORE_WARN("ConstDataIterator not valid!");
        return DataValue();
    }

    DataValue DataValue::ConstDataIterator::operator*() const
    {
        if (isReadValid && readDocument_) {
            return DataValue(readDocument_, &(*it_));
        }
        DN_CORE_WARN("ConstDataIterator invalid, cannot dereference!");
        return DataValue();
    }

    DataValue::ConstDataIterator DataValue::ConstDataIterator::operator++()
    {
        if (isReadValid && readDocument_) {
            ++it_;
            return *this;
        }
        DN_CORE_WARN("ConstDataIterator invalid, cannot increment!");
        return *this;
    }

    bool DataValue::ConstDataIterator::operator==(const ConstDataIterator other)
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

    bool DataValue::ConstDataIterator::operator!=(const ConstDataIterator other)
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
