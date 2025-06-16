#pragma once

#include <string>
#include "ECSManager.h"

namespace duin {
    class SceneBuilder
    {
        public:
            // input string must have root entity as json root
            static void ReadScene(const std::string& file, ECSManager& ecs);
            static void WriteScene(const std::string& file, const std::string& scene);

        private:
    };
}
