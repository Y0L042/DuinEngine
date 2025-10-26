#include "dnpch.h"
#include "SceneDefinition.h"

#include <string>

/* Scene Definition */
const std::string tag_SD_UUID = "UUID";                // File and Scene UUID
const std::string tag_SD_NAME = "NAME";                // Scene Name
const std::string tag_SD_EXT_SCN_DEPS = "EXT_SCN_DEPS";// Links to external Scenes and Deps
const std::string tag_SD_FLECSJSON = "FLECSJSON";      // FLECS entities

/* External Dependancy Definition */
const std::string tag_EDD_EXDEPUUID = "EXDEPUUID";      // UUID of external Dependancy
const std::string tag_EDD_LOCALPARENT = "LOCALPARENT";  // Local parent Entity of ExDep
const std::string tag_EDD_TYPE = "TYPE";                // External Dependancy type

/* Flecs Entity Definition */
const std::string tag_FED_NAME = "name";
const std::string tag_FED_TAGS = "tags";
const std::string tag_FED_PAIRS = "pairs";
const std::string tag_FED_COMPONENTS = "components";
const std::string tag_FED_PARENT = "parent";

duin::SceneDefinition::SceneDefinition(const JSONValue& json)
    : 
       uuid(UUID::FromStringHex(json.GetMember(tag_SD_UUID).GetString())), 
       name(json.GetMember(tag_SD_NAME).GetString()), 
       flecsJSON(json.GetMember(tag_SD_FLECSJSON).GetString())
{
    if (json.GetMember(tag_SD_EXT_SCN_DEPS).IsArray()) 
    {
        auto arr = json.GetMember(tag_SD_EXT_SCN_DEPS);
        for (auto it = arr.Begin(); it != arr.End(); ++it) 
        {
            const duin::JSONValue value = *it;
            extScnDeps.emplace_back(ExternalSceneDependency(value));
        }
    }
}

duin::ExternalSceneDependency::ExternalSceneDependency(const JSONValue& json)
    :
       exDepUUID(UUID::FromStringHex(json.GetMember(tag_EDD_EXDEPUUID).GetString())), 
       localParent(static_cast<uint64_t>(json.GetMember(tag_EDD_LOCALPARENT).GetInt())),
       type(json.GetMember(tag_EDD_TYPE).GetString())
{

}
