/**
 * @file JSONValue.cpp
 * @brief Comprehensive usage examples for duin::JSONValue class
 * @author DuinEngine Contributors
 * @date 2024
 *
 * This file demonstrates various use cases of the JSONValue class for JSON manipulation,
 * including parsing, creation, modification, serialization, and iteration.
 */

#include <Duin/IO/JSONValue.h>
#include <iostream>
#include <string>

// Forward declarations of example functions
void example_basic_construction();
void example_parsing_json();
void example_object_manipulation();
void example_array_operations();
void example_nested_structures();
void example_iteration();
void example_type_checking();
void example_serialization();
void example_cloning();
void example_file_operations();
void example_real_world_config();
void example_real_world_game_entity();
void example_error_handling();

int main()
{
    std::cout << "=== duin::JSONValue Comprehensive Examples ===\n\n";

    example_basic_construction();
    example_parsing_json();
    example_object_manipulation();
    example_array_operations();
    example_nested_structures();
    example_iteration();
    example_type_checking();
    example_serialization();
    example_cloning();
    example_file_operations();
    example_real_world_config();
    example_real_world_game_entity();
    example_error_handling();

    std::cout << "\n=== All Examples Complete ===\n";
    return 0;
}

/**
 * @brief Example 1: Basic Construction
 * Demonstrates creating JSONValue objects with different types
 */
void example_basic_construction()
{
    std::cout << "--- Example 1: Basic Construction ---\n";

    // Default constructor creates an empty object
    duin::JSONValue emptyObj;
    std::cout << "Empty object: " << emptyObj.Write() << "\n";

    // Construct with primitive types
    duin::JSONValue boolValue(true);
    duin::JSONValue intValue(42);
    duin::JSONValue doubleValue(3.14159);
    duin::JSONValue stringValue(std::string("Hello, World!"));

    std::cout << "Boolean: " << boolValue.Write() << "\n";
    std::cout << "Integer: " << intValue.Write() << "\n";
    std::cout << "Double: " << doubleValue.Write() << "\n";
    std::cout << "String: " << stringValue.Write() << "\n";

    // Create invalid JSONValue
    auto invalid = duin::JSONValue::Invalid();
    std::cout << "Is valid: " << (invalid.IsValid() ? "true" : "false") << "\n";

    std::cout << "\n";
}

/**
 * @brief Example 2: Parsing JSON
 * Demonstrates parsing JSON strings and handling different JSON structures
 */
void example_parsing_json()
{
    std::cout << "--- Example 2: Parsing JSON ---\n";

    // Parse simple object
    std::string jsonObject = R"({"name": "DuinEngine", "version": 1, "active": true})";
    duin::JSONValue obj = duin::JSONValue::Parse(jsonObject);

    std::cout << "Parsed object:\n";
    std::cout << "  name: " << obj["name"].GetString() << "\n";
    std::cout << "  version: " << obj["version"].GetInt() << "\n";
    std::cout << "  active: " << (obj["active"].GetBool() ? "true" : "false") << "\n";

    // Parse array
    std::string jsonArray = R"([1, 2, 3, 4, 5])";
    duin::JSONValue arr = duin::JSONValue::Parse(jsonArray);

    std::cout << "Parsed array: " << arr.Write() << "\n";
    std::cout << "  First element: " << arr[0].GetInt() << "\n";
    std::cout << "  Last element: " << arr[4].GetInt() << "\n";

    // Parse nested structure
    std::string jsonNested = R"({
        "user": {
            "name": "Alice",
            "age": 30
        },
        "scores": [95, 87, 92]
    })";
    duin::JSONValue nested = duin::JSONValue::Parse(jsonNested);

    std::cout << "Nested structure:\n";
    std::cout << "  User name: " << nested["user"]["name"].GetString() << "\n";
    std::cout << "  First score: " << nested["scores"][0].GetInt() << "\n";

    std::cout << "\n";
}

