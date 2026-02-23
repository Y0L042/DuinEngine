#include "TestConfig.h"
#include "TestSceneBuilderCommon.h"
#include <doctest.h>
#include <Duin/Scene/SceneBuilder.h>
#include <Duin/Core/Utils/UUID.h>
#include <Duin/IO/JSONValue.h>
#include <Duin/ECS/ECSModule.h>

namespace TestSceneBuilder
{

TEST_SUITE("External Dependency Serialization")
{
    TEST_CASE("Deserialize External Dependency")
    {
        duin::World world;
        duin::SceneBuilder sb(&world);

        std::string jsonStr = R"({
            "uuid": "a1b2c3d4e5f67890",
            "type": "scene"
        })";

        duin::JSONValue v = duin::JSONValue::Parse(jsonStr);
        duin::PackedExternalDependency dep = sb.DeserializeExternalDependency(v);

        CHECK(dep.uuid == duin::UUID::FromStringHex("a1b2c3d4e5f67890"));
        CHECK(dep.type == "scene");
    }

    TEST_CASE("Serialize External Dependency")
    {
        duin::World world;
        duin::SceneBuilder sb(&world);

        duin::PackedExternalDependency dep;
        dep.uuid = duin::UUID::FromStringHex("a1b2c3d4e5f67890");
        dep.type = "asset";

        duin::JSONValue json = sb.SerializeExternalDependency(dep);

        CHECK(json.IsObject());
        CHECK(json.HasMember("uuid"));
        CHECK(json["uuid"].GetString() == duin::UUID::FromStringHex("a1b2c3d4e5f67890").ToStrHex());
        CHECK(json.HasMember("type"));
        CHECK(json["type"].GetString() == "asset");
    }

    TEST_CASE("Round-trip External Dependency")
    {
        duin::World world;
        duin::SceneBuilder sb(&world);

        duin::PackedExternalDependency original;
        original.uuid = duin::UUID::FromStringHex("fedcba9876543210");
        original.type = "material";

        duin::JSONValue json = sb.SerializeExternalDependency(original);
        duin::PackedExternalDependency deserialized = sb.DeserializeExternalDependency(json);

        CHECK(original.uuid == deserialized.uuid);
        CHECK(original.type == deserialized.type);
    }
}

TEST_SUITE("Scene Metadata Serialization")
{
    TEST_CASE("Deserialize Scene Metadata")
    {
        duin::World world;
        duin::SceneBuilder sb(&world);

        std::string jsonStr = R"({
            "editorVersion": "1.0.5",
            "engineVersion": "0.2.1",
            "lastModified": "2025-01-16T14:30:00Z",
            "author": "TestAuthor"
        })";

        duin::JSONValue v = duin::JSONValue::Parse(jsonStr);
        duin::PackedSceneMetadata meta = sb.DeserializeMetadata(v);

        CHECK(meta.editorVersion == "1.0.5");
        CHECK(meta.engineVersion == "0.2.1");
        CHECK(meta.lastModified == "2025-01-16T14:30:00Z");
        CHECK(meta.author == "TestAuthor");
    }

    TEST_CASE("Serialize Scene Metadata")
    {
        duin::World world;
        duin::SceneBuilder sb(&world);

        duin::PackedSceneMetadata meta;
        meta.editorVersion = "2.0.0";
        meta.engineVersion = "1.0.0";
        meta.lastModified = "2025-01-17T10:00:00Z";
        meta.author = "SceneBuilder";

        duin::JSONValue json = sb.SerializeMetadata(meta);

        CHECK(json.IsObject());
        CHECK(json.HasMember("editorVersion"));
        CHECK(json["editorVersion"].GetString() == "2.0.0");
        CHECK(json.HasMember("engineVersion"));
        CHECK(json["engineVersion"].GetString() == "1.0.0");
        CHECK(json.HasMember("lastModified"));
        CHECK(json["lastModified"].GetString() == "2025-01-17T10:00:00Z");
        CHECK(json.HasMember("author"));
        CHECK(json["author"].GetString() == "SceneBuilder");
    }

    TEST_CASE("Round-trip Scene Metadata")
    {
        duin::World world;
        duin::SceneBuilder sb(&world);

        duin::PackedSceneMetadata original;
        original.editorVersion = "3.1.4";
        original.engineVersion = "2.5.9";
        original.lastModified = "2025-01-18T08:45:30Z";
        original.author = "RoundTripTest";

        duin::JSONValue json = sb.SerializeMetadata(original);
        duin::PackedSceneMetadata deserialized = sb.DeserializeMetadata(json);

        CHECK(original.editorVersion == deserialized.editorVersion);
        CHECK(original.engineVersion == deserialized.engineVersion);
        CHECK(original.lastModified == deserialized.lastModified);
        CHECK(original.author == deserialized.author);
    }

    TEST_CASE("Deserialize Partial Metadata")
    {
        duin::World world;
        duin::SceneBuilder sb(&world);

        std::string jsonStr = R"({
            "editorVersion": "1.0",
            "engineVersion": "0.1"
        })";

        duin::JSONValue v = duin::JSONValue::Parse(jsonStr);
        duin::PackedSceneMetadata meta = sb.DeserializeMetadata(v);

        CHECK(meta.editorVersion == "1.0");
        CHECK(meta.engineVersion == "0.1");
        CHECK(meta.lastModified.empty());
        CHECK(meta.author.empty());
    }
}

} // namespace TestSceneBuilder
