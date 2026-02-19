#pragma once
#include <unordered_map>
#include <string>
#include "./DECS/Entity.h"
#include "Duin/Core/Debug/DNLog.h"

namespace duin
{
class PrefabRegistry
{
  public:
    static PrefabRegistry &Get()
    {
        static PrefabRegistry instance;
        return instance;
    }

    void RegisterPrefabEntity(const std::string &name, Entity prefab)
    {
        prefabs_[name] = prefab;
        DN_CORE_INFO("Registered prefab entity: {}", name);
    }

    Entity Find(const std::string &name) const
    {
        auto it = prefabs_.find(name);
        if (it != prefabs_.end())
        {
            return it->second;
        }
        DN_CORE_WARN("Prefab entity not found: {}", name);
        return Entity();
    }

  private:
    std::unordered_map<std::string, Entity> prefabs_;
};

} // namespace duin