/**
 * @brief Example 3: Object Manipulation
 * Demonstrates adding, modifying, and removing object members
 */
void example_object_manipulation()
{
    std::cout << "--- Example 3: Object Manipulation ---\n";

    duin::JSONValue config;

    // Add members with primitive types
    config.AddMember("appName", std::string("MyApp"));
    config.AddMember("version", 1);
    config.AddMember("debugMode", true);
    config.AddMember("framerate", 60.0);

    std::cout << "Initial config:\n" << config.Write(true) << "\n";

    // Check if member exists
    if (config.HasMember("version")) {
        std::cout << "Version exists: " << config["version"].GetInt() << "\n";
    }

    // Modify existing member (without duplicates)
    config.AddMember("version", 2, false); // false = don't allow duplicates
    std::cout << "Updated version: " << config["version"].GetInt() << "\n";

    // Add nested object
    duin::JSONValue graphics;
    graphics.AddMember("width", 1920);
    graphics.AddMember("height", 1080);
    graphics.AddMember("fullscreen", true);

    config.AddMember("graphics", graphics);

    std::cout << "Config with graphics:\n" << config.Write(true) << "\n";

    // Remove member
    config.RemoveMember("debugMode");
    std::cout << "After removing debugMode:\n" << config.Write(true) << "\n";

    // Get member using GetMember method
    duin::JSONValue graphicsSettings = config.GetMember("graphics");
    std::cout << "Graphics width: " << graphicsSettings["width"].GetInt() << "\n";

    std::cout << "\n";
}

/**
 * @brief Example 4: Array Operations
 * Demonstrates creating and manipulating JSON arrays
 */
void example_array_operations()
{
    std::cout << "--- Example 4: Array Operations ---\n";

    // Create array and add primitives
    duin::JSONValue numbers;
    numbers.SetArray();

    numbers.PushBack(10);
    numbers.PushBack(20);
    numbers.PushBack(30);
    numbers.PushBack(40);
    numbers.PushBack(50);

    std::cout << "Numbers array: " << numbers.Write() << "\n";
    std::cout << "Array is empty: " << (numbers.Empty() ? "true" : "false") << "\n";
    std::cout << "Array capacity: " << numbers.Capacity() << "\n";

    // Access elements
    std::cout << "Element at index 2: " << numbers[2].GetInt() << "\n";

    // Array with mixed types (via JSONValue wrapper)
    duin::JSONValue mixed;
    mixed.SetArray();

    mixed.PushBack(duin::JSONValue(42));
    mixed.PushBack(duin::JSONValue(std::string("text")));
    mixed.PushBack(duin::JSONValue(true));
    mixed.PushBack(duin::JSONValue(3.14));

    std::cout << "Mixed array: " << mixed.Write() << "\n";
    std::cout << "  Element 0 is int: " << (mixed[0].IsInt() ? "true" : "false") << "\n";
    std::cout << "  Element 1 is string: " << (mixed[1].IsString() ? "true" : "false") << "\n";
    std::cout << "  Element 2 is bool: " << (mixed[2].IsBool() ? "true" : "false") << "\n";
    std::cout << "  Element 3 is double: " << (mixed[3].IsDouble() ? "true" : "false") << "\n";

    // Array of objects
    duin::JSONValue users;
    users.SetArray();

    duin::JSONValue user1;
    user1.AddMember("id", 1);
    user1.AddMember("name", std::string("Alice"));
    user1.AddMember("active", true);

    duin::JSONValue user2;
    user2.AddMember("id", 2);
    user2.AddMember("name", std::string("Bob"));
    user2.AddMember("active", false);

    users.PushBack(user1);
    users.PushBack(user2);

    std::cout << "Users array:\n" << users.Write(true) << "\n";
    std::cout << "First user name: " << users[0]["name"].GetString() << "\n";

    std::cout << "\n";
}

/**
 * @brief Example 5: Nested Structures
 * Demonstrates complex nested JSON structures
 */
