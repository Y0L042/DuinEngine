#pragma once

#include "Duin/IO/JSONValue.h"
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

        UUID uuid = UUID::INVALID;
        std::string name = "";
        std::vector<ExternalSceneDependency> extScnDeps;
        std::string flecsJSON = "";
        
    private:
        friend class ExternalSceneDependency;
    };



    class ExternalSceneDependency
    {
    public:
        ExternalSceneDependency() = default;
        ExternalSceneDependency(const JSONValue& json);

        UUID exDepUUID = UUID::INVALID;
        uint64_t localParent = 0;
        std::string type = "";

    private:
        friend class SceneDefinition;
    };
}
