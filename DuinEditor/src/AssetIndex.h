#pragma once

#include <map>
#include <Duin/Core/Utils/UUID.h>
#include <Duin/IO/JSONValue.h>
#include <Duin/Assets/AssetRef.h>
#include <string>

namespace duin
{
class AssetIndex
{
  public:
    struct Impl
    {
        size_t version;
        std::vector<duin::AssetRef> refs;
    };

    bool AddAsset(duin::AssetRef ref);
    bool UpdateAsset(duin::AssetRef ref);
    bool EnsureAsset(duin::AssetRef ref);
    bool RemoveAsset(duin::UUID uuid);
    bool RemoveAsset(duin::AssetRef ref);

    bool HasAsset(duin::UUID uuid) const;
    bool HasAsset(duin::AssetRef ref) const;

    bool Serialize(std::string &output) const;
    bool Deserialize(const std::string &input);

    const std::map<duin::UUID, duin::AssetRef> &GetIndex() const
    {
        return index;
    }

  private:
    std::map<duin::UUID, duin::AssetRef> index;
};
} // namespace duin