void example_nested_structures()
{
    std::cout << "--- Example 5: Nested Structures ---\n";

    // Create deeply nested structure
    duin::JSONValue scene;
    scene.AddMember("name", std::string("MainScene"));
    scene.AddMember("id", 1);

    // Add entities array
    duin::JSONValue entities;
    entities.SetArray();

    // Create entity 1
    duin::JSONValue entity1;
    entity1.AddMember("name", std::string("Player"));

    duin::JSONValue transform1;
    transform1.AddMember("x", 100.0);
    transform1.AddMember("y", 200.0);
    transform1.AddMember("rotation", 0.0);
    entity1.AddMember("transform", transform1);

    duin::JSONValue components1;
    components1.SetArray();
    components1.PushBack(duin::JSONValue(std::string("MeshRenderer")));
    components1.PushBack(duin::JSONValue(std::string("PlayerController")));
    entity1.AddMember("components", components1);

    // Create entity 2
    duin::JSONValue entity2;
    entity2.AddMember("name", std::string("Enemy"));

    duin::JSONValue transform2;
    transform2.AddMember("x", 300.0);
    transform2.AddMember("y", 150.0);
    transform2.AddMember("rotation", 45.0);
    entity2.AddMember("transform", transform2);

    duin::JSONValue components2;
    components2.SetArray();
    components2.PushBack(duin::JSONValue(std::string("MeshRenderer")));
    components2.PushBack(duin::JSONValue(std::string("AIController")));
    entity2.AddMember("components", components2);

    entities.PushBack(entity1);
    entities.PushBack(entity2);
    scene.AddMember("entities", entities);

    std::cout << "Scene structure:\n" << scene.Write(true) << "\n";

    // Access nested values
    std::cout << "Player position: ("
              << scene["entities"][0]["transform"]["x"].GetDouble() << ", "
              << scene["entities"][0]["transform"]["y"].GetDouble() << ")\n";

    std::cout << "Enemy first component: "
              << scene["entities"][1]["components"][0].GetString() << "\n";

    std::cout << "\n";
}

/**
 * @brief Example 6: Iteration
 * Demonstrates iterating over JSON arrays
 */
void example_iteration()
{
    std::cout << "--- Example 6: Iteration ---\n";

    // Create array of numbers
    duin::JSONValue numbers;
    numbers.SetArray();
    for (int i = 1; i <= 5; ++i) {
        numbers.PushBack(i * 10);
    }

    // Iterate using mutable iterators
    std::cout << "Array elements (mutable iterator): ";
    for (auto it = numbers.begin(); it != numbers.end(); ++it) {
        std::cout << (*it).GetInt() << " ";
    }
    std::cout << "\n";

    // Calculate sum using iteration
    int sum = 0;
    for (auto it = numbers.begin(); it != numbers.end(); ++it) {
        sum += (*it).GetInt();
    }
    std::cout << "Sum of elements: " << sum << "\n";

    // Const iteration
    const duin::JSONValue& constNumbers = numbers;
    std::cout << "Array elements (const iterator): ";
    for (auto it = constNumbers.begin(); it != constNumbers.end(); ++it) {
        std::cout << (*it).GetInt() << " ";
    }
    std::cout << "\n";

    // Iterate over array of objects
    duin::JSONValue items;
    items.SetArray();

    for (int i = 0; i < 3; ++i) {
        duin::JSONValue item;
        item.AddMember("id", i);
        item.AddMember("value", i * 100);
        items.PushBack(item);
    }

    std::cout << "Items:\n";
    int index = 0;
    for (auto it = items.begin(); it != items.end(); ++it) {
        duin::JSONValue current = *it;
        std::cout << "  Item " << index++ << ": id="
                  << current["id"].GetInt() << ", value="
                  << current["value"].GetInt() << "\n";
    }

    std::cout << "\n";
}

/**
 * @brief Example 7: Type Checking
 * Demonstrates type checking and safe type access
 */
