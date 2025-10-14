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
    //: 
    //    uuid(json[tag_SD_UUID]), 
    //    name(json[tag_SD_NAME]), 
    //    extScnDeps(json[tag_SD_EXT_SCN_DEPS]),
    //    flecsJSON(json[tag_SD_FLECSJSON])
{

}

duin::ExternalSceneDependency::ExternalSceneDependency(const JSONValue& json)
{

}
