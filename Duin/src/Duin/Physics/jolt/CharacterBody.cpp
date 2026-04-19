#include "dnpch.h"
#include "CharacterBody.h"

std::shared_ptr<duin::CharacterBody> duin::CharacterBody::Create(duin::CharacterBodyDesc desc, duin::Vector3 position)
{
    return std::make_shared<duin::CharacterBody>(desc, position);
}

duin::CharacterBody::CharacterBody(duin::CharacterBodyDesc desc, duin::Vector3 position)
    : desc(desc)
{
}

duin::CharacterBody::~CharacterBody()
{
}

void duin::CharacterBody::SetPosition(duin::Vector3 position)
{
}

duin::Vector3 duin::CharacterBody::GetPosition()
{
    return {};
}

void duin::CharacterBody::SetFootPosition(duin::Vector3 position)
{
}

duin::Vector3 duin::CharacterBody::GetFootPosition()
{
    return {};
}

duin::Vector3 duin::CharacterBody::GetCurrentVelocity()
{
    return {};
}

int duin::CharacterBody::IsOnFloor()
{
    return 0;
}

int duin::CharacterBody::IsOnFloorOnly()
{
    return 0;
}

void duin::CharacterBody::Move(duin::Vector3 displacement, double delta)
{
}

void duin::CharacterBody::OnShapeHit()
{
}

void duin::CharacterBody::OnCharacterHit()
{
}

void duin::CharacterBody::OnObstacleHit()
{
}

int duin::CharacterBody::OnFloorShapeCast(double delta)
{
    return 0;
}

const duin::CharacterBodyDesc duin::CharacterBody::GetDescriptor() const
{
    return desc;
}
