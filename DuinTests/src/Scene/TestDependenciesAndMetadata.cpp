#include "TestConfig.h"
#include "TestSceneBuilderCommon.h"
#include <doctest.h>
#include <Duin/Scene/SceneBuilder.h>
#include <Duin/Core/Utils/UUID.h>
#include <Duin/IO/JSONValue.h>
#include <Duin/ECS/ECSModule.h>
#include <Duin/Assets/AssetRef.h>
#include <Duin/Assets/FileTypes.h>

namespace TestSceneBuilder
{

TEST_SUITE("External Dependency Serialization")
{
    TEST_CASE("Serialize External Dependency with all fields")
    {
        duin::SceneBuilder sb;

        duin::PackedExternalDependency dep;
        dep.rPath = "wrk://scenes/enemy.pscn";
        dep.uuid = duin::UUID::FromStringHex("0xA1B2C3D4E5F67890");
        dep.fileType = duin::FS_FILETYPE_TEXT_EXT;
        dep.fileExt = duin::FS_FILEEXT_PSCN;

        duin::JSONValue json = sb.SerializeExternalDependency(dep);
        CHECK(json.IsObject());

        std::string written = json.Write();
        CHECK(written.find("wrk://scenes/enemy.pscn") != std::string::npos);
        CHECK(written.find("0xA1B2C3D4E5F67890") != std::string::npos);
    }

    TEST_CASE("Deserialize External Dependency with all fields")
    {
        duin::SceneBuilder sb;

        duin::PackedExternalDependency original;
        original.rPath = "wrk://data/playerScene.pscn";
        original.uuid = duin::UUID::FromStringHex("fedcba9876543210");
        original.fileType = duin::FS_FILETYPE_TEXT_EXT;
        original.fileExt = duin::FS_FILEEXT_PSCN;

        duin::JSONValue json = sb.SerializeExternalDependency(original);
        duin::PackedExternalDependency deserialized = sb.DeserializeExternalDependency(json);

        CHECK(deserialized.rPath == original.rPath);
        CHECK(deserialized.uuid == original.uuid);
        CHECK(deserialized.fileType == original.fileType);
        CHECK(deserialized.fileExt == original.fileExt);
    }

    TEST_CASE("Round-trip External Dependency")
    {
        duin::SceneBuilder sb;

        duin::PackedExternalDependency original;
        original.rPath = "bin://assets/world.pscn";
        original.uuid = duin::UUID::FromStringHex("fedcba9876543210");

        duin::JSONValue json = sb.SerializeExternalDependency(original);
        duin::PackedExternalDependency deserialized = sb.DeserializeExternalDependency(json);

        CHECK(original.rPath == deserialized.rPath);
        CHECK(original.uuid == deserialized.uuid);
    }

    TEST_CASE("Round-trip External Dependency with minimal fields")
    {
        duin::SceneBuilder sb;

        duin::PackedExternalDependency original;
        original.rPath = "wrk://minimal.pscn";

        duin::JSONValue json = sb.SerializeExternalDependency(original);
        duin::PackedExternalDependency deserialized = sb.DeserializeExternalDependency(json);

        CHECK(original.rPath == deserialized.rPath);
        CHECK(!deserialized.uuid.has_value());
        CHECK(!deserialized.fileType.has_value());
        CHECK(!deserialized.fileExt.has_value());
    }
}

TEST_SUITE("External Dependency - instanceOf on PackedEntity")
{
    TEST_CASE("Serialize entity with instanceOf")
    {
        duin::SceneBuilder sb;

        duin::PackedEntity pe;
        pe.name = "EnemyInstance";
        pe.enabled = true;

        duin::PackedExternalDependency dep;
        dep.rPath = "wrk://scenes/enemy.pscn";
        dep.uuid = duin::UUID::FromStringHex("1234567890abcdef");
        pe.instanceOf = dep;

        duin::JSONValue json = sb.SerializeEntity(pe);
        CHECK(json.HasMember(duin::PackedEntity::TAG_INSTANCEOF));

        duin::JSONValue instanceOfJson = json.GetMember(duin::PackedEntity::TAG_INSTANCEOF);
        std::string written = instanceOfJson.Write();
        CHECK(written.find("wrk://scenes/enemy.pscn") != std::string::npos);
    }

    TEST_CASE("Serialize entity without instanceOf omits field")
    {
        duin::SceneBuilder sb;

        duin::PackedEntity pe;
        pe.name = "PlainEntity";
        pe.enabled = true;

        duin::JSONValue json = sb.SerializeEntity(pe);
        CHECK_FALSE(json.HasMember(duin::PackedEntity::TAG_INSTANCEOF));
    }

    TEST_CASE("Deserialize entity with instanceOf")
    {
        duin::SceneBuilder sb;

        duin::PackedEntity original;
        original.name = "EnemyInstance";
        original.enabled = true;

        duin::PackedExternalDependency dep;
        dep.rPath = "wrk://scenes/enemy.pscn";
        dep.uuid = duin::UUID::FromStringHex("abcdef1234567890");
        original.instanceOf = dep;

        duin::JSONValue json = sb.SerializeEntity(original);
        duin::PackedEntity deserialized = sb.DeserializeEntity(json);

        REQUIRE(deserialized.instanceOf.has_value());
        CHECK(deserialized.instanceOf->rPath == "wrk://scenes/enemy.pscn");
        CHECK(deserialized.instanceOf->uuid == duin::UUID::FromStringHex("abcdef1234567890"));
    }

    TEST_CASE("Deserialize entity without instanceOf leaves it empty")
    {
        duin::SceneBuilder sb;

        duin::PackedEntity original;
        original.name = "PlainEntity";
        original.enabled = true;

        duin::JSONValue json = sb.SerializeEntity(original);
        duin::PackedEntity deserialized = sb.DeserializeEntity(json);

        CHECK_FALSE(deserialized.instanceOf.has_value());
    }
}

TEST_SUITE("ValidateExternalDependency")
{
    TEST_CASE("Valid dependency with non-empty rPath")
    {
        duin::SceneBuilder sb;
        duin::PackedExternalDependency dep;
        dep.rPath = "wrk://scenes/enemy.pscn";
        CHECK(sb.ValidateExternalDependency(dep) == true);
    }

    TEST_CASE("Invalid dependency with empty rPath")
    {
        duin::SceneBuilder sb;
        duin::PackedExternalDependency dep;
        dep.rPath = "";
        CHECK(sb.ValidateExternalDependency(dep) == false);
    }

    TEST_CASE("Invalid default-constructed dependency")
    {
        duin::SceneBuilder sb;
        duin::PackedExternalDependency dep;
        CHECK(sb.ValidateExternalDependency(dep) == false);
    }
}

TEST_SUITE("Scene Metadata Serialization")
{
    TEST_CASE("Deserialize Scene Metadata")
    {
        duin::World world;
        duin::SceneBuilder sb;

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
        duin::SceneBuilder sb;

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
        duin::SceneBuilder sb;

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
        duin::SceneBuilder sb;

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
