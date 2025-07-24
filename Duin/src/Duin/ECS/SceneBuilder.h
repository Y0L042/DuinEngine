#pragma once

#include <string>
#include "Duin/IO/DataValue.h"
#include "ECSManager.h"
#include <flecs.h>

namespace duin {
    class SceneBuilder
    {
        public:
            // input string must have root entity as json root
            static void ReadScene(const std::string& file, ECSManager& ecs);
            static void WriteScene(std::string& file, ECSManager& ecs);
            static void WriteEntity(std::string& file, flecs::entity entity);

        private:
            static void RecursiveWriter(flecs::entity e, DataValue& entityData);
    };
}
