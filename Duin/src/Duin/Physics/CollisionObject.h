#pragma once

#include "Duin/Core/Utils/UUID.h"

namespace duin {
    class CollisionObject
    {
        public:
            UUID GetUUID();

        private:
            UUID uuid;
    };
}
