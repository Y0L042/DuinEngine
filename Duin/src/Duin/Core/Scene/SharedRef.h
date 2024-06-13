#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Core/Scene/Object.h"

#include <memory>
#include <cinttypes>

namespace Duin
{
    template<typename T>
    class DUIN_API SharedRef;

    template<typename T>
    class DUIN_API SilentRef : public Object
    {
    public:
        SilentRef()
            : ptr(nullptr), refCount(new uint32_t(0)), sharedCount(new uint32_t(0))
        {}

        SilentRef(T* rawPtr)
            : ptr(rawPtr, [](T* p) { delete p; }), refCount(new uint32_t(1)), sharedCount(new uint32_t(0))
        {}

        uint32_t GetRefCount() const
        {
            return ptr.use_count();
        }

        uint32_t GetSharedCount() const
        {
            return *this->sharedCount;
        }

        T* Get() const
        {
            return ptr.get();
        }

        SilentRef(const SilentRef& obj)
            : ptr(obj.ptr), refCount(obj.refCount), sharedCount(obj.sharedCount)
        {
            if (ptr)
            {
                (*refCount)++;
            }
        }

        SilentRef& operator=(const SilentRef& obj)
        {
            if (this != &obj)
            {
                ptr = obj.ptr;
                refCount = obj.refCount;
                sharedCount = obj.sharedCount;
                if (ptr)
                {
                    (*refCount)++;
                }
            }
            return *this;
        }

        SilentRef(SilentRef&& dyingObj) noexcept
            : ptr(std::move(dyingObj.ptr)), refCount(dyingObj.refCount), sharedCount(dyingObj.sharedCount)
        {
            dyingObj.refCount = nullptr;
            dyingObj.sharedCount = nullptr;
        }

        SilentRef& operator=(SilentRef&& dyingObj) noexcept
        {
            if (this != &dyingObj)
            {
                ptr = std::move(dyingObj.ptr);
                refCount = dyingObj.refCount;
                sharedCount = dyingObj.sharedCount;
                dyingObj.refCount = nullptr;
                dyingObj.sharedCount = nullptr;
            }
            return *this;
        }

        T* operator->() const
        {
            return ptr.get();
        }

        T& operator*() const
        {
            return *ptr;
        }

        SharedRef<T> ToSharedRef();

    private:
        std::shared_ptr<T> ptr;
        std::shared_ptr<uint32_t> refCount;
        std::shared_ptr<uint32_t> sharedCount;

        friend class SharedRef<T>;
    };

    template<typename T>
    class DUIN_API SharedRef : public SilentRef<T>
    {
    public:
        SharedRef() : SilentRef<T>() {}

        SharedRef(const SilentRef<T>& obj)
            : SilentRef<T>(obj)
        {
            if (this->ptr)
            {
                (*this->sharedCount)++;
            }
        }

        SharedRef(const SharedRef& obj)
            : SilentRef<T>(obj)
        {
            if (this->ptr)
            {
                (*this->sharedCount)++;
            }
        }

        SharedRef& operator=(const SharedRef& obj)
        {
            if (this != &obj)
            {
                SilentRef<T>::operator=(obj);
                if (this->ptr)
                {
                    (*this->sharedCount)++;
                }
            }
            return *this;
        }

        SharedRef(SharedRef&& dyingObj) noexcept
            : SilentRef<T>(std::move(dyingObj))
        {}

        SharedRef& operator=(SharedRef&& dyingObj) noexcept
        {
            SilentRef<T>::operator=(std::move(dyingObj));
            return *this;
        }
    };

    template<typename T>
    SharedRef<T> SilentRef<T>::ToSharedRef()
    {
        return SharedRef<T>(*this);
    }
}
