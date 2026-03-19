#include <doctest.h>
#include "AssetIndex.h"
#include <Duin/Assets/AssetRef.h>
#include <Duin/Assets/FileTypes.h>
#include <Duin/Core/Utils/UUID.h>
#include <string>
#include <cstdlib>

namespace TestAssetIndex
{
TEST_SUITE("AssetIndex")
{
    TEST_CASE("AddAsset - adds a new asset to the index")
    {
        duin::AssetIndex assetIndex;
        duin::AssetRef ref("assets/texture.png", duin::FS_FILETYPE_IMAGE_EXT, duin::FS_FILEEXT_PNG);

        CHECK(ref.IsValid());
        CHECK(assetIndex.AddAsset(ref) == true);
        CHECK(assetIndex.HasAsset(ref.uuid) == true);
    }

    TEST_CASE("AddAsset - rejects duplicate UUID")
    {
        duin::AssetIndex assetIndex;
        duin::AssetRef ref("assets/texture.png", duin::FS_FILETYPE_IMAGE_EXT, duin::FS_FILEEXT_PNG);

        CHECK(assetIndex.AddAsset(ref) == true);
        CHECK(assetIndex.AddAsset(ref) == false);
    }

    TEST_CASE("UpdateAsset - updates an existing asset")
    {
        duin::AssetIndex assetIndex;
        duin::AssetRef ref("assets/texture.png", duin::FS_FILETYPE_IMAGE_EXT, duin::FS_FILEEXT_PNG);
        assetIndex.AddAsset(ref);

        duin::AssetRef updated;
        updated.uuid = ref.uuid;
        updated.rPath = "assets/texture_v2.png";
        updated.fileType = duin::FS_FILETYPE_IMAGE_EXT;
        updated.fileExt = duin::FS_FILEEXT_PNG;

        CHECK(assetIndex.UpdateAsset(updated) == true);
        CHECK(assetIndex.HasAsset(ref.uuid) == true);
    }

    TEST_CASE("UpdateAsset - fails for non-existent asset")
    {
        duin::AssetIndex assetIndex;
        duin::AssetRef ref("assets/texture.png", duin::FS_FILETYPE_IMAGE_EXT, duin::FS_FILEEXT_PNG);

        CHECK(assetIndex.UpdateAsset(ref) == false);
    }

    TEST_CASE("EnsureAsset - adds new asset if not present")
    {
        duin::AssetIndex assetIndex;
        duin::AssetRef ref("assets/model.blend", duin::FS_FILETYPE_MODEL_EXT, duin::FS_FILEEXT_BLEND);

        CHECK(assetIndex.EnsureAsset(ref) == true);
        CHECK(assetIndex.HasAsset(ref.uuid) == true);
    }

    TEST_CASE("EnsureAsset - overwrites existing asset")
    {
        duin::AssetIndex assetIndex;
        duin::AssetRef ref("assets/model.blend", duin::FS_FILETYPE_MODEL_EXT, duin::FS_FILEEXT_BLEND);
        assetIndex.EnsureAsset(ref);

        duin::AssetRef updated;
        updated.uuid = ref.uuid;
        updated.rPath = "assets/model_v2.blend";
        updated.fileType = duin::FS_FILETYPE_MODEL_EXT;
        updated.fileExt = duin::FS_FILEEXT_BLEND;

        CHECK(assetIndex.EnsureAsset(updated) == true);
        CHECK(assetIndex.HasAsset(ref.uuid) == true);
    }

    TEST_CASE("RemoveAsset by UUID - removes existing asset")
    {
        duin::AssetIndex assetIndex;
        duin::AssetRef ref("assets/sound.wav", duin::FS_FILETYPE_AUDIO_EXT, duin::FS_FILEEXT_WAV);
        assetIndex.EnsureAsset(ref);

        CHECK(assetIndex.RemoveAsset(ref.uuid) == true);
        CHECK(assetIndex.HasAsset(ref.uuid) == false);
    }

    TEST_CASE("RemoveAsset by UUID - fails for non-existent UUID")
    {
        duin::AssetIndex assetIndex;
        duin::UUID nonExistent;

        CHECK(assetIndex.RemoveAsset(nonExistent) == false);
    }

    TEST_CASE("RemoveAsset by AssetRef - removes existing asset")
    {
        duin::AssetIndex assetIndex;
        duin::AssetRef ref("assets/video.mp4", duin::FS_FILETYPE_VIDEO_EXT, duin::FS_FILEEXT_MP4);
        assetIndex.EnsureAsset(ref);

        CHECK(assetIndex.RemoveAsset(ref) == true);
        CHECK(assetIndex.HasAsset(ref) == false);
    }

    TEST_CASE("HasAsset by UUID - returns false for empty index")
    {
        duin::AssetIndex assetIndex;
        duin::UUID uuid;

        CHECK(assetIndex.HasAsset(uuid) == false);
    }

    TEST_CASE("HasAsset by AssetRef - returns true for present asset")
    {
        duin::AssetIndex assetIndex;
        duin::AssetRef ref("assets/config.json", duin::FS_FILETYPE_TEXT_EXT, duin::FS_FILEEXT_JSON);
        assetIndex.EnsureAsset(ref);

        CHECK(assetIndex.HasAsset(ref) == true);
    }

    TEST_CASE("GetIndex - returns the internal map")
    {
        duin::AssetIndex assetIndex;
        duin::AssetRef ref1("assets/a.png", duin::FS_FILETYPE_IMAGE_EXT, duin::FS_FILEEXT_PNG);
        duin::AssetRef ref2("assets/b.wav", duin::FS_FILETYPE_AUDIO_EXT, duin::FS_FILEEXT_WAV);
        assetIndex.EnsureAsset(ref1);
        assetIndex.EnsureAsset(ref2);

        const auto &index = assetIndex.GetIndex();
        CHECK(index.size() == 2);
        CHECK(index.count(ref1.uuid) == 1);
        CHECK(index.count(ref2.uuid) == 1);
    }

    TEST_CASE("Serialize and Deserialize - round-trip preserves assets")
    {
        duin::AssetIndex original;

        duin::AssetRef ref1("assets/texture.png", duin::FS_FILETYPE_IMAGE_EXT, duin::FS_FILEEXT_PNG);
        duin::AssetRef ref2("assets/model.blend", duin::FS_FILETYPE_MODEL_EXT, duin::FS_FILEEXT_BLEND);
        duin::AssetRef ref3("assets/sound.ogg", duin::FS_FILETYPE_AUDIO_EXT, duin::FS_FILEEXT_OGG);
        original.EnsureAsset(ref1);
        original.EnsureAsset(ref2);
        original.EnsureAsset(ref3);

        std::string serialized;
        CHECK(original.Serialize(serialized) == true);
        CHECK(serialized.empty() == false);

        duin::AssetIndex restored;
        CHECK(restored.Deserialize(serialized) == true);

        CHECK(restored.HasAsset(ref1.uuid) == true);
        CHECK(restored.HasAsset(ref2.uuid) == true);
        CHECK(restored.HasAsset(ref3.uuid) == true);

        const auto &restoredIndex = restored.GetIndex();
        CHECK(restoredIndex.size() == 3);
    }

    TEST_CASE("Serialize - empty index produces valid output")
    {
        duin::AssetIndex assetIndex;
        std::string output;

        CHECK(assetIndex.Serialize(output) == true);
        CHECK(output.empty() == false);
    }

    TEST_CASE("Deserialize into non-empty index merges assets")
    {
        duin::AssetIndex index1;
        duin::AssetRef ref1("assets/a.png", duin::FS_FILETYPE_IMAGE_EXT, duin::FS_FILEEXT_PNG);
        index1.EnsureAsset(ref1);

        std::string serialized;
        index1.Serialize(serialized);

        duin::AssetIndex index2;
        duin::AssetRef ref2("assets/b.wav", duin::FS_FILETYPE_AUDIO_EXT, duin::FS_FILEEXT_WAV);
        index2.EnsureAsset(ref2);

        index2.Deserialize(serialized);

        CHECK(index2.HasAsset(ref1.uuid) == true);
        CHECK(index2.HasAsset(ref2.uuid) == true);
        CHECK(index2.GetIndex().size() == 2);
    }

    TEST_CASE("Serialize and Deserialize - many assets round-trip")
    {
        duin::AssetIndex original;

        for (int i = 0; i < 150; i++)
        {
            duin::AssetRef ref;
            ref.uuid = duin::UUID();
            ref.rPath = "assets/item_" + std::to_string(i);
            ref.fileType = static_cast<duin::FileType>(std::rand() % duin::FS_FILETYPE_COUNT);
            ref.fileExt = static_cast<duin::FileExt>(std::rand() % duin::FS_FILEEXT_COUNT);
            original.EnsureAsset(ref);
        }

        CHECK(original.GetIndex().size() == 150);

        std::string serialized;
        CHECK(original.Serialize(serialized) == true);

        duin::AssetIndex restored;
        CHECK(restored.Deserialize(serialized) == true);
        CHECK(restored.GetIndex().size() == 150);

        for (const auto &[uuid, ref] : original.GetIndex())
        {
            CHECK(restored.HasAsset(uuid) == true);
        }
    }

    TEST_CASE("AddAsset then RemoveAsset - index returns to empty")
    {
        duin::AssetIndex assetIndex;
        duin::AssetRef ref("assets/temp.txt", duin::FS_FILETYPE_TEXT_EXT, duin::FS_FILEEXT_TXT);

        assetIndex.AddAsset(ref);
        assetIndex.RemoveAsset(ref);

        CHECK(assetIndex.HasAsset(ref.uuid) == false);
        CHECK(assetIndex.GetIndex().empty() == true);
    }

    TEST_CASE("Multiple operations sequence")
    {
        duin::AssetIndex assetIndex;

        duin::AssetRef ref1("assets/a.png", duin::FS_FILETYPE_IMAGE_EXT, duin::FS_FILEEXT_PNG);
        duin::AssetRef ref2("assets/b.obj", duin::FS_FILETYPE_MODEL_EXT, duin::FS_FILEEXT_OBJ);
        duin::AssetRef ref3("assets/c.mp3", duin::FS_FILETYPE_AUDIO_EXT, duin::FS_FILEEXT_MP3);

        assetIndex.EnsureAsset(ref1);
        assetIndex.EnsureAsset(ref2);
        assetIndex.EnsureAsset(ref3);
        CHECK(assetIndex.GetIndex().size() == 3);

        assetIndex.RemoveAsset(ref2);
        CHECK(assetIndex.GetIndex().size() == 2);
        CHECK(assetIndex.HasAsset(ref2) == false);

        duin::AssetRef ref1Updated;
        ref1Updated.uuid = ref1.uuid;
        ref1Updated.rPath = "assets/a_updated.png";
        ref1Updated.fileType = duin::FS_FILETYPE_IMAGE_EXT;
        ref1Updated.fileExt = duin::FS_FILEEXT_PNG;
        assetIndex.UpdateAsset(ref1Updated);

        CHECK(assetIndex.HasAsset(ref1.uuid) == true);
        CHECK(assetIndex.HasAsset(ref3.uuid) == true);
    }
    TEST_CASE("Stress - 5000 assets add, lookup, and remove")
    {
        duin::AssetIndex assetIndex;
        constexpr int COUNT = 5000;
        std::vector<duin::UUID> uuids;
        uuids.reserve(COUNT);

        for (int i = 0; i < COUNT; i++)
        {
            duin::AssetRef ref;
            ref.uuid = duin::UUID();
            ref.rPath = "assets/stress/item_" + std::to_string(i) + ".png";
            ref.fileType = static_cast<duin::FileType>(i % duin::FS_FILETYPE_COUNT);
            ref.fileExt = static_cast<duin::FileExt>(i % duin::FS_FILEEXT_COUNT);
            uuids.push_back(ref.uuid);
            assetIndex.EnsureAsset(ref);
        }

        CHECK(assetIndex.GetIndex().size() == COUNT);

        for (const auto &uuid : uuids)
        {
            CHECK(assetIndex.HasAsset(uuid) == true);
        }

        for (int i = 0; i < COUNT / 2; i++)
        {
            CHECK(assetIndex.RemoveAsset(uuids[i]) == true);
        }

        CHECK(assetIndex.GetIndex().size() == COUNT / 2);

        for (int i = 0; i < COUNT / 2; i++)
        {
            CHECK(assetIndex.HasAsset(uuids[i]) == false);
        }
        for (int i = COUNT / 2; i < COUNT; i++)
        {
            CHECK(assetIndex.HasAsset(uuids[i]) == true);
        }
    }

    TEST_CASE("Stress - 10000 assets serialize and deserialize round-trip")
    {
        duin::AssetIndex original;
        constexpr int COUNT = 10000;

        const duin::FileType fileTypes[] = {
            duin::FS_FILETYPE_IMAGE_EXT, duin::FS_FILETYPE_AUDIO_EXT,
            duin::FS_FILETYPE_VIDEO_EXT, duin::FS_FILETYPE_MODEL_EXT,
            duin::FS_FILETYPE_TEXT_EXT};
        const duin::FileExt fileExts[] = {
            duin::FS_FILEEXT_PNG, duin::FS_FILEEXT_WAV, duin::FS_FILEEXT_MP4,
            duin::FS_FILEEXT_BLEND, duin::FS_FILEEXT_JSON};

        for (int i = 0; i < COUNT; i++)
        {
            duin::AssetRef ref;
            ref.uuid = duin::UUID();
            ref.rPath = "assets/project/folder_" + std::to_string(i / 100) + "/asset_" + std::to_string(i);
            ref.fileType = fileTypes[i % 5];
            ref.fileExt = fileExts[i % 5];
            original.EnsureAsset(ref);
        }

        CHECK(original.GetIndex().size() == COUNT);

        std::string serialized;
        CHECK(original.Serialize(serialized) == true);
        CHECK(serialized.size() > 0);

        duin::AssetIndex restored;
        CHECK(restored.Deserialize(serialized) == true);
        CHECK(restored.GetIndex().size() == COUNT);

        for (const auto &[uuid, ref] : original.GetIndex())
        {
            CHECK(restored.HasAsset(uuid) == true);
        }

        std::string reserialized;
        CHECK(restored.Serialize(reserialized) == true);

        duin::AssetIndex restored2;
        CHECK(restored2.Deserialize(reserialized) == true);
        CHECK(restored2.GetIndex().size() == COUNT);
    }
}
} // namespace TestAssetIndex
