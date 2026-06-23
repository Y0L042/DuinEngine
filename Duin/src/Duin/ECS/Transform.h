#pragma once
#include "DECS/DECS.h"
#include "Duin/Core/Maths/DuinMaths.h"
#include "ECSComponents.h"

namespace duin::transform
{

void SetGlobalTransform(duin::Entity e, ECSComponent::Transform3D tx);
ECSComponent::Transform3D GetGlobalTransform(duin::Entity e);
void SetGlobalPosition(duin::Entity e, Vector3 position);
Vector3 GetGlobalPosition(duin::Entity e);
void SetGlobalScale(duin::Entity e, Vector3 scale);
Vector3 GetGlobalScale(duin::Entity e);
void SetGlobalRotation(duin::Entity e, Quaternion rotation);
Quaternion GetGlobalRotation(duin::Entity e);

} // namespace duin::transform