#pragma once

#include <string>
#include "Duin/IO/DataValue.h"
#include "ECSManager.h"
#include <flecs.h>
#include <vector>

namespace duin {
    enum ResourceType {
        RES_UNKNOWN = 0,
        RES_SCENE,
        RES_MODEL,
        RES_TEXTURE,
        RES_SHADER,
        RES_AUDIO
    };

    std::string ResourceTypeToString(ResourceType type);
    ResourceType StringToResourceType(const std::string& string);

    struct Scene
    {
    public:
        struct ExDep
        {
            UUID uuid;
            UUID exdepUUID;          // UUID of external dependency
            uint64_t parent;    // Parent component of external dependency
            ResourceType type;  // Type of external dependency
        };
        
        static const char TAG_EXDEP_EXDEPUUID[];
        static const char TAG_EXDEP_PARENT[];
        static const char TAG_EXDEP_TYPE[];

        static const char TAG_UUID[];
        static const char TAG_NAME[];
        static const char TAG_FLECSJSON[];
        static const char TAG_EXT_SCN_DEPS[];

        UUID uuid;              // Contains the UUID of the scene; the file also uses this UUID as identifier
        std::string name;       // Name of scene given by user
        std::string flecsJSON;  // This is the flecs JSON of the entities and components of the scene (eg. player and player components)
        std::vector<ExDep> externalSceneDependencies;   // List of external scene dependencies used by this scene.

		static Scene ReadFromFile(const std::string& filePath);
		void WriteToFile(const std::string& filePath);

        static Scene ReadJSON(JSONValue sceneJSON);
        JSONValue WriteJSON();

    private:

    };

    class SceneBuilder
    {
        public:


            // input string must have root entity as json root
            static void ReadScene(const std::string& string, ECSManager& ecs);          // Read world from json string
            static void WriteScene(std::string& file, ECSManager& ecs);                 // Write world to json string
            static flecs::entity ReadEntity(const std::string& file, ECSManager& ecs);  // Read entity from json string
            static void WriteEntity(std::string& file, flecs::entity entity, bool recursive = false);           // Write entity to json string

        private:
            static void RecursiveWriter(flecs::entity e, std::string& file);
    };
}
