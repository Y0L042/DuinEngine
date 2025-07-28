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
            static void ReadScene(const std::string& string, ECSManager& ecs);          // Read world from json string
            static void WriteScene(std::string& file, ECSManager& ecs);                 // Write world to json string
            static flecs::entity ReadEntity(const std::string& file, ECSManager& ecs);  // Read entity from json string
            static void WriteEntity(std::string& file, flecs::entity entity);           // Write entity to json string

        private:
            static void RecursiveWriter(flecs::entity e, DataValue& entityData);
    };
}
