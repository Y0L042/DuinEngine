#pragma once

#include "Duin/IO/JSONValue.h"
#include "ECSManager.h"
#include <flecs.h>
#include <string>
#include <vector>

namespace duin
{
enum ResourceType
{
    RES_UNKNOWN = 0,
    RES_SCENE,
    RES_MODEL,
    RES_TEXTURE,
    RES_SHADER,
    RES_AUDIO
};

std::string ResourceTypeToString(ResourceType type);
ResourceType StringToResourceType(const std::string &string);

struct Scene
{
  public:
    struct ExDep
    {
        UUID uuid;         // UUID of ExDep entry (not of the linked dependency)
        UUID exdepUUID;    // UUID of the linked external dependency
        uint64_t parent;   // Parent component of external dependency
        ResourceType type; // Type of external dependency
    };

    static const char TAG_EXDEP_UUID[];
    static const char TAG_EXDEP_EXDEPUUID[];
    static const char TAG_EXDEP_PARENT[];
    static const char TAG_EXDEP_TYPE[];

    static const char TAG_UUID[];
    static const char TAG_NAME[];
    static const char TAG_FLECSJSON[];
    static const char TAG_EXT_SCN_DEPS[];

    UUID uuid = UUID::INVALID;             // Contains the UUID of the scene; the file also uses this UUID as identifier
    std::string name = "";     // Name of scene given by user
    std::string flecsJSON = ""; // This is the flecs JSON of the entities and components of the scene (eg. player and
                                // player
                           // components)
    std::vector<ExDep> externalSceneDependencies; // List of external scene dependencies used by this scene.

    static Scene ReadFromFile(const std::string &filePath);
    static Scene ReadFromString(const std::string &string);
    void WriteToFile(const std::string &filePath);
    std::string WriteToString();

    static Scene ReadJSON(JSONValue sceneJSON);
    JSONValue WriteJSON();

  private:
};

} // namespace duin