void example_type_checking()
{
    std::cout << "--- Example 7: Type Checking ---\n";

    // Create values of different types
    duin::JSONValue obj;
    obj.AddMember("name", std::string("Test"));
    obj.AddMember("count", 42);
    obj.AddMember("ratio", 0.75);
    obj.AddMember("enabled", true);

    duin::JSONValue arr;
    arr.SetArray();
    arr.PushBack(1);

    // Check types
    std::cout << "obj is object: " << (obj.IsObject() ? "true" : "false") << "\n";
    std::cout << "arr is array: " << (arr.IsArray() ? "true" : "false") << "\n";

    std::cout << "obj['name'] is string: " << (obj["name"].IsString() ? "true" : "false") << "\n";
    std::cout << "obj['count'] is int: " << (obj["count"].IsInt() ? "true" : "false") << "\n";
    std::cout << "obj['count'] is number: " << (obj["count"].IsNumber() ? "true" : "false") << "\n";
    std::cout << "obj['ratio'] is double: " << (obj["ratio"].IsDouble() ? "true" : "false") << "\n";
    std::cout << "obj['ratio'] is number: " << (obj["ratio"].IsNumber() ? "true" : "false") << "\n";
    std::cout << "obj['enabled'] is bool: " << (obj["enabled"].IsBool() ? "true" : "false") << "\n";

    // Safe type access pattern
    duin::JSONValue value = obj["count"];
    if (value.IsInt()) {
        int intValue = value.GetInt();
        std::cout << "Safely retrieved int value: " << intValue << "\n";
    }

    // Type conversion behavior (returns default on mismatch)
    duin::JSONValue stringVal(std::string("not a number"));
    int defaultInt = stringVal.GetInt(); // Returns 0 with warning
    std::cout << "GetInt on string returns: " << defaultInt << "\n";

    duin::JSONValue intVal(42);
    std::string defaultStr = intVal.GetString(); // Returns empty with warning
    std::cout << "GetString on int returns empty: " << (defaultStr.empty() ? "true" : "false") << "\n";

    std::cout << "\n";
}

/**
 * @brief Example 8: Serialization
 * Demonstrates serializing JSONValue to strings
 */
void example_serialization()
{
    std::cout << "--- Example 8: Serialization ---\n";

    // Create a complex object
    duin::JSONValue data;
    data.AddMember("application", std::string("DuinEngine"));
    data.AddMember("version", std::string("1.0.0"));

    duin::JSONValue settings;
    settings.AddMember("width", 1920);
    settings.AddMember("height", 1080);
    settings.AddMember("vsync", true);
    data.AddMember("settings", settings);

    duin::JSONValue plugins;
    plugins.SetArray();
    plugins.PushBack(duin::JSONValue(std::string("Audio")));
    plugins.PushBack(duin::JSONValue(std::string("Physics")));
    plugins.PushBack(duin::JSONValue(std::string("Rendering")));
    data.AddMember("plugins", plugins);

    // Compact serialization
    std::string compact = data.Write();
    std::cout << "Compact JSON:\n" << compact << "\n\n";

    // Pretty serialization
    std::string pretty = data.Write(true);
    std::cout << "Pretty JSON:\n" << pretty << "\n";

    // Static Write method
    std::string staticWrite = duin::JSONValue::Write(data, true);
    std::cout << "Static Write (pretty):\n" << staticWrite << "\n";

    std::cout << "\n";
}

/**
 * @brief Example 9: Cloning
 * Demonstrates deep copying of JSONValue objects
 */
