#pragma once

#include <unordered_map>
#include <functional>
#include <memory>

namespace duin
{
class ScriptMemory
{
  public:
    using Key = void *;

    template <typename T>
    T *Add(std::shared_ptr<T> ptr)
    {
        Key key = ptr.get();
        memory[key] = ptr;
        return (T *)(key);
    }

    template <typename T>
    std::shared_ptr<T> Get(Key key)
    {
        auto it = memory.find(key);
        if (it == memory.end()) return nullptr;
        return std::static_pointer_cast<T>(it->second);
    }

    void Remove(Key key)
    {
        memory.erase(key);
    }

    void ClearMemory()
    {
        memory.clear();
    }

  private:
    std::unordered_map<Key, std::shared_ptr<void>> memory;
};
} // namespace duin