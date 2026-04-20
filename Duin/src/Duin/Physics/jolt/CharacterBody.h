#pragma once
#include "PhysicsObject.h"
#include "Duin/Core/Maths/DuinMaths.h"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Character/CharacterID.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <memory>
#include "CollisionShape.h"

namespace duin
{

struct CharacterBodyDesc
{
    float mass;
    float maxStrength;
    float maxSlopeAngle;
    bool enableStairStepping;
    bool enableStickToFloor;
    bool playerCanPushOtherCharacters;
    bool otherCharactersCanPushPlayer;
};

class CharacterBody : public PhysicsObject
{
  public:
    CharacterBody(CharacterBodyDesc bodyDesc, CollisionShapeDesc shapeDesc, Vector3 position);
    ~CharacterBody();

    virtual void Initialize(Vector3 position);
    void CreateBody(const CollisionShape &shape, const Vector3 &position, const Quaternion &rotation = Quaternion());

    void Move(Vector3 displacement, double delta);

    void SetPosition(Vector3 position);
    Vector3 GetPosition() override;

    void SetFootPosition(Vector3 position);
    Vector3 GetFootPosition();
    Vector3 GetCurrentVelocity();
    int IsOnFloor();
    int IsOnFloorOnly();

    void OnShapeHit(/*TODO*/);
    void OnCharacterHit(/*TODO*/);
    void OnObstacleHit(/*TODO*/);

    int OnFloorShapeCast(double delta);

    CharacterBody(const CharacterBody &) = delete;
    CharacterBody &operator=(const CharacterBody &) = delete;

  private:
    JPH::CharacterID characterID;
    JPH::Ref<JPH::CharacterVirtual> character;

    Vector3 currentVelocity = Vector3Zero();
    double onFloorGrace = 0.1;
    double timeWhenLastMoved = 0.0;
    double timeSinceOnFloor = 0.0;
    int isOnFloor = 0;

    CharacterBodyDesc bodyDesc;
    CollisionShapeDesc shapeDesc;
};

} // namespace duin