void example_cloning()
{
    std::cout << "--- Example 9: Cloning ---\n";

    // Create original object
    duin::JSONValue original;
    original.AddMember("name", std::string("Original"));
    original.AddMember("value", 100);

    duin::JSONValue nested;
    nested.AddMember("data", 42);
    original.AddMember("nested", nested);

    std::cout << "Original:\n" << original.Write(true) << "\n";

    // Clone creates deep copy
    duin::JSONValue cloned = original.Clone();
    std::cout << "Cloned:\n" << cloned.Write(true) << "\n";

    // Modify clone
    cloned["name"].SetString("Cloned");
    cloned["value"].SetInt(200);
    cloned["nested"]["data"].SetInt(99);

    std::cout << "After modifying clone:\n";
    std::cout << "  Original name: " << original["name"].GetString() << "\n";
    std::cout << "  Cloned name: " << cloned["name"].GetString() << "\n";
    std::cout << "  Original nested data: " << original["nested"]["data"].GetInt() << "\n";
    std::cout << "  Cloned nested data: " << cloned["nested"]["data"].GetInt() << "\n";

    // Copy assignment vs Clone
    duin::JSONValue v1(42);
    duin::JSONValue v2 = v1;        // Shares underlying pointer
    duin::JSONValue v3 = v1.Clone(); // Deep copy

    std::cout << "v1 == v2 (copy assignment): " << (v1 == v2 ? "true" : "false") << "\n";
    std::cout << "v1 == v3 (clone): " << (v1 == v3 ? "true" : "false") << "\n";

    std::cout << "\n";
}

/**
 * @brief Example 10: File Operations
 * Demonstrates reading and writing JSON files
 */
void example_file_operations()
{
    std::cout << "--- Example 10: File Operations ---\n";

    // Create configuration data
    duin::JSONValue config;
    config.AddMember("engineName", std::string("DuinEngine"));
    config.AddMember("version", std::string("1.0.0"));

    duin::JSONValue renderer;
    renderer.AddMember("api", std::string("Vulkan"));
    renderer.AddMember("width", 1920);
    renderer.AddMember("height", 1080);
    renderer.AddMember("vsync", true);
    config.AddMember("renderer", renderer);

    // Note: In a real scenario, you would write to file using std::ofstream
    // For this example, we'll demonstrate the pattern:
    std::string jsonOutput = config.Write(true);
    std::cout << "JSON to write to file:\n" << jsonOutput << "\n";

    // Simulate file writing
    std::cout << "// In real code: write jsonOutput to file using std::ofstream\n";

    // Parse from file (simulated)
    // In real code: duin::JSONValue loaded = duin::JSONValue::ParseFromFile("config.json");

    // For this example, parse from string
    duin::JSONValue loaded = duin::JSONValue::Parse(jsonOutput);

    std::cout << "Loaded from 'file':\n";
    std::cout << "  Engine: " << loaded["engineName"].GetString() << "\n";
    std::cout << "  Renderer API: " << loaded["renderer"]["api"].GetString() << "\n";
    std::cout << "  Resolution: " << loaded["renderer"]["width"].GetInt()
              << "x" << loaded["renderer"]["height"].GetInt() << "\n";

    std::cout << "\n";
}

/**
 * @brief Example 11: Real-World Config Management
 * Demonstrates practical configuration file management
 */
