#pragma once

#include <flecs.h>

namespace duin {

/*----------------------------------------------------------------------------*/
//  2D Components
/*----------------------------------------------------------------------------*/
struct LocalPosition2D {
    Vector2 value;
};

struct LocalRotation2D {
    float value;
};

struct LocalScale2D {
    Vector2 value;
};

struct LocalTransform2D {
    LocalPosition2D position;
    LocalRotation2D rotation;
    LocalScale2D scale;
};

struct LocalVelocity2D {
    Vector2 value;
};

/*----------------------------------------------------------------------------*/
//  3D Components
/*----------------------------------------------------------------------------*/
struct LocalPosition3D {
    Vector3 value;
};

struct LocalRotation3D {
    Quaternion value;
};

struct LocalScale3D {
    Vector3 value;
};

struct LocalTransform3D {
    LocalPosition3D position;
    LocalRotation3D rotation;
    LocalScale3D scale;
};

struct LocalVelocity3D {
    Vector3 value;
};

/*----------------------------------------------------------------------------*/
//  Tags
/*----------------------------------------------------------------------------*/
//struct ExternalRefTag {};
//struct BasicControlledTag {};
//struct PxControlledTag {};
//struct PxKinematicTag {};
//struct PxDynamicTag {};
//struct PxStaticTag {};

class ECSManager
{
public:
    flecs::entity Node3D;
    flecs::entity CharacterBody3D;

    ECSManager();
    ~ECSManager();

    flecs::world& GetWorld();
    void RegisterComponents();
    void CreatePrefabs();

private:
    flecs::world world;
};

}

