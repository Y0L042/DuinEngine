#pragma once

#include "Duin/Core/Utils/UUID.h"
#include "./CollisionMeta.h"

namespace duin {
    class CollisionObject
    {
        public:
            UUID GetUUID();

            CollisionLayer GetCollisionLayer();
            CollisionMask GetCollisionMask();
            void SetCollisionLayer(CollisionLayer bits);
            void SetCollisionMask(CollisionMask bits);

        private:
            UUID uuid;
            CollisionLayer collisionLayer;
            CollisionMask collisionMask;
    };
}