void example_real_world_config()
{
    std::cout << "--- Example 11: Real-World Config Management ---\n";

    // Application configuration
    duin::JSONValue appConfig;
    appConfig.AddMember("appName", std::string("MyGame"));
    appConfig.AddMember("version", std::string("0.1.0"));
    appConfig.AddMember("logLevel", std::string("info"));

    // Window settings
    duin::JSONValue window;
    window.AddMember("title", std::string("My Game - Main Window"));
    window.AddMember("width", 1280);
    window.AddMember("height", 720);
    window.AddMember("fullscreen", false);
    window.AddMember("resizable", true);
    window.AddMember("vsync", true);
    appConfig.AddMember("window", window);

    // Audio settings
    duin::JSONValue audio;
    audio.AddMember("masterVolume", 0.8);
    audio.AddMember("musicVolume", 0.6);
    audio.AddMember("sfxVolume", 1.0);
    audio.AddMember("muted", false);
    appConfig.AddMember("audio", audio);

    // Input bindings
    duin::JSONValue input;
    input.AddMember("moveForward", std::string("W"));
    input.AddMember("moveBackward", std::string("S"));
    input.AddMember("moveLeft", std::string("A"));
    input.AddMember("moveRight", std::string("D"));
    input.AddMember("jump", std::string("Space"));
    input.AddMember("mouseSensitivity", 0.5);
    appConfig.AddMember("input", input);

    // Graphics quality presets
    duin::JSONValue graphics;
    graphics.AddMember("preset", std::string("high"));
    graphics.AddMember("shadowQuality", 3);
    graphics.AddMember("textureQuality", 2);
    graphics.AddMember("antialiasing", std::string("MSAA4x"));
    graphics.AddMember("anisotropicFiltering", 16);
    appConfig.AddMember("graphics", graphics);

    std::cout << "Application Configuration:\n" << appConfig.Write(true) << "\n";

    // Accessing and modifying config
    if (appConfig.HasMember("window")) {
        duin::JSONValue& windowRef = appConfig["window"];
        int width = windowRef["width"].GetInt();
        int height = windowRef["height"].GetInt();
        std::cout << "Current resolution: " << width << "x" << height << "\n";
    }

    // Update setting
    appConfig["audio"]["masterVolume"].SetDouble(0.9);
    std::cout << "Updated master volume: " << appConfig["audio"]["masterVolume"].GetDouble() << "\n";

    std::cout << "\n";
}

/**
 * @brief Example 12: Game Entity Serialization
 * Demonstrates serializing game entities with components
 */
void example_real_world_game_entity()
{
    std::cout << "--- Example 12: Game Entity Serialization ---\n";

    // Create entity
    duin::JSONValue entity;
    entity.AddMember("name", std::string("Player"));
    entity.AddMember("tag", std::string("Player"));
    entity.AddMember("active", true);
    entity.AddMember("entityId", 1001);

    // Transform component
    duin::JSONValue transformComp;
    transformComp.AddMember("type", std::string("Transform"));

    duin::JSONValue position;
    position.SetArray();
    position.PushBack(10.5);
    position.PushBack(0.0);
    position.PushBack(5.3);
    transformComp.AddMember("position", position);

    duin::JSONValue rotation;
    rotation.SetArray();
    rotation.PushBack(0.0);
    rotation.PushBack(45.0);
    rotation.PushBack(0.0);
    transformComp.AddMember("rotation", rotation);

    duin::JSONValue scale;
    scale.SetArray();
    scale.PushBack(1.0);
    scale.PushBack(1.0);
    scale.PushBack(1.0);
    transformComp.AddMember("scale", scale);

    // Mesh component
    duin::JSONValue meshComp;
    meshComp.AddMember("type", std::string("MeshRenderer"));
    meshComp.AddMember("meshPath", std::string("models/character.obj"));
    meshComp.AddMember("materialPath", std::string("materials/player.mat"));
    meshComp.AddMember("castShadows", true);
    meshComp.AddMember("receiveShadows", true);

    // Rigidbody component
    duin::JSONValue rigidbodyComp;
    rigidbodyComp.AddMember("type", std::string("Rigidbody"));
    rigidbodyComp.AddMember("mass", 75.0);
    rigidbodyComp.AddMember("useGravity", true);
    rigidbodyComp.AddMember("isKinematic", false);
    rigidbodyComp.AddMember("drag", 0.5);
    rigidbodyComp.AddMember("angularDrag", 0.05);

    // Player controller component
    duin::JSONValue controllerComp;
    controllerComp.AddMember("type", std::string("PlayerController"));
    controllerComp.AddMember("moveSpeed", 5.0);
    controllerComp.AddMember("jumpForce", 10.0);
    controllerComp.AddMember("health", 100);
    controllerComp.AddMember("maxHealth", 100);

    // Add components array
    duin::JSONValue components;
    components.SetArray();
    components.PushBack(transformComp);
    components.PushBack(meshComp);
    components.PushBack(rigidbodyComp);
    components.PushBack(controllerComp);
    entity.AddMember("components", components);

    std::cout << "Game Entity:\n" << entity.Write(true) << "\n";

    // Access entity data
    std::cout << "Entity name: " << entity["name"].GetString() << "\n";
    std::cout << "Number of components: " << entity["components"].Capacity() << "\n";

    // Iterate over components
    std::cout << "Components:\n";
    for (auto it = entity["components"].begin(); it != entity["components"].end(); ++it) {
        duin::JSONValue comp = *it;
        std::cout << "  - " << comp["type"].GetString() << "\n";
    }

    // Access nested component data
    std::cout << "Player position: ["
              << entity["components"][0]["position"][0].GetDouble() << ", "
              << entity["components"][0]["position"][1].GetDouble() << ", "
              << entity["components"][0]["position"][2].GetDouble() << "]\n";

    std::cout << "Player health: "
              << entity["components"][3]["health"].GetInt() << "/"
              << entity["components"][3]["maxHealth"].GetInt() << "\n";

    std::cout << "\n";
}

