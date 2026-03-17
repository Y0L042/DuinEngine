#include "dnpch.h"
#include "AssetIndex.h"
#include <rfl.hpp>
#include <rfl/json.hpp>

bool duin::AssetIndex::AddAsset(AssetRef ref)
{
    if (index.find(ref.uuid) == index.end())
    {
        index[ref.uuid] = ref;
        return true;
    }
    return false;
}

bool duin::AssetIndex::UpdateAsset(AssetRef ref)
{
    if (index.find(ref.uuid) == index.end())
    {
        return false;
    }
    index[ref.uuid] = ref;
    return true;
}

bool duin::AssetIndex::EnsureAsset(AssetRef ref)
{
    index[ref.uuid] = ref;
    return true;
}

bool duin::AssetIndex::RemoveAsset(UUID uuid)
{
    if (index.find(uuid) == index.end())
    {
        return false;
    }
    index.erase(uuid);
    return true;
}

bool duin::AssetIndex::RemoveAsset(AssetRef ref)
{
    return RemoveAsset(ref.uuid);
}

bool duin::AssetIndex::HasAsset(UUID uuid) const
{
    return (index.find(uuid) != index.end());
}

bool duin::AssetIndex::HasAsset(AssetRef ref) const
{
    return HasAsset(ref.uuid);
}

bool duin::AssetIndex::Serialize(std::string &output) const
{
    Impl impl;
    impl.version = 2;
    impl.refs.reserve(index.size());
    for (const auto &kv : index)
    {
        impl.refs.push_back(kv.second);
    }

    output = rfl::json::write(impl);
    return true;
}

bool duin::AssetIndex::Deserialize(const std::string &input)
{
    Impl impl;
    impl = rfl::json::read<Impl>(input).value();

    for (auto &ref : impl.refs)
    {
        index[ref.uuid] = ref;
    }

    return true;
}
