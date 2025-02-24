#include "dnpch.h"

#include "PhysX_CharacterBody.h"
#include "Duin/Core/Maths/DuinMaths.h"


namespace duin {

    PhysXCharacterBody::~PhysXCharacterBody()
    {
        // TODO clean physx objects
    }

    Vector3 PhysXCharacterBody::GetPosition()
    {
        return Vector3();
    }

    Vector3 PhysXCharacterBody::GetFootPosition()
    {
        return Vector3();
    }

    Vector3 PhysXCharacterBody::GetCurrentVelocity()
    {
        return Vector3();
    }

    int PhysXCharacterBody::IsOnFloor()
    {
        return 0;
    }

    int PhysXCharacterBody::IsOnFloorOnly()
    {
        return 0;
    }

    void PhysXCharacterBody::Move(Vector3 displacement, double delta)
    {
    }

    void PhysXCharacterBody::OnShapeHit()
    {
    }

    void PhysXCharacterBody::OnCharacterHit()
    {
    }

    void PhysXCharacterBody::OnObstacleHit()
    {
    }
}
