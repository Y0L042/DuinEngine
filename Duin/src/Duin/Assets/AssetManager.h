#pragma once
#include "Duin/Core/Utils/UUID.h"

namespace duin {

    class UUID;
    class AssetManager
    {
        public:
            AssetManager();
            ~AssetManager();

            UUID GetUUID();

            // Generate and share meshes

        private:
            UUID uuid;

    };

}
