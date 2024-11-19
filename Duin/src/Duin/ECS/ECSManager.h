#pragma once

#include <memory.h>

#include <flecs.h>
#include "Duin/Core/Maths/DuinMaths.h"
#include "Duin/Core/Utils/UUID.h"
#include "Duin/Physics/PhysXManager.h"

namespace duin {




    /*----------------------------------------------------------------------------*/
    //  ECS Structure Namespaces
    /*----------------------------------------------------------------------------*/
    namespace ECSTag {
        /*------------------------------------------------------------------------*/
        //  Tags
        /*------------------------------------------------------------------------*/
        struct Local {};
        struct Global {};

        /* Tag used to queue a component for a query that will create a component with external references, and/or its dependencies */
        struct CreateExternalRef {};
        /* Tag used to indicate when a component references external memory */
        struct ActiveExternalRef {};
        /* Tag used to queue a component for a query that will delete a component with external references, and/or its dependencies */
        struct DeleteExternalRef {};

        /* Tags used to describe PhysX bodies */
        struct PxKinematic {};
        struct PxDynamic {};
        struct PxStatic {};

        /* Tags used to set and track camera as active */
        struct SetCameraAsActive {};
        struct CameraIsActive {};
        struct ActiveCamera {};
    };



    namespace ECSComponent {
        /*------------------------------------------------------------------------*/
        //  2D Components
        /*------------------------------------------------------------------------*/
        struct Position2D {
            Vector2 value;

            Position2D()
                : value(Vector2Zero())
            {}

            Position2D(Vector2 value)
                : value(value)
            {}

            Position2D(float x, float y)
                : value(x, y)
            {}
        };

        struct Rotation2D {
            float value;

            Rotation2D()
                : value(0.0f)
            {}

            Rotation2D(float r)
                : value(r)
            {}
        };

        struct Scale2D {
            Vector2 value;

            Scale2D()
                : value(Vector2Zero())
            {}

            Scale2D(Vector2 value)
                : value(value)
            {}

            Scale2D(float x, float y)
                : value(x, y)
            {}
        };

        struct Velocity2D {
            Vector2 value;

            Velocity2D()
                : value(Vector2Zero())
            {}

            Velocity2D(Vector2 value)
                : value(value)
            {}

            Velocity2D(float x, float y)
                : value(x, y)
            {}
        };

        /*------------------------------------------------------------------------*/
        //  3D Components
        /*------------------------------------------------------------------------*/
        struct Position3D {
            Vector3 value;

            Position3D()
                : value(Vector3Zero())
            {}

            Position3D(Vector3 value)
                : value(value)
            {}

            Position3D(float x, float y, float z)
                : value(x, y, z)
            {}
        };

        struct Rotation3D {
            Quaternion value;

            Rotation3D()
                : value(Vector4Zero())
            {}

            Rotation3D(Quaternion value)
                : value(value)
            {}

            Rotation3D(float x, float y, float z, float w)
                : value(x, y, z, w)
            {}
        };

        struct Scale3D {
            Vector3 value;

            Scale3D()
                : value(Vector3Zero())
            {}

            Scale3D(Vector3 value)
                : value(value)
            {}

            Scale3D(float x, float y, float z)
                : value(x, y, z)
            {}
        };

        struct Velocity3D {
            Vector3 value;

            Velocity3D()
                : value(Vector3Zero())
            {}

            Velocity3D(Vector3 value)
                : value(value)
            {}

            Velocity3D(float x, float y, float z)
                : value(x, y, z)
            {}
        };

        /*------------------------------------------------------------------------*/
        //  Physics Components
        /*------------------------------------------------------------------------*/
        struct CharacterBody3DComponent {
            CharacterBody3D *characterBody;
        };

        /*------------------------------------------------------------------------*/
        //  Physics Components
        /*------------------------------------------------------------------------*/
        struct DebugCapsuleDesc {
            float height, radius;
            int slices, rings;
            ::Color color;
        };
    };



    namespace ECSPrefab {
        extern flecs::entity Node;
        extern flecs::entity Node2D;
        extern flecs::entity Node3D;
        extern flecs::entity CharacterBody3D;
        extern flecs::entity Camera3D;

        extern flecs::entity DebugCapsule;
    };



    /*----------------------------------------------------------------------------*/
    //  ECS Manager Class
    /*----------------------------------------------------------------------------*/
    class ECSManager
    {
    public:
        flecs::world world;

        ECSManager() = default;
        ~ECSManager() = default;

        void Initialize();

        void ActivateCameraEntity(flecs::entity entity);
        void SetGlobalPosition3D(flecs::entity entity, Vector3 newPos);

        void ExecuteQuerySetCameraAsActive(flecs::world& world);
        void ExecuteCharacterBody3DCreation(PhysXManager& manager);
        void ExecuteQueryUpdatePosition3D(flecs::world& world);        
        void ExecuteQueryHierarchicalUpdatePosition3D(flecs::world& world);
        void ExecuteQueryUpdateRotation3D(flecs::world& world);
        void ExecuteQueryHierarchicalUpdateRotation3D(flecs::world& world);
        void ExecuteQueryUpdateCharacterBodyPosition(flecs::world& world);
        void ExecuteQueryUpdateCameraPosition(flecs::world& world);
        void ExecuteQueryDrawDebugCapsule(flecs::world& world);

    private:
        // WIP: Not ready to be used
        void InitializeRemoteExplorer();
    };

}

