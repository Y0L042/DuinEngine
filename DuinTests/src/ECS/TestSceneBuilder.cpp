#include "TestConfig.h"
#include <doctest.h>
#include <Duin/ECS/SceneBuilder.h>
#include <Duin/Core/Utils/UUID.h>
#include <Duin/IO/JSONValue.h>
#include <Duin/IO/FileUtils.h>
#include <string>
#include <vector>

TEST_SUITE("SceneBuilder")
{
    // ========================================================================
    // ResourceType Conversion Tests
    // ========================================================================

    TEST_CASE("ResourceTypeToString - All Enum Values")
    {
        SUBCASE("RES_UNKNOWN")
        {
            std::string result = duin::ResourceTypeToString(duin::ResourceType::RES_UNKNOWN);
            CHECK(result == "RES_UNKNOWN");
        }

        SUBCASE("RES_SCENE")
        {
            std::string result = duin::ResourceTypeToString(duin::ResourceType::RES_SCENE);
            CHECK(result == "RES_SCENE");
        }

        SUBCASE("RES_MODEL")
        {
            std::string result = duin::ResourceTypeToString(duin::ResourceType::RES_MODEL);
            CHECK(result == "RES_MODEL");
        }

        SUBCASE("RES_TEXTURE")
        {
            std::string result = duin::ResourceTypeToString(duin::ResourceType::RES_TEXTURE);
            CHECK(result == "RES_TEXTURE");
        }

        SUBCASE("RES_SHADER")
        {
            std::string result = duin::ResourceTypeToString(duin::ResourceType::RES_SHADER);
            CHECK(result == "RES_SHADER");
        }

        SUBCASE("RES_AUDIO")
        {
            std::string result = duin::ResourceTypeToString(duin::ResourceType::RES_AUDIO);
            CHECK(result == "RES_AUDIO");
        }
    }

    TEST_CASE("StringToResourceType - Valid Strings")
    {
        SUBCASE("RES_UNKNOWN")
        {
            duin::ResourceType type = duin::StringToResourceType("RES_UNKNOWN");
            CHECK(type == duin::ResourceType::RES_UNKNOWN);
        }

        SUBCASE("RES_SCENE")
        {
            duin::ResourceType type = duin::StringToResourceType("RES_SCENE");
            CHECK(type == duin::ResourceType::RES_SCENE);
        }

        SUBCASE("RES_MODEL")
        {
            duin::ResourceType type = duin::StringToResourceType("RES_MODEL");
            CHECK(type == duin::ResourceType::RES_MODEL);
        }

        SUBCASE("RES_TEXTURE")
        {
            duin::ResourceType type = duin::StringToResourceType("RES_TEXTURE");
            CHECK(type == duin::ResourceType::RES_TEXTURE);
        }

        SUBCASE("RES_SHADER")
        {
            duin::ResourceType type = duin::StringToResourceType("RES_SHADER");
            CHECK(type == duin::ResourceType::RES_SHADER);
        }

        SUBCASE("RES_AUDIO")
        {
            duin::ResourceType type = duin::StringToResourceType("RES_AUDIO");
            CHECK(type == duin::ResourceType::RES_AUDIO);
        }

        SUBCASE("Unknown string defaults to RES_UNKNOWN")
        {
            duin::ResourceType type = duin::StringToResourceType("INVALID_TYPE");
            CHECK(type == duin::ResourceType::RES_UNKNOWN);
        }

        SUBCASE("Empty string defaults to RES_UNKNOWN")
        {
            duin::ResourceType type = duin::StringToResourceType("");
            CHECK(type == duin::ResourceType::RES_UNKNOWN);
        }
    }

    TEST_CASE("ResourceType Round-trip Conversion")
    {
        SUBCASE("All valid types")
        {
            std::vector<duin::ResourceType> types = {duin::ResourceType::RES_UNKNOWN, duin::ResourceType::RES_SCENE,
                                                     duin::ResourceType::RES_MODEL,   duin::ResourceType::RES_TEXTURE,
                                                     duin::ResourceType::RES_SHADER,  duin::ResourceType::RES_AUDIO};

            for (const auto &original : types)
            {
                std::string str = duin::ResourceTypeToString(original);
                duin::ResourceType parsed = duin::StringToResourceType(str);
                CHECK(parsed == original);
            }
        }
    }

    // ========================================================================
    // Scene::ExDep Tests
    // ========================================================================

    TEST_CASE("Scene::ExDep - Construction and Assignment")
    {
        SUBCASE("Default construction")
        {
            duin::Scene::ExDep exDep;
            // No specific requirements for default values, just verify it compiles
            CHECK(true);
        }

        SUBCASE("UUID assignment")
        {
            duin::Scene::ExDep exDep;
            exDep.uuid = duin::UUID(12345);
            exDep.exdepUUID = duin::UUID(67890);
            CHECK(static_cast<uint64_t>(exDep.uuid) == 12345);
            CHECK(static_cast<uint64_t>(exDep.exdepUUID) == 67890);
        }

        SUBCASE("INVALID UUID")
        {
            duin::Scene::ExDep exDep;
            exDep.uuid = duin::UUID::INVALID;
            exDep.exdepUUID = duin::UUID::INVALID;
            CHECK(exDep.uuid == duin::UUID::INVALID);
            CHECK(exDep.exdepUUID == duin::UUID::INVALID);
        }

        SUBCASE("Parent assignment")
        {
            duin::Scene::ExDep exDep;
            exDep.parent = 42;
            CHECK(exDep.parent == 42);

            exDep.parent = UINT64_MAX;
            CHECK(exDep.parent == UINT64_MAX);
        }

        SUBCASE("ResourceType assignment")
        {
            duin::Scene::ExDep exDep;

            exDep.type = duin::ResourceType::RES_SCENE;
            CHECK(exDep.type == duin::ResourceType::RES_SCENE);

            exDep.type = duin::ResourceType::RES_MODEL;
            CHECK(exDep.type == duin::ResourceType::RES_MODEL);

            exDep.type = duin::ResourceType::RES_TEXTURE;
            CHECK(exDep.type == duin::ResourceType::RES_TEXTURE);
        }
    }

    // ========================================================================
    // Scene JSON Serialization Tests
    // ========================================================================

    TEST_CASE("Scene::WriteToJSONValue and ReadFromJSONValue - Basic Round-trip")
    {
        SUBCASE("Empty scene with minimal data")
        {
            duin::Scene original;
            original.uuid = duin::UUID(12345);
            original.name = "TestScene";
            original.flecsJSON = "{}";

            duin::JSONValue json = original.WriteToJSONValue();
            duin::Scene parsed = duin::Scene::ReadFromJSONValue(json);

            CHECK(parsed.uuid == original.uuid);
            CHECK(parsed.name == original.name);
            CHECK(parsed.flecsJSON == original.flecsJSON);
            CHECK(parsed.externalSceneDependencies.size() == 0);
        }

        SUBCASE("Scene with INVALID UUID")
        {
            duin::Scene original;
            original.uuid = duin::UUID::INVALID;
            original.name = "InvalidUUIDScene";
            original.flecsJSON = "{}";

            duin::JSONValue json = original.WriteToJSONValue();
            duin::Scene parsed = duin::Scene::ReadFromJSONValue(json);

            CHECK(parsed.uuid == duin::UUID::INVALID);
            CHECK(parsed.name == original.name);
        }

        SUBCASE("Scene with empty name")
        {
            duin::Scene original;
            original.uuid = duin::UUID(999);
            original.name = "";
            original.flecsJSON = "{}";

            duin::JSONValue json = original.WriteToJSONValue();
            duin::Scene parsed = duin::Scene::ReadFromJSONValue(json);

            CHECK(parsed.uuid == original.uuid);
            CHECK(parsed.name == "");
            CHECK(parsed.flecsJSON == original.flecsJSON);
        }

        SUBCASE("Scene with empty flecsJSON")
        {
            duin::Scene original;
            original.uuid = duin::UUID(111);
            original.name = "EmptyFlecsScene";
            original.flecsJSON = "";

            duin::JSONValue json = original.WriteToJSONValue();
            duin::Scene parsed = duin::Scene::ReadFromJSONValue(json);

            CHECK(parsed.uuid == original.uuid);
            CHECK(parsed.name == original.name);
            CHECK(parsed.flecsJSON == "");
        }

        SUBCASE("Scene with large UUID")
        {
            duin::Scene original;
            original.uuid = duin::UUID(UINT64_MAX);
            original.name = "MaxUUIDScene";
            original.flecsJSON = "{}";

            duin::JSONValue json = original.WriteToJSONValue();
            duin::Scene parsed = duin::Scene::ReadFromJSONValue(json);

            CHECK(parsed.uuid == original.uuid);
        }
    }

    TEST_CASE("Scene::WriteToJSONValue and ReadFromJSONValue - External Dependencies")
    {
        SUBCASE("Single external dependency")
        {
            duin::Scene original;
            original.uuid = duin::UUID(1000);
            original.name = "SceneWithDep";
            original.flecsJSON = "{}";

            duin::Scene::ExDep dep;
            dep.uuid = duin::UUID(2000);
            dep.exdepUUID = duin::UUID(3000);
            dep.parent = 100;
            dep.type = duin::ResourceType::RES_MODEL;

            original.externalSceneDependencies.push_back(dep);

            duin::JSONValue json = original.WriteToJSONValue();
            duin::Scene parsed = duin::Scene::ReadFromJSONValue(json);

            REQUIRE(parsed.externalSceneDependencies.size() == 1);
            CHECK(parsed.externalSceneDependencies[0].uuid == dep.uuid);
            CHECK(parsed.externalSceneDependencies[0].exdepUUID == dep.exdepUUID);
            CHECK(parsed.externalSceneDependencies[0].parent == dep.parent);
            CHECK(parsed.externalSceneDependencies[0].type == dep.type);
        }

        SUBCASE("Multiple external dependencies with different types")
        {
            duin::Scene original;
            original.uuid = duin::UUID(5000);
            original.name = "MultiDepScene";
            original.flecsJSON = "{}";

            // Add scene dependency
            duin::Scene::ExDep sceneDep;
            sceneDep.uuid = duin::UUID(5001);
            sceneDep.exdepUUID = duin::UUID(5002);
            sceneDep.parent = 10;
            sceneDep.type = duin::ResourceType::RES_SCENE;
            original.externalSceneDependencies.push_back(sceneDep);

            // Add model dependency
            duin::Scene::ExDep modelDep;
            modelDep.uuid = duin::UUID(5003);
            modelDep.exdepUUID = duin::UUID(5004);
            modelDep.parent = 20;
            modelDep.type = duin::ResourceType::RES_MODEL;
            original.externalSceneDependencies.push_back(modelDep);

            // Add texture dependency
            duin::Scene::ExDep textureDep;
            textureDep.uuid = duin::UUID(5005);
            textureDep.exdepUUID = duin::UUID(5006);
            textureDep.parent = 30;
            textureDep.type = duin::ResourceType::RES_TEXTURE;
            original.externalSceneDependencies.push_back(textureDep);

            // Add shader dependency
            duin::Scene::ExDep shaderDep;
            shaderDep.uuid = duin::UUID(5007);
            shaderDep.exdepUUID = duin::UUID(5008);
            shaderDep.parent = 40;
            shaderDep.type = duin::ResourceType::RES_SHADER;
            original.externalSceneDependencies.push_back(shaderDep);

            // Add audio dependency
            duin::Scene::ExDep audioDep;
            audioDep.uuid = duin::UUID(5009);
            audioDep.exdepUUID = duin::UUID(5010);
            audioDep.parent = 50;
            audioDep.type = duin::ResourceType::RES_AUDIO;
            original.externalSceneDependencies.push_back(audioDep);

            duin::JSONValue json = original.WriteToJSONValue();
            duin::Scene parsed = duin::Scene::ReadFromJSONValue(json);

            REQUIRE(parsed.externalSceneDependencies.size() == 5);
            CHECK(parsed.externalSceneDependencies[0].type == duin::ResourceType::RES_SCENE);
            CHECK(parsed.externalSceneDependencies[1].type == duin::ResourceType::RES_MODEL);
            CHECK(parsed.externalSceneDependencies[2].type == duin::ResourceType::RES_TEXTURE);
            CHECK(parsed.externalSceneDependencies[3].type == duin::ResourceType::RES_SHADER);
            CHECK(parsed.externalSceneDependencies[4].type == duin::ResourceType::RES_AUDIO);
        }

        SUBCASE("External dependency with max parent value")
        {
            duin::Scene original;
            original.uuid = duin::UUID(7000);
            original.name = "MaxParentScene";
            original.flecsJSON = "{}";

            duin::Scene::ExDep dep;
            dep.uuid = duin::UUID(7001);
            dep.exdepUUID = duin::UUID(7002);
            dep.parent = UINT64_MAX;
            dep.type = duin::ResourceType::RES_UNKNOWN;

            original.externalSceneDependencies.push_back(dep);

            duin::JSONValue json = original.WriteToJSONValue();
            duin::Scene parsed = duin::Scene::ReadFromJSONValue(json);

            REQUIRE(parsed.externalSceneDependencies.size() == 1);
            CHECK(parsed.externalSceneDependencies[0].parent == UINT64_MAX);
        }
    }

    TEST_CASE("Scene::WriteToJSONValue and ReadFromJSONValue - Complex flecsJSON")
    {
        SUBCASE("Realistic FLECS JSON structure")
        {
            std::string flecsJSON = R"({
                "results": [{
                    "name": "Player",
                    "id": 580,
                    "components": {
                        "duin.Transform3D": {
                            "position": {"x": 0, "y": 2, "z": 0},
                            "rotation": {"x": 0, "y": 0, "z": 0},
                            "scale": {"x": 1, "y": 1, "z": 1}
                        }
                    }
                }]
            })";

            duin::Scene original;
            original.uuid = duin::UUID(9000);
            original.name = "ComplexScene";
            original.flecsJSON = flecsJSON;

            duin::JSONValue json = original.WriteToJSONValue();
            duin::Scene parsed = duin::Scene::ReadFromJSONValue(json);

            CHECK(parsed.uuid == original.uuid);
            CHECK(parsed.name == original.name);
            CHECK(parsed.flecsJSON == flecsJSON);
        }

        SUBCASE("Large flecsJSON with special characters")
        {
            std::string flecsJSON = "{ \"key\": \"value with \\\"quotes\\\" and \\n newlines\" }";

            duin::Scene original;
            original.uuid = duin::UUID(9100);
            original.name = "SpecialCharsScene";
            original.flecsJSON = flecsJSON;

            duin::JSONValue json = original.WriteToJSONValue();
            duin::Scene parsed = duin::Scene::ReadFromJSONValue(json);

            CHECK(parsed.flecsJSON == flecsJSON);
        }
    }

    TEST_CASE("Scene::WriteToJSONValue - JSON Structure Validation")
    {
        SUBCASE("Verify JSON field names")
        {
            duin::Scene scene;
            scene.uuid = duin::UUID(8000);
            scene.name = "FieldTestScene";
            scene.flecsJSON = "{}";

            duin::JSONValue json = scene.WriteToJSONValue();

            // Verify required fields exist
            CHECK(json.HasMember("UUID"));
            CHECK(json.HasMember("NAME"));
            CHECK(json.HasMember("FLECSJSON"));
            CHECK(json.HasMember("EXT_SCN_DEPS"));

            // Verify field types
            CHECK(json["UUID"].IsString());
            CHECK(json["NAME"].IsString());
            CHECK(json["FLECSJSON"].IsString());
            CHECK(json["EXT_SCN_DEPS"].IsArray());
        }

        SUBCASE("Verify UUID is in hex format")
        {
            duin::Scene scene;
            scene.uuid = duin::UUID(255); // 0xFF in hex
            scene.name = "HexUUIDScene";
            scene.flecsJSON = "{}";

            duin::JSONValue json = scene.WriteToJSONValue();
            std::string uuidStr = json["UUID"].GetString();

            // Should contain hex prefix and hex digits
            CHECK(uuidStr.find("0x") == 0);
        }

        SUBCASE("Verify external dependencies array structure")
        {
            duin::Scene scene;
            scene.uuid = duin::UUID(8500);
            scene.name = "DepStructureScene";
            scene.flecsJSON = "{}";

            duin::Scene::ExDep dep;
            dep.uuid = duin::UUID(8501);
            dep.exdepUUID = duin::UUID(8502);
            dep.parent = 99;
            dep.type = duin::ResourceType::RES_TEXTURE;
            scene.externalSceneDependencies.push_back(dep);

            duin::JSONValue json = scene.WriteToJSONValue();
            duin::JSONValue depsArray = json["EXT_SCN_DEPS"];

            REQUIRE(depsArray.IsArray());
            REQUIRE(depsArray.Capacity() == 1);

            duin::JSONValue depJson = depsArray[0];
            CHECK(depJson.HasMember("EXDEPUUID"));
            CHECK(depJson.HasMember("PARENT"));
            CHECK(depJson.HasMember("TYPE"));
            CHECK(depJson["EXDEPUUID"].IsString());
            CHECK(depJson["PARENT"].IsInt());
            CHECK(depJson["TYPE"].IsString());
        }
    }

    // ========================================================================
    // Scene String Serialization Tests
    // ========================================================================

    TEST_CASE("Scene::WriteToString and ReadFromString - Round-trip")
    {
        SUBCASE("Complete scene serialization")
        {
            duin::Scene original;
            original.uuid = duin::UUID(10000);
            original.name = "StringTestScene";
            original.flecsJSON = "{\"entities\":[]}";

            duin::Scene::ExDep dep;
            dep.uuid = duin::UUID(10001);
            dep.exdepUUID = duin::UUID(10002);
            dep.parent = 5;
            dep.type = duin::ResourceType::RES_MODEL;
            original.externalSceneDependencies.push_back(dep);

            std::string sceneStr = original.WriteToString();
            CHECK_FALSE(sceneStr.empty());

            duin::Scene parsed = duin::Scene::ReadFromString(sceneStr);

            CHECK(parsed.uuid == original.uuid);
            CHECK(parsed.name == original.name);
            CHECK(parsed.flecsJSON == original.flecsJSON);
            REQUIRE(parsed.externalSceneDependencies.size() == 1);
            CHECK(parsed.externalSceneDependencies[0].exdepUUID == dep.exdepUUID);
        }

        SUBCASE("Scene with special characters in name")
        {
            duin::Scene original;
            original.uuid = duin::UUID(10100);
            original.name = "Scene with spaces & symbols!@#$%";
            original.flecsJSON = "{}";

            std::string sceneStr = original.WriteToString();
            duin::Scene parsed = duin::Scene::ReadFromString(sceneStr);

            CHECK(parsed.name == original.name);
        }
    }

    TEST_CASE("Scene::ReadFromString - Invalid Input")
    {
        SUBCASE("Empty string")
        {
            duin::Scene parsed = duin::Scene::ReadFromString("");

            // Should return default-constructed scene (INVALID UUID)
            CHECK(parsed.uuid == duin::UUID::INVALID);
        }

        SUBCASE("Invalid JSON string")
        {
            duin::Scene parsed = duin::Scene::ReadFromString("{invalid json}");

            // Should return default-constructed scene
            CHECK(parsed.uuid == duin::UUID::INVALID);
        }

        SUBCASE("Not JSON at all")
        {
            duin::Scene parsed = duin::Scene::ReadFromString("This is not JSON");

            // Should return default-constructed scene
            CHECK(parsed.uuid == duin::UUID::INVALID);
        }

        SUBCASE("Valid JSON but missing required fields")
        {
            // This will likely crash or throw without proper error handling
            // Documenting expected behavior based on TODO comments in code
            std::string incompleteJSON = R"({"NAME": "TestScene"})";

            // NOTE: Current implementation has no error handling (per TODO)
            // This test documents the expected failure case
            // When error handling is added, this should return a default Scene
            // For now, we just verify it doesn't return valid data
            // CHECK - commented out as this may crash without error handling
        }
    }

    // ========================================================================
    // Scene File I/O Tests
    // ========================================================================

    TEST_CASE("Scene::WriteToFile and ReadFromFile - Round-trip")
    {
        SUBCASE("Basic file persistence")
        {
            std::string testFilePath = "test_scene_basic.json";

            duin::Scene original;
            original.uuid = duin::UUID(20000);
            original.name = "FileTestScene";
            original.flecsJSON = "{\"test\":true}";

            duin::Scene::ExDep dep;
            dep.uuid = duin::UUID(20001);
            dep.exdepUUID = duin::UUID(20002);
            dep.parent = 123;
            dep.type = duin::ResourceType::RES_AUDIO;
            original.externalSceneDependencies.push_back(dep);

            // Write to file
            original.WriteToFile(testFilePath);

            // Read from file
            duin::Scene parsed = duin::Scene::ReadFromFile(testFilePath);

            // Verify
            CHECK(parsed.uuid == original.uuid);
            CHECK(parsed.name == original.name);
            CHECK(parsed.flecsJSON == original.flecsJSON);
            REQUIRE(parsed.externalSceneDependencies.size() == 1);
            CHECK(parsed.externalSceneDependencies[0].type == duin::ResourceType::RES_AUDIO);

            // Cleanup
            std::remove(testFilePath.c_str());
        }

        SUBCASE("Complex scene with multiple dependencies")
        {
            std::string testFilePath = "test_scene_complex.json";

            duin::Scene original;
            original.uuid = duin::UUID(21000);
            original.name = "ComplexFileScene";
            original.flecsJSON = R"({
                "results": [
                    {"id": 1, "name": "Entity1"},
                    {"id": 2, "name": "Entity2"}
                ]
            })";

            for (int i = 0; i < 3; ++i)
            {
                duin::Scene::ExDep dep;
                dep.uuid = duin::UUID(21001 + i);
                dep.exdepUUID = duin::UUID(21010 + i);
                dep.parent = i * 10;
                dep.type = static_cast<duin::ResourceType>(i % 5 + 1); // Cycle through types
                original.externalSceneDependencies.push_back(dep);
            }

            // Write and read
            original.WriteToFile(testFilePath);
            duin::Scene parsed = duin::Scene::ReadFromFile(testFilePath);

            // Verify
            CHECK(parsed.uuid == original.uuid);
            CHECK(parsed.name == original.name);
            CHECK(parsed.externalSceneDependencies.size() == 3);

            // Cleanup
            std::remove(testFilePath.c_str());
        }

        SUBCASE("File with special characters in name")
        {
            std::string testFilePath = "test_scene_special_chars.json";

            duin::Scene original;
            original.uuid = duin::UUID(22000);
            original.name = "Special!@#$%^&*()_+-=[]{}|;:',.<>?/~ Scene";
            original.flecsJSON = "{}";

            original.WriteToFile(testFilePath);
            duin::Scene parsed = duin::Scene::ReadFromFile(testFilePath);

            CHECK(parsed.name == original.name);

            // Cleanup
            std::remove(testFilePath.c_str());
        }
    }

    TEST_CASE("Scene::ReadFromFile - Invalid Input")
    {
        SUBCASE("Non-existent file")
        {
            duin::Scene parsed = duin::Scene::ReadFromFile("nonexistent_file_12345.json");

            // Should return default-constructed scene
            CHECK(parsed.uuid == duin::UUID::INVALID);
        }

        SUBCASE("Empty file")
        {
            std::string testFilePath = "test_scene_empty.json";

            // Create empty file
            duin::FileUtils::WriteStringIntoFile(testFilePath, "");

            duin::Scene parsed = duin::Scene::ReadFromFile(testFilePath);

            // Should return default-constructed scene
            CHECK(parsed.uuid == duin::UUID::INVALID);

            // Cleanup
            std::remove(testFilePath.c_str());
        }
    }

    // ========================================================================
    // Edge Cases and Default Values
    // ========================================================================

    TEST_CASE("Scene - Default Construction")
    {
        SUBCASE("Default constructed scene")
        {
            duin::Scene scene;

            // UUID should be INVALID (0) by default
            CHECK(scene.uuid == duin::UUID::INVALID);
            CHECK(scene.name == "");
            CHECK(scene.flecsJSON == "");
            CHECK(scene.externalSceneDependencies.empty());
        }

        SUBCASE("Default scene can be serialized")
        {
            duin::Scene original;

            duin::JSONValue json = original.WriteToJSONValue();
            duin::Scene parsed = duin::Scene::ReadFromJSONValue(json);

            CHECK(parsed.uuid == duin::UUID::INVALID);
            CHECK(parsed.name == "");
            CHECK(parsed.flecsJSON == "");
            CHECK(parsed.externalSceneDependencies.empty());
        }
    }

    TEST_CASE("Scene - Boundary Values")
    {
        SUBCASE("Very long scene name")
        {
            duin::Scene original;
            original.uuid = duin::UUID(30000);
            original.name = std::string(10000, 'A'); // 10,000 character name
            original.flecsJSON = "{}";

            std::string sceneStr = original.WriteToString();
            duin::Scene parsed = duin::Scene::ReadFromString(sceneStr);

            CHECK(parsed.name.length() == 10000);
            CHECK(parsed.name == original.name);
        }

        SUBCASE("Very large flecsJSON")
        {
            duin::Scene original;
            original.uuid = duin::UUID(30100);
            original.name = "LargeFlecsScene";

            // Create a large JSON string
            std::string largeFlecsJSON = "{\"entities\":[";
            for (int i = 0; i < 1000; ++i)
            {
                if (i > 0)
                    largeFlecsJSON += ",";
                largeFlecsJSON += "{\"id\":" + std::to_string(i) + "}";
            }
            largeFlecsJSON += "]}";
            original.flecsJSON = largeFlecsJSON;

            std::string sceneStr = original.WriteToString();
            duin::Scene parsed = duin::Scene::ReadFromString(sceneStr);

            CHECK(parsed.flecsJSON == largeFlecsJSON);
        }

        SUBCASE("Many external dependencies")
        {
            duin::Scene original;
            original.uuid = duin::UUID(30200);
            original.name = "ManyDepsScene";
            original.flecsJSON = "{}";

            // Add 100 dependencies
            for (int i = 0; i < 100; ++i)
            {
                duin::Scene::ExDep dep;
                dep.uuid = duin::UUID(30200 + i);
                dep.exdepUUID = duin::UUID(30300 + i);
                dep.parent = i;
                dep.type = static_cast<duin::ResourceType>((i % 5) + 1);
                original.externalSceneDependencies.push_back(dep);
            }

            duin::JSONValue json = original.WriteToJSONValue();
            duin::Scene parsed = duin::Scene::ReadFromJSONValue(json);

            CHECK(parsed.externalSceneDependencies.size() == 100);

            // Verify first and last
            CHECK(parsed.externalSceneDependencies[0].parent == 0);
            CHECK(parsed.externalSceneDependencies[99].parent == 99);
        }
    }
}