/**
 * @brief Example 13: Error Handling
 * Demonstrates proper error handling and edge cases
 */
void example_error_handling()
{
    std::cout << "--- Example 13: Error Handling ---\n";

    // Accessing non-existent member
    duin::JSONValue obj;
    obj.AddMember("exists", 1);

    duin::JSONValue missing = obj["doesNotExist"];
    std::cout << "Non-existent member returns default object: "
              << (missing.IsObject() ? "true" : "false") << "\n";

    // Type mismatch
    duin::JSONValue strValue(std::string("not a number"));
    int defaultInt = strValue.GetInt(); // Returns 0 with warning
    std::cout << "GetInt on string returns default: " << defaultInt << "\n";

    // Invalid operations
    duin::JSONValue arrayVal;
    arrayVal.SetArray();

    std::cout << "Adding member to array (invalid operation):\n";
    arrayVal.AddMember("key", 42); // Logs warning, no effect
    std::cout << "Array is still array: " << (arrayVal.IsArray() ? "true" : "false") << "\n";

    // PushBack on non-array
    duin::JSONValue objVal;
    std::cout << "PushBack on object (invalid operation):\n";
    objVal.PushBack(42); // Logs warning
    std::cout << "Object is still object: " << (objVal.IsObject() ? "true" : "false") << "\n";

    // Validity checking
    std::cout << "IsValid() check:\n";
    duin::JSONValue valid;
    auto invalid = duin::JSONValue::Invalid();

    std::cout << "  Valid value: " << (valid.IsValid() ? "true" : "false") << "\n";
    std::cout << "  Invalid value: " << (invalid.IsValid() ? "true" : "false") << "\n";

    // IsReadValid check
    std::cout << "IsReadValid() check:\n";
    std::cout << "  Normal value: " << (valid.IsReadValid() ? "true" : "false") << "\n";

    // Removing non-existent member (safe operation)
    obj.RemoveMember("doesNotExist"); // No crash, no effect
    std::cout << "Removing non-existent member: OK (no crash)\n";

    // Empty array operations
    duin::JSONValue emptyArray;
    emptyArray.SetArray();
    std::cout << "Empty array:\n";
    std::cout << "  IsEmpty: " << (emptyArray.Empty() ? "true" : "false") << "\n";
    std::cout << "  Capacity: " << emptyArray.Capacity() << "\n";

    // Special values
    duin::JSONValue emptyString(std::string(""));
    std::cout << "Empty string is valid string: " << (emptyString.IsString() ? "true" : "false") << "\n";
    std::cout << "Empty string value is empty: " << (emptyString.GetString().empty() ? "true" : "false") << "\n";

    duin::JSONValue zero(0);
    std::cout << "Zero value: " << zero.GetInt() << "\n";

    duin::JSONValue negative(-42);
    std::cout << "Negative value: " << negative.GetInt() << "\n";

    std::cout << "\n";
}
