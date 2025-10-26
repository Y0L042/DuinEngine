#include <doctest.h>
#include <Duin/Scene/SceneDefinition.h>
#include <Duin/IO/JSONValue.h>
#include <string>
#include <vector>

TEST_SUITE("SceneDefinition") {
    TEST_CASE("Default Constructor") {
        duin::SceneDefinition sceneDef;

        CHECK(sceneDef.uuid == duin::UUID::INVALID);
        CHECK(sceneDef.name.empty());
        CHECK(sceneDef.extScnDeps.empty());
        CHECK(sceneDef.flecsJSON.empty());
    }

    TEST_CASE("Constructor with JSONValue - Valid JSON") {
        // Create a valid JSONValue with 64-bit UUIDs
        std::string jsonString = R"({
            "UUID": "ABCDEF1234567890",
            "NAME": "TestScene",
            "EXT_SCN_DEPS": [
                {
                    "EXDEPUUID": "1234567890ABCDEF",
                    "LOCALPARENT": 42,
                    "TYPE": "DependencyType"
                }
            ],
            "FLECSJSON": "{\"key\": \"value\"}"
        })";

        duin::JSONValue json = duin::JSONValue::Parse(jsonString);
        REQUIRE(json.IsValid());

        // Construct SceneDefinition
        duin::SceneDefinition sceneDef(json);

        // Validate fields
        CHECK(sceneDef.uuid == duin::UUID::FromStringHex("ABCDEF1234567890"));
        CHECK(sceneDef.name == "TestScene");
        CHECK(sceneDef.extScnDeps[0].exDepUUID == duin::UUID::FromStringHex("1234567890ABCDEF"));
        CHECK(sceneDef.extScnDeps[0].localParent == 42);
        CHECK(sceneDef.extScnDeps[0].type == "DependencyType");
        CHECK(sceneDef.flecsJSON == "{\"key\": \"value\"}");
    }

    TEST_CASE("Constructor with JSONValue - Valid JSON with Delimiters") {
        // Create a valid JSONValue with 64-bit UUIDs containing dash delimiters
        std::string jsonString = R"({
            "UUID": "ABCD-EF12-3456-7890",
            "NAME": "TestScene",
            "EXT_SCN_DEPS": [
                {
                    "EXDEPUUID": "1234-5678-90AB-CDEF",
                    "LOCALPARENT": 42,
                    "TYPE": "DependencyType"
                }
            ],
            "FLECSJSON": "{\"key\": \"value\"}"
        })";

        duin::JSONValue json = duin::JSONValue::Parse(jsonString);
        REQUIRE(json.IsValid());

        // Construct SceneDefinition
        duin::SceneDefinition sceneDef(json);

        // Validate fields
        CHECK(sceneDef.uuid == duin::UUID::FromStringHex("ABCD-EF12-3456-7890"));
        CHECK(sceneDef.name == "TestScene");
        CHECK(sceneDef.extScnDeps[0].exDepUUID == duin::UUID::FromStringHex("1234-5678-90AB-CDEF"));
        CHECK(sceneDef.extScnDeps[0].localParent == 42);
        CHECK(sceneDef.extScnDeps[0].type == "DependencyType");
        CHECK(sceneDef.flecsJSON == "{\"key\": \"value\"}");
    }

    TEST_CASE("Constructor with JSONValue - Missing Fields") {
        // Create a JSONValue with missing fields
        std::string jsonString = R"({
            "NAME": "PartialScene"
        })";

        duin::JSONValue json = duin::JSONValue::Parse(jsonString);
        REQUIRE(json.IsValid());

        // Construct SceneDefinition
        duin::SceneDefinition sceneDef(json);

        // Validate fields
        CHECK(sceneDef.uuid == duin::UUID::INVALID); // Default UUID
        CHECK(sceneDef.name == "PartialScene");
        CHECK(sceneDef.extScnDeps.empty());
        CHECK(sceneDef.flecsJSON.empty());
    }

    TEST_CASE("Constructor with JSONValue - Invalid JSON") {
        // Create an invalid JSONValue
        duin::JSONValue json = duin::JSONValue::Invalid();
        REQUIRE_FALSE(json.IsValid());

        // Construct SceneDefinition
        duin::SceneDefinition sceneDef(json);

        // Validate fields
        CHECK(sceneDef.uuid == duin::UUID::INVALID); // Default UUID
        CHECK(sceneDef.name.empty());
        CHECK(sceneDef.extScnDeps.empty());
        CHECK(sceneDef.flecsJSON.empty());
    }
}


