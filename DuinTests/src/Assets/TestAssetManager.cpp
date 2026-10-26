#include <doctest.h>
#include <Duin/Assets/AssetManager.h>
#include <Duin/Assets/Model.h>
#include <filesystem>
#include <fstream>
#include <string>

namespace TestAssetManager
{
TEST_SUITE("AssetManager - Without a renderer")
{
    TEST_CASE("Unknown paths are not reported as loaded")
    {
        duin::AssetManager &am = duin::AssetManager::Get();
        CHECK_FALSE(am.HasAsset("does/not/exist.obj"));
        CHECK_FALSE(am.HasAsset(""));
    }

    TEST_CASE("Singleton is stable")
    {
        CHECK(&duin::AssetManager::Get() == &duin::AssetManager::Get());
    }

    TEST_CASE("LoadModel of a missing file returns nullopt and caches nothing")
    {
        duin::AssetManager &am = duin::AssetManager::Get();
        const std::string path = "./artifacts/asset_manager/missing_model.obj";

        std::optional<duin::Model> model = am.LoadModel(path);
        CHECK_FALSE(model.has_value());
        CHECK_FALSE(am.HasAsset(path));
    }

    TEST_CASE("LoadModel of an empty file returns nullopt")
    {
        duin::AssetManager &am = duin::AssetManager::Get();
        std::filesystem::create_directories("./artifacts/asset_manager");
        const std::string path = "./artifacts/asset_manager/empty.obj";
        {
            std::ofstream f(path);
        }

        std::optional<duin::Model> model = am.LoadModel(path);
        CHECK_FALSE(model.has_value());
        CHECK_FALSE(am.HasAsset(path));
    }

    TEST_CASE("Separate AssetManager instances get distinct UUIDs")
    {
        duin::AssetManager a;
        duin::AssetManager b;
        CHECK(a.GetUUID() != b.GetUUID());
    }
}

TEST_SUITE("Model - Without data")
{
    TEST_CASE("Model constructed from a path has no data")
    {
        duin::Model model("some/path.obj");
        CHECK(model.path == "some/path.obj");
        CHECK(model.data == nullptr);
    }
}
} // namespace TestAssetManager
