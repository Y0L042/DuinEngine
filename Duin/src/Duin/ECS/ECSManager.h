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
                static void RegisterOnSetObserver(flecs::world& world)
                {
                    DN_CORE_INFO("Register Transform3D OnSet Observer.");
                    static flecs::observer o = world.observer<Transform3D>()
                        .event(flecs::OnSet)
                        .observer_flags(EcsObserverYieldOnCreate)
                        .each([](flecs::entity e, Transform3D& tx) {
                              tx.SetEntity(e);
                        });
                }

                static void RegisterOnAddObserver(flecs::world& world)
                {
                    DN_CORE_INFO("Register Transform3D OnAdd Observer.");
                    static flecs::observer o = world.observer<Transform3D>()
                        .event(flecs::OnAdd)
                        .observer_flags(EcsObserverYieldOnCreate)
                        .each([](flecs::entity e, Transform3D& tx) {
                              tx.SetEntity(e);
                        });
                }

                static void SetGlobalTransform(flecs::entity e, Transform3D tx)
                {
                    // TODO
                    // if (!e.is_valid() || !e.has<Transform3D>()) { 
                    //     return; 
                    // }
                    // Transform3D *eTx = e.get_mut<Transform3D>();
                    // if (eTx) {
                    //     SetGlobalPosition(e, tx.GetPosition());
                    //     SetGlobalScale(e, tx.GetScale());
                    //     SetGlobalRotation(e, tx.GetRotation());
                    // }
                }

                static Transform3D GetGlobalTransform(flecs::entity e)
                {
                    // TODO
                    // if (!e.is_valid() || !e.has<Transform3D>()) { return Transform3D(); }
                    // const Transform3D *tx = e.get<Transform3D>();
                    // return (tx ? *tx : Transform3D());
                }


                static void SetGlobalPosition(flecs::entity e, Vector3 position)
                {
                    if (!e.is_valid() || !e.has<Transform3D>()) { 
                        DN_CORE_WARN("STATIC Entity not valid, or does not have Transform3D!");
                        return; 
                    }
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
                    if (!e.is_valid() || !e.has<Transform3D>()) { 
                        DN_CORE_WARN("STATIC Entity not valid, or does not have Transform3D!");
                        return Vector3Zero(); 
                    }
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
                    if (!e.is_valid() || !e.has<Transform3D>()) { 
                        DN_CORE_WARN("STATIC Entity not valid, or does not have Transform3D!");
                        return; 
                    }
                    flecs::entity parent = e.parent();
                    if (parent.is_valid() && parent.has<Transform3D>()) {
                        // Global = ParentGlobal * Local  =>  Local = inverse(ParentGlobal) * Global
                        Quaternion parentGlobalRot = GetGlobalRotation(parent);
                        Quaternion invParentRot = QuaternionInvert(parentGlobalRot);
                        Quaternion localRotation = QuaternionMultiply(invParentRot, rotation);
                        e.get_mut<Transform3D>()->SetRotation(localRotation);
                    } else {
                        e.get_mut<Transform3D>()->SetRotation(rotation);
                    }
                }

                static Quaternion GetGlobalRotation(flecs::entity e)
                {
                    if (!e.is_valid() || !e.has<Transform3D>()) { 
                        DN_CORE_WARN("STATIC Entity not valid, or does not have Transform3D!");
                        return QuaternionIdentity();
                    }
                    Transform3D* tx = e.get_mut<Transform3D>();
                    if (!tx) { return QuaternionIdentity(); }
                    if (tx->globalRotationCacheDirtyFlag) {
                        flecs::entity parent = e.parent();
                        if (!parent.is_valid() || !parent.has<Transform3D>()) {
                            // No parent: global rotation is the local rotation.
                            tx->UpdateGlobalRotationCache(tx->GetRotation());
                        } else {
                            // Global = ParentGlobal * LocalRotation
                            Quaternion parentGlobalRot = GetGlobalRotation(parent);
                            Quaternion globalRot = QuaternionMultiply(parentGlobalRot, tx->GetRotation());
                            tx->UpdateGlobalRotationCache(globalRot);
                        }
                    }
                    return tx->globalRotationCache;
                }

                Transform3D(Vector3 position = Vector3Zero(), Vector3 scale = Vector3One(), Quaternion rotation = QuaternionIdentity()) 
                    : position_(position), scale_(scale), rotation_(rotation)
                {};

                Transform3D(Vector3 position, Quaternion rotation)
                    : position_(position), scale_(Vector3One()), rotation_(rotation)
                {};

                Transform3D(Vector3 position, Quaternion rotation, flecs::entity e)
                    : position_(position), scale_(Vector3One()), rotation_(rotation), entity_(e)
                {};

                Transform3D(Vector3 position, flecs::entity e)
                    : position_(position), scale_(Vector3One()), rotation_(QuaternionIdentity()), entity_(e)
                {};

                Transform3D(flecs::entity e)
                    : position_(Vector3Zero()), scale_(Vector3One()), rotation_(QuaternionIdentity()), entity_(e)
                {}

                Transform3D(const Transform3D& tx)
                    : 
                        position_(tx.GetPosition()), 
                        scale_(tx.GetScale()), 
                        rotation_(tx.GetRotation()),
                        entity_(tx.GetEntity())
                {};

                void InvalidateCacheFlags()
                {
                    globalPositionCacheDirtyFlag = true;
                    globalScaleCacheDirtyFlag = true;
                    globalRotationCacheDirtyFlag = true;

                    PropagateInvalidateCacheFlags();
                }

                void SetEntity(flecs::entity e)
                {
                    entity_ = e;
                }

                flecs::entity GetEntity() const
                {
                    return entity_;
                }

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
                    InvalidateCacheFlags();
                }
                
                Quaternion GetRotation() const
                {
                    return rotation_;
                }

                void SetGlobalTransform(Transform3D tx)
                {
                    // TODO
                    // if (!entity_.is_valid() || !entity_.has<Transform3D>()) { return; }
                    // Transform3D *eTx = entity_.get_mut<Transform3D>();
                    // if (eTx) {
                    //     SetGlobalPosition(entity_, tx.GetPosition());
                    //     SetGlobalScale(entity_, tx.GetScale());
                    //     SetGlobalRotation(entity_, tx.GetRotation());
                    // }
                }

                Transform3D GetGlobalTransform() const
                {
                    // TODO
                    // if (!entity_.is_valid() || !entity_.has<Transform3D>()) { return Transform3D(); }
                    // const Transform3D *tx = entity_.get<Transform3D>();
                    // return (tx ? *tx : Transform3D());
                }

                void SetGlobalPosition(Vector3 position)
                {
                    if (!entity_.is_valid() || !entity_.has<Transform3D>()) { 
                        DN_CORE_WARN("Entity not valid, or does not have Transform3D!");
                        return; 
                    }
                    flecs::entity parent = entity_.parent();
                    if (parent.is_valid() && parent.has<Transform3D>()) {
                        Vector3 parentGlobalPos   = GetGlobalPosition(parent);
                        Quaternion parentGlobalRot = GetGlobalRotation(parent);
                        Vector3 parentGlobalScale = GetGlobalScale(parent);
                        Vector3 offset = Vector3Subtract(position, parentGlobalPos);
                        Quaternion invParentRot = QuaternionInvert(parentGlobalRot);
                        Vector3 localPosUnscaled = Vector3RotateByQuaternion(offset, invParentRot);
                        Vector3 localPos = Vector3Divide(localPosUnscaled, parentGlobalScale);
                        SetPosition(localPos);
                    } else {
                        SetPosition(position);
                    }
                    UpdateGlobalPositionCache(position);
                }

                Vector3 GetGlobalPosition() const
                {
                    if (!entity_.is_valid() || !entity_.has<Transform3D>()) { 
                        DN_CORE_WARN("Entity not valid, or does not have Transform3D!");
                        return Vector3Zero(); 
                    }
                    Transform3D *tx = entity_.get_mut<Transform3D>();
                    if (!tx) { return Vector3Zero(); }

                    if (tx->globalPositionCacheDirtyFlag) {
                        if (!entity_.parent().is_valid() || !entity_.parent().has<Transform3D>()) {
                            tx->UpdateGlobalPositionCache(tx->GetPosition());
                        } else {
                            Transform3D parentGlobal = GetGlobalTransform(entity_.parent());
                            Vector3 scaledLocalPos = Vector3Multiply(tx->GetPosition(), parentGlobal.GetScale());
                            Vector3 rotatedPos = Vector3RotateByQuaternion(scaledLocalPos, parentGlobal.GetRotation());
                            Vector3 globalPos = Vector3Add(parentGlobal.GetPosition(), rotatedPos);
                            tx->UpdateGlobalPositionCache(globalPos);
                        }
                    }

                    return tx->globalPositionCache;
                }

                void SetGlobalScale(Vector3 scale) 
                {
                    if (!entity_.is_valid() || !entity_.has<Transform3D>()) { 
                        DN_CORE_WARN("Entity not valid, or does not have Transform3D!");
                        return; 
                    }
                    flecs::entity parent = entity_.parent();
                    if (parent.is_valid() && parent.has<Transform3D>()) {
                        Vector3 parentGlobalScale = GetGlobalScale(parent);
                        SetScale(Vector3Divide(scale, parentGlobalScale));
                    } else {
                        SetScale(scale);
                    }
                    UpdateGlobalScaleCache(scale);
                }

                Vector3 GetGlobalScale() const
                {
                    if (!entity_.is_valid() || !entity_.has<Transform3D>()) { 
                        DN_CORE_WARN("Entity not valid, or does not have Transform3D!");
                        return Vector3Zero(); 
                    }
                    Transform3D* tx = entity_.get_mut<Transform3D>();
                    if (!tx) { return Vector3One(); }
                    if (tx->globalScaleCacheDirtyFlag) {
                        if (!entity_.parent().is_valid() || !entity_.parent().has<Transform3D>()) {
                            tx->UpdateGlobalScaleCache(tx->GetScale());
                        } else {
                            Vector3 parentGlobalScale = GetGlobalScale(entity_.parent());
                            Vector3 globalScale = Vector3Multiply(parentGlobalScale, tx->GetScale());
                            tx->UpdateGlobalScaleCache(globalScale);
                        }
                    }

                    return tx->globalScaleCache;
                }

                void SetGlobalRotation(Quaternion rotation)
                {
                    if (!entity_.is_valid() || !entity_.has<Transform3D>()) { 
                        DN_CORE_WARN("Entity not valid, or does not have Transform3D!");
                        return; 
                    }
                    flecs::entity parent = entity_.parent();
                    if (parent.is_valid() && parent.has<Transform3D>()) {
                        // Global = ParentGlobal * Local  =>  Local = inverse(ParentGlobal) * Global
                        Quaternion parentGlobalRot = GetGlobalRotation(parent);
                        Quaternion invParentRot = QuaternionInvert(parentGlobalRot);
                        Quaternion localRotation = QuaternionMultiply(invParentRot, rotation);
                        SetRotation(localRotation);
                    } else {
                        SetRotation(rotation);
                    }
                    UpdateGlobalRotationCache(rotation);
                }

                Quaternion GetGlobalRotation() const
                {
                    if (!entity_.is_valid() || !entity_.has<Transform3D>()) { 
                        DN_CORE_WARN("Entity not valid, or does not have Transform3D!");
                        return QuaternionIdentity(); 
                    }
                    Transform3D* tx = entity_.get_mut<Transform3D>();
                    if (!tx) { return QuaternionIdentity(); }
                    if (tx->globalRotationCacheDirtyFlag) {
                        flecs::entity parent = entity_.parent();
                        if (!parent.is_valid() || !parent.has<Transform3D>()) {
                            // No parent: global rotation is the local rotation.
                            tx->UpdateGlobalRotationCache(tx->GetRotation());
                        } else {
                            // Global = ParentGlobal * LocalRotation
                            Quaternion parentGlobalRot = GetGlobalRotation(parent);
                            Quaternion globalRot = QuaternionMultiply(parentGlobalRot, tx->GetRotation());
                            tx->UpdateGlobalRotationCache(globalRot);
                        }
                    }
                    return tx->globalRotationCache;
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
                flecs::entity entity_ = flecs::entity::null();

                static void InvalidateCacheFlags(flecs::entity e)
                {
                    if (!e.is_valid() || !e.has<Transform3D>()) { 
                        DN_CORE_WARN("STATIC Entity not valid, or does not have Transform3D!");
                        return; 
                    }
                    Transform3D *tx = e.get_mut<Transform3D>();
                    if (!tx) { return; }

                    tx->InvalidateCacheFlags();
                }

                void PropagateInvalidateCacheFlags()
                {
                    if (!e.is_valid() || !e.has<Transform3D>()) { 
                        DN_CORE_WARN("STATIC Entity not valid, or does not have Transform3D!");
                        return; 
                    }
                    entity_.children([](flecs::entity child) {
                        if (!child.is_valid() || !child.has<Transform3D>()) { 
                            DN_CORE_WARN("Child not valid, or does not have Transform3D!");
                            return; 
                        }
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



    /*------------------------------------------------------------------------*/
    //  ECS Manager Class
    /*------------------------------------------------------------------------*/
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

        /*----------------------------------------------------------------------  
         * Transform Updates
        ----------------------------------------------------------------------*/  
        void ExecuteQueryTransform3DHierarchicalUpdate();
        void ExecuteQueryCharacterBody3DUpdateTransform();
        void ExecuteQuerySyncDynamicBody3DTransform();



        void ExecuteQuerySetCameraAsActive();
  
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

