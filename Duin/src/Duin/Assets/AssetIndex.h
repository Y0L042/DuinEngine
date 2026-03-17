#pragma once

#include <map>
#include "Duin/Core/Utils/UUID.h"
#include "Duin/IO/JSONValue.h"
#include "AssetRef.h"
#include <string>

namespace duin
{
class AssetIndex
{
  public:
    struct Impl
    {
        size_t version;
        std::vector<AssetRef> refs;
    };

    bool AddAsset(AssetRef ref);
    bool UpdateAsset(AssetRef ref);
    bool EnsureAsset(AssetRef ref);
    bool RemoveAsset(UUID uuid);
    bool RemoveAsset(AssetRef ref);

    bool HasAsset(UUID uuid) const;
    bool HasAsset(AssetRef ref) const;

    bool Serialize(std::string &output) const;
    bool Deserialize(const std::string &input);

    const std::map<UUID, AssetRef> &GetIndex() const
    {
        return index;
    }

  private:
    std::map<UUID, AssetRef> index;
};
} // namespace duin
