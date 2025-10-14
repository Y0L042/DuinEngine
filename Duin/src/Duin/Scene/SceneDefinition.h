#pragma once

#include "Duin/IO/DataValue.h"
#include "Duin/Core/Utils/UUID.h"

#include <string>
#include <vector>

namespace duin
{
    class ExternalSceneDependency;
    class SceneDefinition
    {
    public:
        SceneDefinition() = default;
        SceneDefinition(const JSONValue& json);

        UUID uuid;
        std::string name;
        std::vector<ExternalSceneDependency> extScnDeps;
        std::string flecsJSON;
        
    private:
        friend class ExternalSceneDependency;
    };



    class ExternalSceneDependency
    {
    public:
        ExternalSceneDependency() = default;
        ExternalSceneDependency(const JSONValue& json);

        UUID exDepUUID;
        uint64_t localParent;
        std::string type;

    private:
        friend class SceneDefinition;
    };
}
