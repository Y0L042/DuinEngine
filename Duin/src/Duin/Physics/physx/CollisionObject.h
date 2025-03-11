#pragma once

#include "Duin/Core/Utils/UUID.h"
#include "CollisionMeta.h"

#include <memory>

namespace duin {
    class PhysicsServer;
    class CollisionObject
    {
        public:
            void SetCollisionShape();

            const UUID GetUUID();

            CollisionLayer GetCollisionLayer();
            CollisionMask GetCollisionMask();
            void SetCollisionLayer(CollisionLayer bits);
            void SetCollisionMask(CollisionMask bits);

            PhysicsServer& GetPhysicsServer();

        private:
            UUID uuid;
            CollisionLayer collisionLayer;
            CollisionMask collisionMask;

    };
}
