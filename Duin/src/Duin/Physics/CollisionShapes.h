#pragma once

#include <memory>

#include "Duin/Core/Utils/UUID.h"

namespace duin {
    class CollisionShape
    {
        public:
            static std::shared_ptr<CollisionShape> Create();

            UUID GetUUID();

        protected:
            UUID uuid;
    };
}
