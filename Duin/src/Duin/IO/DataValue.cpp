#include "dnpch.h"
#include "DataValue.h"
#include <Duin/Core/Debug/DNLog.h>

namespace duin {
    /* DATAVALUE */
    DataValue::DataValue()
        : readDocument_(nullptr), readValue_(nullptr)
    {}

    DataValue::DataValue(bool b)
        : modValue_(b)
    {}

    DataValue::DataValue(int x)
        : modValue_(x)
    {}

    DataValue::DataValue(const std::string& text)
    {
        modValue_.SetString(text.c_str(), text.size());
    }

    DataValue::DataValue(double x)
    {}

    DataValue::DataValue(const DataValue & other)
    {
        other;
    }

    DataValue::DataValue(std::shared_ptr<rapidjson::Document> document, rapidjson::Value *value)
        : readDocument_(document), readValue_(value)
    {
        // If no subvalue is passed, use document as value
        if (readValue_ == nullptr) {
            readValue_ = readDocument_.get();
        }
    }

    DataValue::DataValue(std::shared_ptr<rapidjson::Document> document, const rapidjson::Value* value)
        : readDocument_(document), readValue_(const_cast<rapidjson::Value*>(value)) // Cast const to non-const
    {
        // If no subvalue is passed, use document as value
        if (readValue_ == nullptr) {
            readValue_ = readDocument_.get();
            DN_CORE_ERROR("Cannot create DataValue with const nullptr rapidjson::Value!");
        }
    }

    DataValue DataValue::Parse(const std::string& filePath)
    {
        std::shared_ptr<rapidjson::Document> document = std::make_shared<rapidjson::Document>();
        document->Parse(filePath.c_str());

        DataValue dataValue(document);
        
        return dataValue;
    }

    bool DataValue::IsReadValid() const
    {
        if (readDocument_ && readValue_) {
            return true;
        } else {
            return false;
        }
        DN_CORE_WARN("DataValue is empty!");
        return false;
    }

    bool DataValue::HasMember(const std::string& member) const
    {
        if (readValue_) {
            if (readValue_->HasMember(member.c_str())) {
                return true;
            } else {
                return false;
            }
        }
        DN_CORE_WARN("DataValue is empty!");
        return false;
    }

    bool DataValue::IsNull() const
    {
        if (readValue_) {
            bool v = readValue_->IsNull();
            return v;
        }
        DN_CORE_WARN("DataValue is empty!");
        return false;
    }

    bool DataValue::IsString() const
    {
        if (readValue_) {
            bool v = readValue_->IsString();
            return v;
        }
        DN_CORE_WARN("DataValue is empty!");
        return false;
    }

    std::string DataValue::GetString() const
    {
        if (readValue_) {
            if (readValue_->IsString()) {
                std::string s = readValue_->GetString();
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
        if (readValue_) {
            bool v = readValue_->IsBool();
            return v;
        }
        DN_CORE_WARN("DataValue is empty!");
        return false;
    }

    bool DataValue::GetBool() const
    {
        if (readValue_) {
            if (readValue_->IsBool()) {
                bool v = readValue_->GetBool();
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
        if (readValue_) {
            bool v = readValue_->IsNumber();
            return v;
        }
        DN_CORE_WARN("DataValue is empty!");
        return false;
    }

    bool DataValue::IsInt() const
    {
        if (readValue_) {
            bool v = readValue_->IsInt();
            return v;
        }
        DN_CORE_WARN("DataValue is empty!");
        return false;
    }

    int DataValue::GetInt() const
    {
        if (readValue_) {
            if (readValue_->IsInt()) {
                int v = readValue_->GetInt();
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
        if (readValue_) {
            bool v = readValue_->IsDouble();
            return v;
        }
        DN_CORE_WARN("DataValue is empty!");
        return false;
    }

    double DataValue::GetDouble() const
    {
        if (readValue_) {
            if (readValue_->IsDouble()) {
                double v = readValue_->GetDouble();
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
        if (readValue_) {
            bool v = readValue_->IsArray();
            return v;
        }
        DN_CORE_WARN("DataValue is empty!");
        return false;
    }

    bool DataValue::Empty() const
    {
        if (readValue_) {
            if (readValue_->IsArray()) {
                bool v = readValue_->Empty();
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
        if (readValue_) {
            if (readValue_->IsArray()) {
                rapidjson::SizeType v = readValue_->Capacity();
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
        if (readValue_) {
            if (readValue_->IsArray()) {
                rapidjson::Value::ConstValueIterator it = readValue_->Begin();
                return ConstDataIterator(readDocument_, it);
            } else {
                DN_CORE_WARN("DataValue not array!");
            }
        }
        DN_CORE_WARN("DataValue is empty!");
        return ConstDataIterator();
    }

    DataValue::ConstDataIterator DataValue::End()
    {
        if (readValue_) {
            if (readValue_->IsArray()) {
                rapidjson::Value::ConstValueIterator it = readValue_->End();
                return ConstDataIterator(readDocument_, it);
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

    void DataValue::SetObject()
    {
        modValue_.SetObject();
    }

    void DataValue::SetInt(int x)
    {
        modValue_.SetInt(x);
    }

    void DataValue::SetString(const std::string& text)
    {
        modValue_.SetString(text.c_str(), text.size());
    }

    void DataValue::SetDouble(double x)
    {
        modValue_.SetDouble(x);
    }

    void DataValue::SetBool(bool b)
    {
        modValue_.SetBool(b);
    }

    void DataValue::SetArray()
    {
        modValue_.SetArray();
    }

    const DataValue DataValue::operator[](const std::string& member)
    {
        if (IsReadValid()) {
            rapidjson::Value& value(*readDocument_);
            return DataValue(readDocument_, &value);
        }
        DN_CORE_WARN("DataValue is empty!");
        return DataValue();
    }

    const DataValue DataValue::operator[](int idx)
    {
        if (IsReadValid()) {
            if (readValue_->IsArray()) {
                rapidjson::SizeType rjIdx = static_cast<rapidjson::SizeType>(idx);
                rapidjson::Value& value = readValue_[rjIdx];
                return DataValue(readDocument_, &value);
            } else {
                DN_CORE_WARN("DataValue not array, cannot dereference!");
                return DataValue();
            }
        };
        DN_CORE_WARN("DataValue is empty!");
        return DataValue();
    }

    const DataValue DataValue::operator*() const
    {
        if (readDocument_ && readValue_) {
            return *this;
        }
        DN_CORE_WARN("ConstDataIterator invalid, cannot dereference!");
        return DataValue();
    }

    bool DataValue::operator==(const DataValue& other)
    {
        if (IsReadValid()) {
            if (other.IsReadValid()) {
                return readValue_ == other.readValue_;
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
                return readValue_ != other.readValue_;
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

    DataValue::ConstDataIterator& DataValue::ConstDataIterator::operator++()
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

    void DataValue::DataValue::operator=(DataValue& other)
    {
        readDocument_ = other.readDocument_;
        readValue_ = other.readValue_; 

        modValue_ = other.modValue_; // Moves other.modValue_

        other.readDocument_ = nullptr;
        other.readValue_ = nullptr;
    }
}
