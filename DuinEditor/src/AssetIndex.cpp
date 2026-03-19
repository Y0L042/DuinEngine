#include "dnpch.h"
#include "AssetIndex.h"
#include <rfl.hpp>
#include <rfl/json.hpp>
#include <Duin/IO/VirtualFilesystem.h>

bool duin::AssetIndex::AddAsset(duin::AssetRef ref)
{
    if (!ref.uuid)
        return false;
    if (index.find(*ref.uuid) == index.end())
    {
        index[*ref.uuid] = ref;
        return true;
    }
    return false;
}

bool duin::AssetIndex::UpdateAsset(duin::AssetRef ref)
{
    if (!ref.uuid)
        return false;
    if (index.find(*ref.uuid) == index.end())
    {
        return false;
    }
    index[*ref.uuid] = ref;
    return true;
}

bool duin::AssetIndex::EnsureAsset(duin::AssetRef ref)
{
    if (!ref.uuid)
        return false;
    index[*ref.uuid] = ref;
    return true;
}

bool duin::AssetIndex::RemoveAsset(duin::UUID uuid)
{
    if (index.find(uuid) == index.end())
    {
        return false;
    }
    index.erase(uuid);
    return true;
}

bool duin::AssetIndex::RemoveAsset(duin::AssetRef ref)
{
    if (!ref.uuid)
        return false;
    return RemoveAsset(*ref.uuid);
}

bool duin::AssetIndex::HasAsset(duin::UUID uuid) const
{
    return (index.find(uuid) != index.end());
}

bool duin::AssetIndex::HasAsset(duin::AssetRef ref) const
{
    if (!ref.uuid)
        return false;
    return HasAsset(*ref.uuid);
}

bool duin::AssetIndex::Serialize(std::string &output) const
{
    Impl impl;
    impl.version = 2;
    impl.refs.reserve(index.size());
    for (const auto &kv : index)
    {
        duin::AssetRef r = kv.second;
        r.rPath = duin::vfs::OverridePathPrefix(r.rPath, duin::vfs::APPROOT()); // Ensure that rPath points to bin:// (not wrk://)
        impl.refs.push_back(r);
    }

    output = rfl::json::write(impl);
    return true;
}

bool duin::AssetIndex::Deserialize(const std::string &input)
{
    Impl impl;
    duin::JSONValue v = duin::JSONValue::Parse(input);

    if (!v.IsValid() || v.IsEmpty())
    {
        DN_WARN("Invalid AssetIndex input {}!", input);
        return false;
    }

    impl = rfl::json::read<Impl>(v.Write()).value();

    for (auto ref : impl.refs)
    {
        if (!ref.uuid)
            continue;
        ref.rPath = duin::vfs::OverridePathPrefix(ref.rPath, duin::vfs::WRKDATA()); // Convert rPath to wrk:// for editor usage
        index[*ref.uuid] = ref;
    }

    return true;
}
