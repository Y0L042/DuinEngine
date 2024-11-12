#pragma once
#include <Duin.h>
#include "Components.h"

class ECSManager
{
public:
    flecs::world world;

    ECSManager();
    ~ECSManager();

    flecs::world GetWorld();

    void RegisterComponents();
    void CreateQueries();
    void FreeQueries();
    void CreatePrefabs();

    void ExecuteQueryUpdatePosition3D(double delta);
    void ExecuteQueryHierarchicalUpdatePosition3D();

    void ExecuteQueryUpdateRotation3D(double delta);
    void ExecuteQueryHierarchicalUpdateRotation3D();

    void ExecuteQueryControlCamera(double delta);

    void ExecuteQueryMovementInput();

    void ExecuteQueryUpdatePlayerYaw(double delta);
    void ExecuteQueryUpdateCameraPitch(double delta);

    void ExecuteQueryControlPlayerMovement(double delta);

    void ExecuteQueryCreatePxCapsuleCharacter3D(std::unordered_map<duin::UUID, physx::PxController *>& controllerRegistry, physx::PxControllerManager *pxManager);
    void ExecuteQueryMovePxCapsuleCharacter3D(double delta);

private:

    flecs::query<Position3D, const Velocity3D, const Rotation3D> queryUpdatePosition3D;
    flecs::query<const Position3D, const Position3D *, Position3D> queryHierarchicalUpdatePosition3D;

    flecs::query<Rotation3D, const AngularVelocity3D> queryUpdateRotation3D;
    flecs::query<const Rotation3D, const Rotation3D *, Rotation3D> queryHierarchicalUpdateRotation3D;

    flecs::query<Velocity3D, const MovementInput3D> queryMovementInput;
    flecs::query<AngularVelocity3D, const RotationInput3D> queryRotationInput;

    flecs::query<Rotation3D, RotationInput3D, MouseInputVec2> queryUpdatePlayerYaw;
    flecs::query<Rotation3D, RotationInput3D, MouseInputVec2> queryUpdateCameraPitch;

    flecs::query<MovementInput3D, const PlayerMovementInputVec2> queryControlPlayerMovement;
    flecs::query<RotationInput3D, const MouseInputVec2> queryControlPlayerRotation;

    flecs::query<Camera3D, const Position3D, const Rotation3D> queryControlCamera;

    flecs::query<Camera3D, const Position3D> queryUpdateCameraPosition;

    flecs::query<PxCapsuleCharacter3DCreation> queryCreatePxCapsuleCharacter3D;

    flecs::query<PxCapsuleCharacter3D> queryMovePxCapsuleCharacter3D;
};


