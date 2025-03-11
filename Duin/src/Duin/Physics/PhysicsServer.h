#pragma once

#include <unordered_map>
#include <memory>

// Use PhysX by default
#ifndef JOLT
#define PHYSICS_SERVER PhysXPhysicsServer
#endif

#ifdef JOLT
#ifndef PHYSX
#define PHYSICS_SERVER JoltServer
#endif
#endif

namespace duin {
    enum PhysicsServerError {
        SUCCESS = 0,
        INIT_FAILED,
        MATERIAL_CREATION_FAILED
    };

    class UUID;
    class CollisionObject;
    class CollisionShape;
    class CharacterBody;
    class PhysicsServer
    {
        public:
            static PhysicsServer& Get();
            static int CatchError(void *ptr, int errorCode);

            PhysicsServer() = default;
            virtual ~PhysicsServer() = default;

            virtual void Initialize() = 0;
            virtual void Clean() = 0;

            virtual void StepPhysics(double delta) = 0;
            virtual std::shared_ptr<CollisionShape> CreateCollisionShape() = 0;

       protected:
            class PHYSICS_SERVER;
            static std::unique_ptr<PHYSICS_SERVER> implServer;

            static void PrintErrorCode(int errorCode);
    };
}

#undef PHYSICS_SERVER
