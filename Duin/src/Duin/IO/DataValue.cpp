#include "dnpch.h"
#include "DataValue.h"

#include <Duin/Core/Debug/DNLog.h>

// TODO add own error handling for this

namespace duin {
    /* DATAVALUE */
    DataValue::DataValue()
        : document_(nullptr), value_(nullptr)
    {}

    DataValue::DataValue(std::shared_ptr<rapidjson::Document> document, rapidjson::Value *value)
        : document_(document), value_(value)
    {
        // If no subvalue is passed, use document as value
        if (value_ == nullptr) {
            value_ = document_.get();
        }
    }

    DataValue::DataValue(std::shared_ptr<rapidjson::Document> document, const rapidjson::Value* value)
        : document_(document), value_(const_cast<rapidjson::Value*>(value)) // Cast const to non-const
    {
        // If no subvalue is passed, use document as value
        if (value_ == nullptr) {
            value_ = document_.get();
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

    bool DataValue::IsValid() const
    {
        if (document_ && value_) {
            return true;
        } else {
            return false;
        }
        DN_CORE_WARN("DataValue is empty!");
        return false;
    }

    bool DataValue::HasMember(const std::string& member) const
    {
        if (value_) {
            if (value_->HasMember(member.c_str())) {
                return true;
            } else {
                return false;
            }
        }
        DN_CORE_WARN("DataValue is empty!");
        return false;
    }

    bool DataValue::IsString() const
    {
        if (value_) {
            bool v = value_->IsString();
            return v;
        }
        DN_CORE_WARN("DataValue is empty!");
        return false;
    }

    std::string DataValue::GetString() const
    {
        if (value_) {
            if (value_->IsString()) {
                std::string s = value_->GetString();
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
        if (value_) {
            bool v = value_->IsBool();
            return v;
        }
        DN_CORE_WARN("DataValue is empty!");
        return false;
    }

    bool DataValue::GetBool() const
    {
        if (value_) {
            if (value_->IsBool()) {
                bool v = value_->GetBool();
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
        if (value_) {
            bool v = value_->IsNumber();
            return v;
        }
        DN_CORE_WARN("DataValue is empty!");
        return false;
    }

    bool DataValue::IsInt() const
    {
        if (value_) {
            bool v = value_->IsInt();
            return v;
        }
        DN_CORE_WARN("DataValue is empty!");
        return false;
    }

    int DataValue::GetInt() const
    {
        if (value_) {
            if (value_->IsInt()) {
                int v = value_->GetInt();
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
        if (value_) {
            bool v = value_->IsDouble();
            return v;
        }
        DN_CORE_WARN("DataValue is empty!");
        return false;
    }

    double DataValue::GetDouble() const
    {
        if (value_) {
            if (value_->IsDouble()) {
                double v = value_->GetDouble();
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
        if (value_) {
            bool v = value_->IsArray();
            return v;
        }
        DN_CORE_WARN("DataValue is empty!");
        return false;
    }

    bool DataValue::Empty() const
    {
        if (value_) {
            if (value_->IsArray()) {
                bool v = value_->Empty();
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
        if (value_) {
            if (value_->IsArray()) {
                rapidjson::SizeType v = value_->Capacity();
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
        if (value_) {
            if (value_->IsArray()) {
                rapidjson::Value::ConstValueIterator it = value_->Begin();
                return ConstDataIterator(document_, it);
            } else {
                DN_CORE_WARN("DataValue not array!");
            }
        }
        DN_CORE_WARN("DataValue is empty!");
        return ConstDataIterator();
    }

    DataValue::ConstDataIterator DataValue::End()
    {
        if (value_) {
            if (value_->IsArray()) {
                rapidjson::Value::ConstValueIterator it = value_->End();
                return ConstDataIterator(document_, it);
            } else {
                DN_CORE_WARN("DataValue not array!");
            }
        }
        DN_CORE_WARN("DataValue is empty!");
        return ConstDataIterator();
    }

    DataValue DataValue::operator[](const std::string& member)
    {
        if (IsValid()) {
            rapidjson::Value& value(*document_);
            return DataValue(document_, &value);
        }
        return DataValue();
    }

    DataValue DataValue::operator*() const
    {
        if (document_ && value_) {
            return *this;
        }
        DN_CORE_WARN("ConstDataIterator invalid, cannot dereference!");
        return DataValue();
    }

    bool DataValue::operator==(const DataValue& other)
    {
        if (IsValid()) {
            if (other.IsValid()) {
                return value_ == other.value_;
            }
            DN_CORE_WARN("Second DataValue invalid, cannot test equality!");
            return false;
        }
        DN_CORE_WARN("First DataValue invalid, cannot test equality!");
        return false;
    }

    bool DataValue::operator!=(const DataValue& other)
    {
        if (IsValid()) {
            if (other.IsValid()) {
                return value_ != other.value_;
            }
            DN_CORE_WARN("Second DataValue invalid, cannot test inequality!");
            return false;
        }
        DN_CORE_WARN("First DataValue invalid, cannot test inequality!");
        return false;
    }





    /* ITERATOR */
    DataValue::DataIterator::DataIterator()
        : isValid(false), document_(nullptr)
    {}

    DataValue::DataIterator::DataIterator(std::shared_ptr<rapidjson::Document> document, rapidjson::Value::ValueIterator it)
        : isValid(true), document_(document), it_(it)
    {}

    DataValue DataValue::DataIterator::GetValue()
    {
        if (isValid) {
            DataValue value(document_, it_);
            return value;
        }
        DN_CORE_WARN("DataIterator not valid!");
        return DataValue();
    }

    DataValue DataValue::DataIterator::operator*() const
    {
        if (isValid && document_) {
            return DataValue(document_, &(*it_));
        }
        DN_CORE_WARN("DataIterator invalid, cannot dereference!");
        return DataValue();
    }

    DataValue::DataIterator& DataValue::DataIterator::operator++()
    {
        if (isValid && document_) {
            ++it_;
            return *this;
        }
        DN_CORE_WARN("DataIterator invalid, cannot increment!");
        return *this;
    }

    bool DataValue::DataIterator::operator==(const DataIterator other)
    {
        if (isValid && document_) {
            if (other.isValid && other.document_) {
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
        if (isValid && document_) {
            if (other.isValid && other.document_) {
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
        : isValid(false), document_(nullptr)
    {}

    DataValue::ConstDataIterator::ConstDataIterator(std::shared_ptr<rapidjson::Document> document, const rapidjson::Value::ConstValueIterator it)
        : isValid(true), document_(document), it_(it)
    {}

    const DataValue DataValue::ConstDataIterator::GetValue()
    {
        if (isValid) {
            const DataValue value(document_, it_);
            return value;
        }
        DN_CORE_WARN("ConstDataIterator not valid!");
        return DataValue();
    }

    DataValue DataValue::ConstDataIterator::operator*() const
    {
        if (isValid && document_) {
            return DataValue(document_, &(*it_));
        }
        DN_CORE_WARN("ConstDataIterator invalid, cannot dereference!");
        return DataValue();
    }

    DataValue::ConstDataIterator& DataValue::ConstDataIterator::operator++()
    {
        if (isValid && document_) {
            ++it_;
            return *this;
        }
        DN_CORE_WARN("ConstDataIterator invalid, cannot increment!");
        return *this;
    }

    bool DataValue::ConstDataIterator::operator==(const ConstDataIterator other)
    {
        if (isValid && document_) {
            if (other.isValid && other.document_) {
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
        if (isValid && document_) {
            if (other.isValid && other.document_) {
                return it_ != other.it_;
            }
            DN_CORE_WARN("Second ConstDataIterator invalid, cannot test equality!");
            return false;
        }
        DN_CORE_WARN("First ConstDataIterator invalid, cannot test equality!");
        return false;
    }
}
