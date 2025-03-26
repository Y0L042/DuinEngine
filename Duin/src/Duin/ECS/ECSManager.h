#pragma once

#include <memory.h>

#include <flecs.h>
#include "Duin/Core/Maths/DuinMaths.h"
#include "Duin/Core/Debug/DNLog.h"
#include "Duin/Core/Utils/UUID.h"
#include "Duin/Physics/PhysicsIncludes.h"

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
        struct NonPx {};

        /* Tags used to set and track camera as active */
        struct SetCameraAsActive {};
        struct CameraIsActive {};
        struct ActiveCamera {};
    };


    /*------------------------------------------------------------------------*/
    //  ECS Components
    /*------------------------------------------------------------------------*/
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

        struct Transform2D {

        };

        /**
         * @class Velocity2D
         * @brief Always relative to global Coords
         *
         */
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

        struct Transform3D {
            public:
                Transform3D(Vector3 position = Vector3Zero(), Vector3 scale = Vector3One(), Quaternion rotation = QuaternionIdentity()) 
                    : position_(position), scale_(scale), rotation_(rotation)
                {};

                Transform3D(Vector3 position, Quaternion rotation)
                    : position_(position), scale_(Vector3One()), rotation_(rotation)
                {};

                Transform3D(const Transform3D& tx)
                    : position_(tx.GetPosition()), scale_(tx.GetScale()), rotation_(tx.GetRotation())
                {};

                void SetPosition(Vector3 position)
                {
                    position_ = position;
                    InvalidateCacheFlags();
                }
                
                Vector3 GetPosition() const
                {
                    return position_;
                }

                void SetScale(Vector3 scale)
                {
                    scale_ = scale;
                    InvalidateCacheFlags();
                }
                
                Vector3 GetScale() const
                {
                    return scale_;
                }

                void SetRotation(Quaternion rotation)
                {
                    rotation_ = rotation;
                }
                
                Quaternion GetRotation() const
                {
                    return rotation_;
                }

                static void SetGlobalTransform(flecs::entity e, Transform3D tx)
                {
                    // TODO
                }

                static Transform3D GetGlobalTransform(flecs::entity e)
                {
                    // TODO
                    return Transform3D();
                }


                static void SetGlobalPosition(flecs::entity e, Vector3 position)
                {
                    if (!e.is_valid() || !e.has<Transform3D>()) { return; }
                    flecs::entity parent = e.parent();
                    if (parent.is_valid() && parent.has<Transform3D>()) {
                        Vector3 parentGlobalPos   = GetGlobalPosition(parent);
                        Quaternion parentGlobalRot = GetGlobalRotation(parent);
                        Vector3 parentGlobalScale = GetGlobalScale(parent);
                        Vector3 offset = Vector3Subtract(position, parentGlobalPos);
                        Quaternion invParentRot = QuaternionInvert(parentGlobalRot);
                        Vector3 localPosUnscaled = Vector3RotateByQuaternion(offset, invParentRot);
                        Vector3 localPos = Vector3Divide(localPosUnscaled, parentGlobalScale);
                        e.get_mut<Transform3D>()->SetPosition(localPos);
                    } else {
                        e.get_mut<Transform3D>()->SetPosition(position);
                    }
                }

                static Vector3 GetGlobalPosition(flecs::entity e)
                {
                    if (!e.is_valid() || !e.has<Transform3D>()) { return Vector3Zero(); }
                    Transform3D *tx = e.get_mut<Transform3D>();
                    if (!tx) { return Vector3Zero(); }

                    if (tx->globalPositionCacheDirtyFlag) {
                        if (!e.parent().is_valid() || !e.parent().has<Transform3D>()) {
                            tx->UpdateGlobalPositionCache(tx->GetPosition());
                        } else {
                            Transform3D parentGlobal = GetGlobalTransform(e.parent());
                            Vector3 scaledLocalPos = Vector3Multiply(tx->GetPosition(), parentGlobal.GetScale());
                            Vector3 rotatedPos = Vector3RotateByQuaternion(scaledLocalPos, parentGlobal.GetRotation());
                            Vector3 globalPos = Vector3Add(parentGlobal.GetPosition(), rotatedPos);
                            tx->UpdateGlobalPositionCache(globalPos);
                        }
                    }

                    return tx->globalPositionCache;
                }

                static void SetGlobalScale(flecs::entity e, Vector3 scale)
                {
                    if (!e.is_valid() || !e.has<Transform3D>()) { return; }
                    flecs::entity parent = e.parent();
                    if (parent.is_valid() && parent.has<Transform3D>()) {
                        Vector3 parentGlobalScale = GetGlobalScale(parent);
                        e.get_mut<Transform3D>()->SetScale(Vector3Divide(scale, parentGlobalScale));
                    } else {
                        e.get_mut<Transform3D>()->SetScale(scale);
                    }
                }

                static Vector3 GetGlobalScale(flecs::entity e)
                {
                    if (!e.is_valid() || !e.has<Transform3D>()) { return Vector3One(); }
                    Transform3D* tx = e.get_mut<Transform3D>();
                    if (!tx) { return Vector3One(); }
                    if (tx->globalScaleCacheDirtyFlag) {
                        if (!e.parent().is_valid() || !e.parent().has<Transform3D>()) {
                            tx->UpdateGlobalScaleCache(tx->GetScale());
                        } else {
                            Vector3 parentGlobalScale = GetGlobalScale(e.parent());
                            Vector3 globalScale = Vector3Multiply(parentGlobalScale, tx->GetScale());
                            tx->UpdateGlobalScaleCache(globalScale);
                        }
                    }

                    return tx->globalScaleCache;
                }

                static void SetGlobalRotation(flecs::entity e, Quaternion rotation)
                {
                    if (!e.is_valid() || !e.has<Transform3D>()) { return; }
                }

                static Quaternion GetGlobalRotation(flecs::entity e)
                {
                    if (!e.is_valid() || !e.has<Transform3D>()) { return QuaternionIdentity(); }

                    return Quaternion();
                }

            private:
                Vector3 position_;
                Vector3 scale_;
                Quaternion rotation_;
                Vector3 globalPositionCache;
                Vector3 globalScaleCache;
                Quaternion globalRotationCache;
                bool globalPositionCacheDirtyFlag = true;
                bool globalScaleCacheDirtyFlag = true;
                bool globalRotationCacheDirtyFlag = true;

                void InvalidateCacheFlags()
                {
                    globalPositionCacheDirtyFlag = true;
                    globalScaleCacheDirtyFlag = true;
                    globalRotationCacheDirtyFlag = true;
                }

                static void InvalidateCacheFlags(flecs::entity e)
                {
                    if (!e.is_valid() || !e.has<Transform3D>()) { return; }
                    Transform3D *tx = e.get_mut<Transform3D>();
                    if (!tx) { return; }
                    tx->InvalidateCacheFlags();
                    e.children([](flecs::entity child) {
                        if (!child.is_valid() || !child.has<Transform3D>()) { return; }
                        InvalidateCacheFlags(child);
                    });
                }

                void UpdateGlobalPositionCache(Vector3 newGlobalPosition)
                {
                    globalPositionCache = newGlobalPosition;
                    globalPositionCacheDirtyFlag = false;
                }

                void UpdateGlobalScaleCache(Vector3 newGlobalScale)
                {
                    globalScaleCache = newGlobalScale;
                    globalScaleCacheDirtyFlag = false;
                }

                void UpdateGlobalRotationCache(Quaternion newGlobalRotation)
                {
                    globalRotationCache = newGlobalRotation;
                    globalRotationCacheDirtyFlag = false;
                }

                static Vector3 GetGlobalPositionRecursively(flecs::world& world, flecs::entity e)
                {
                    if (!e.is_valid()) { return Vector3Zero(); }
                    const Transform3D *tx = e.get<Transform3D>();
                    if (!tx) { return Vector3Zero(); }
                    if (!e.parent().is_valid()) { return tx->GetPosition(); }

                    return Vector3Add(tx->GetPosition(), GetGlobalPositionRecursively(world, e.parent()));
                }

                static Vector3 GetGlobalScaleRecursively(flecs::world& world, flecs::entity e)
                {
                    if (!e.is_valid()) { return Vector3One(); }
                    const Transform3D *tx = e.get<Transform3D>();
                    if (!tx) { return Vector3One(); }
                    if (!e.parent().is_valid()) { return tx->GetScale(); }

                    return Vector3Multiply(tx->GetScale(), GetGlobalScaleRecursively(world, e.parent()));
                }

                static Quaternion GetGlobalRotationRecursively(flecs::world& world, flecs::entity e)
                {
                    if (!e.is_valid()) { return QuaternionIdentity(); }
                    const Transform3D *tx = e.get<Transform3D>();
                    if (!tx) { return QuaternionIdentity(); }
                    if (!e.parent().is_valid()) { return tx->GetRotation(); }

                    return QuaternionAdd(tx->GetRotation(), GetGlobalRotationRecursively(world, e.parent()));
                }

                static Transform3D GetGlobalTransformRecursively(flecs::world& world, flecs::entity e)
                {
                    if (!e.is_valid()) { return Transform3D(); }
                    const Transform3D *tx = e.get<Transform3D>();
                    if (!tx) { return Transform3D(); }
                    if (!e.parent().is_valid()) { return *tx; }

                    Transform3D parentTx = GetGlobalTransformRecursively(world, e.parent());
                    Transform3D resultTx(Vector3Add(tx->GetPosition(), parentTx.GetPosition()),
                                         Vector3Multiply(tx->GetScale(), parentTx.GetScale()),
                                         QuaternionAdd(tx->GetRotation(), parentTx.GetRotation()));

                    return resultTx;
                }

        };

        /**
         * @class Velocity3D
         * @brief Always relative to global Coords
         *
         */
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
        //  3D Visual Components
        /*------------------------------------------------------------------------*/
        struct CubeComponent {
            float width, height, length;
            ::Color color;
        };

        /*------------------------------------------------------------------------*/
        //  3D Physics Components
        /*------------------------------------------------------------------------*/
        struct CharacterBodyComponent {
            std::shared_ptr<CharacterBody> characterBody;
        };

        struct StaticBodyComponent {
            std::shared_ptr<StaticBody> staticBody;
        };

        struct KinematicBodyComponent {
            std::shared_ptr<KinematicBody> kinematicBody;
        };

        struct DynamicBodyComponent {
            std::shared_ptr<DynamicBody> dynamicBody;
        };

        struct PhysicsStaticCubeComponent {
            PhysicsStaticCubeComponent *cube;
        };
        /*------------------------------------------------------------------------*/
        //  3D Debug Components
        /*------------------------------------------------------------------------*/
        struct DebugCapsuleComponent {
            float height, radius;
            int slices, rings;
            ::Color color;
        };

        struct DebugCubeComponent {
            float width, height, length;
            ::Color color;
        };
    };



    /*------------------------------------------------------------------------*/
    //  ECS Prefabs
    /*------------------------------------------------------------------------*/
    namespace ECSPrefab {
        extern flecs::entity Node;
        extern flecs::entity Node2D;
        extern flecs::entity Node3D;

        extern flecs::entity PhysicsStaticBody;
        extern flecs::entity PhysicsKinematicBody;
        extern flecs::entity PhysicsDynamicBody;
        extern flecs::entity PhysicsCharacterBody;

        extern flecs::entity Camera3D;
        extern flecs::entity Cube;

        extern flecs::entity DebugCapsule;
    };



    /*----------------------------------------------------------------------------*/
    //  ECS Manager Class
    /*----------------------------------------------------------------------------*/
    class JSONMember;
    class ECSManager
    {
    public:
        flecs::world world;

        ECSManager() = default;
        ~ECSManager() = default;

        void Initialize();

        flecs::entity CreateEntityFromJSON(JSONMember& member);

        void ActivateCameraEntity(flecs::entity entity);
        void SetGlobalPosition3D(flecs::entity entity, Vector3 newPos);

        void PostUpdateQueryExecution(double delta);
        void PostPhysicsUpdateQueryExecution(double delta);
        void PostDrawQueryExecution();
        void PostDrawUIQueryExecution();


        void ExecuteCharacterBody3DCreation(PhysicsServer& server);

        void ExecuteQuerySetCameraAsActive();
        void ExecuteQueryUpdatePosition3D();        
        void ExecuteQueryHierarchicalUpdatePosition3D();
        void ExecuteQueryUpdateRotation3D();
        void ExecuteQueryHierarchicalUpdateRotation3D();
        void ExecuteQueryUpdateCharacterBody3DPosition();
        void ExecuteQuerySyncDynamicBody3DPosition();
        void ExecuteQueryUpdateCameraPosition();
        void ExecuteQueryControlCamera();
        void ExecuteQueryDrawCube();
        void ExecuteQueryDrawDebugCapsule();
        void ExecuteQueryDrawDebugCube();

    private:
        // WIP: Not ready to be used
        void InitializeRemoteExplorer();
    };

}

