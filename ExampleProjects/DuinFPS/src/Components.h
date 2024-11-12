#pragma once

#include <Duin.h>

// Macro to define 3D components
#define DEFINE_VECTOR3_COMPONENT(name)         \
struct name {                                  \
    Vector3 value;                             \
                                               \
    name() : value{0.0f, 0.0f, 0.0f} {}        \
    name(Vector3 v) : value(v) {}                  \
    name(float x, float y, float z) : value{x, y, z} {} \
                                               \
    name& operator=(const Vector3& vec) {      \
        this->value = vec;                      \
        return *this;                           \
    }                                          \
                                               \
    name& operator=(std::initializer_list<float> list) { \
        auto it = list.begin();                 \
        if (list.size() == 3) {                 \
            this->value.x = *it++;               \
            this->value.y = *it++;               \
            this->value.z = *it;                 \
        }                                        \
        return *this;                            \
    }                                            \
};

// Macro to define 2D components
#define DEFINE_VECTOR2_COMPONENT(name)         \
struct name {                                  \
    Vector2 value;                             \
                                               \
    name() : value{0.0f, 0.0f} {}               \
    name(Vector2 v) : value(v) {}            \
    name(float x, float y) : value{x, y} {}     \
                                               \
    name& operator=(const Vector2& vec) {      \
        this->value = vec;                      \
        return *this;                           \
    }                                          \
                                               \
    name& operator=(std::initializer_list<float> list) { \
        auto it = list.begin();                 \
        if (list.size() == 2) {                 \
            this->value.x = *it++;               \
            this->value.y = *it;                 \
        }                                        \
        return *this;                            \
    }                                            \
};                                              


/* Components */
DEFINE_VECTOR3_COMPONENT(Position3D)
DEFINE_VECTOR3_COMPONENT(Scale3D)
DEFINE_VECTOR3_COMPONENT(Velocity3D)
DEFINE_VECTOR3_COMPONENT(AngularVelocity3D)
DEFINE_VECTOR3_COMPONENT(MovementInput3D)
DEFINE_VECTOR3_COMPONENT(RotationInput3D)

DEFINE_VECTOR2_COMPONENT(PlayerMovementInputVec2)
DEFINE_VECTOR2_COMPONENT(MouseInputVec2)

typedef struct Rotation3D {
    Quaternion starting;
    Quaternion value;

    Rotation3D() 
        :   starting(QuaternionIdentity()), 
            value(QuaternionIdentity()) {}
    Rotation3D(Quaternion q) 
        :   starting(q), 
            value(q) {}
    Rotation3D(float x, float y, float z, float w) 
        :   starting({x, y, z, w}), 
            value({x, y, z, w}) {}

    Rotation3D& operator=(std::initializer_list<float> list) { 
        auto it = list.begin();                 
        if (list.size() == 4) {                 
            this->value.x = *it++;               
            this->value.y = *it++;                 
            this->value.z = *it++;                 
            this->value.w = *it;                 
        }                                        
        return *this;                            
    }                                            
} Rotation3D;

struct PxCapsuleCharacter3DCreation {
    physx::PxCapsuleControllerDesc desc;
    duin::UUID characterID;
};

struct PxCapsuleCharacter3D {
    duin::UUID id;
};

/* Tags */
struct Local {};
struct Global {};
struct PlayerTag {};
struct CameraTag {};
struct ExternalRefTag {};
struct BasicControlledTag {};
struct PxControlledTag {};
struct PxKinematicTag {};
struct PxDynamicTag {};
struct PxStaticTag {};

#undef DEFINE_VECTOR3_COMPONENT
#undef DEFINE_VECTOR2_COMPONENT


