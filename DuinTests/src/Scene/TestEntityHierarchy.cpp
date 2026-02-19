#include "TestConfig.h"
#include "TestSceneBuilderCommon.h"
#include <doctest.h>
#include <Duin/Scene/SceneBuilder.h>
#include <Duin/Core/Utils/UUID.h>
#include <Duin/IO/JSONValue.h>
#include <Duin/ECS/ECSModule.h>

namespace TestSceneBuilder
{

TEST_SUITE("Full Entity Hierarchy Serialization")
{
    TEST_CASE("Deserialize Entity with Children")
    {
        duin::JSONValue v = duin::JSONValue::Parse(FULL_ENTITY_JSON);
        duin::PackedEntity p = duin::PackedEntity::Deserialize(v);

        // Parent entity checks
        CHECK(p.uuid == duin::UUID::FromStringHex("f5a3e8d1c9b2a7f4"));
        CHECK(p.name == "PlayerCharacter");
        CHECK(p.enabled == true);
        CHECK(p.tags.size() == 3);
        CHECK(p.components.size() == 3);
        CHECK(p.children.size() == 2);

        if (p.children.size() >= 2)
        {
            // First child (Camera)
            duin::PackedEntity &camera = p.children[0];
            CHECK(camera.uuid == duin::UUID::FromStringHex("1a2b3c4d5e6f7890"));
            CHECK(camera.name == "Camera");
            CHECK(camera.enabled == true);
            CHECK(camera.tags.size() == 1);
            CHECK(camera.components.size() == 2);
            CHECK(camera.children.empty());

            // Second child (WeaponSlot)
            duin::PackedEntity &weapon = p.children[1];
            CHECK(weapon.uuid == duin::UUID::FromStringHex("9876543210abcdef"));
            CHECK(weapon.name == "WeaponSlot");
            CHECK(weapon.enabled == false);
            CHECK(weapon.tags.size() == 1);
            CHECK(weapon.components.size() == 1);
            CHECK(weapon.children.empty());
        }
    }


    TEST_CASE("Serialize Entity with Children")
    {
        // Create parent entity
        duin::PackedEntity parent;
        parent.uuid = duin::UUID::FromStringHex("f5a3e8d1c9b2a7f4");
        parent.name = "PlayerCharacter";
        parent.enabled = true;
        
        // Add tags as PackedComponents
        duin::PackedComponent tag1;
        tag1.componentTypeName = "player";
        tag1.jsonData = R"({"type":"player"})";
        parent.tags.push_back(tag1);
        
        duin::PackedComponent tag2;
        tag2.componentTypeName = "controllable";
        tag2.jsonData = R"({"type":"controllable"})";
        parent.tags.push_back(tag2);

        // Create child entity
        duin::PackedEntity child;
        child.uuid = duin::UUID::FromStringHex("1a2b3c4d5e6f7890");
        child.name = "Camera";
        child.enabled = true;


        parent.children.push_back(child);

        // Serialize
        duin::JSONValue json = duin::PackedEntity::Serialize(parent);

        CHECK(json.IsObject());
        CHECK(json["uuid"].GetString() == duin::UUID::FromStringHex("f5a3e8d1c9b2a7f4").ToStrHex());
        CHECK(json["name"].GetString() == "PlayerCharacter");
        CHECK(json["enabled"].GetBool() == true);
        CHECK(json["tags"].IsArray());
        CHECK(json["tags"].Capacity() >= 2);
        CHECK(json["children"].IsArray());
        CHECK(json["children"].Capacity() == 1);

        // Verify child
        duin::JSONValue childJson = json["children"][0];
        CHECK(childJson["uuid"].GetString() == duin::UUID::FromStringHex("1a2b3c4d5e6f7890").ToStrHex());
        CHECK(childJson["name"].GetString() == "Camera");
    }

    TEST_CASE("Round-trip Entity with Children")
    {
        // Create original hierarchy
        duin::PackedEntity original;
        original.uuid = duin::UUID::FromStringHex("abcd1234efgh5678");
        original.name = "Parent";
        original.enabled = true;

        duin::PackedEntity child;
        child.uuid = duin::UUID::FromStringHex("1111222233334444");
        child.name = "Child";
        child.enabled = false;

        original.children.push_back(child);

        // Round-trip
        duin::JSONValue json = duin::PackedEntity::Serialize(original);
        duin::PackedEntity deserialized = duin::PackedEntity::Deserialize(json);

        CHECK(original.uuid == deserialized.uuid);
        CHECK(original.name == deserialized.name);
        CHECK(original.enabled == deserialized.enabled);
        CHECK(original.tags.size() == deserialized.tags.size());
        CHECK(original.children.size() == deserialized.children.size());
        if (original.children.size() >= 1 && deserialized.children.size() >= 1)
        {
            CHECK(original.children[0].uuid == deserialized.children[0].uuid);
            CHECK(original.children[0].name == deserialized.children[0].name);
        }
    }
}

} // namespace TestSceneBuilder
