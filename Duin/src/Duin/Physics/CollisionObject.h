#pragma once

#include "Duin/Core/Utils/UUID.h"
#include "CollisionMeta.h"
#include "CollisionShapes.h"

namespace duin {
    class PhysicsServer;
    class CollisionObject
    {
        public:
            virtual void SetCollisionShape() = 0;

            const UUID GetUUID();

            CollisionLayer GetCollisionLayer();
            CollisionMask GetCollisionMask();
            void SetCollisionLayer(CollisionLayer bits);
            void SetCollisionMask(CollisionMask bits);

            PhysicsServer& GetServer();

        private:
            UUID uuid;
            CollisionLayer collisionLayer;
            CollisionMask collisionMask;

    };
}
