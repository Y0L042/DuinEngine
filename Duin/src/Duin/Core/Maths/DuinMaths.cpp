#include "dnpch.h"

#include "DuinMaths.h"

namespace duin
{
	const Vector2 Vector2::ZERO = { 0.0f, 0.0f };
	const Vector2 Vector2::ONE = { 1.0f, 1.0f };
	const Vector2 Vector2::X = { 1.0f, 0.0f };
	const Vector2 Vector2::Y = { 0.0f, 1.0f };
	const Vector2 Vector2::UP = { 0.0f, 1.0f };
	const Vector2 Vector2::DOWN = { 0.0f, -1.0f };
	const Vector2 Vector2::RIGHT = { 1.0f, 0.0f };
	const Vector2 Vector2::LEFT = { -1.0f, 0.0f };


	const Vector3 Vector3::ZERO = { 0.0f, 0.0f, 0.0f };
	const Vector3 Vector3::ONE = { 1.0f, 1.0f, 1.0f };
	const Vector3 Vector3::X = { 1.0f, 0.0f, 0.0f };
	const Vector3 Vector3::Y = { 0.0f, 1.0f, 0.0f };
	const Vector3 Vector3::Z = { 0.0f, 0.0f, 1.0f };
	const Vector3 Vector3::UP = { 0.0f, 1.0f, 0.0f };
	const Vector3 Vector3::DOWN = { 0.0f, -1.0f, 0.0f };
	const Vector3 Vector3::LEFT = { -1.0f, 0.0f, 0.0f };
	const Vector3 Vector3::RIGHT = { 1.0f, 0.0f, 0.0f };
	const Vector3 Vector3::TOWARD = { 0.0f, 0.0f, 1.0f };
	const Vector3 Vector3::AWAY = { 0.0f, 0.0f, -1.0f };

    const Vector4 Vector4::IDENTITY_QUAT = Vector4(0.0f, 0.0f, 0.0f, 1.0f); // Quaternion identity
    const Vector4 Vector4::ZERO   = Vector4(0.0f, 0.0f, 0.0f, 0.0f); // All components zero
    const Vector4 Vector4::ONE    = Vector4(1.0f, 1.0f, 1.0f, 1.0f); // All components one
    const Vector4 Vector4::X      = Vector4(1.0f, 0.0f, 0.0f, 0.0f); // Unit vector along X
    const Vector4 Vector4::Y      = Vector4(0.0f, 1.0f, 0.0f, 0.0f); // Unit vector along Y
    const Vector4 Vector4::Z      = Vector4(0.0f, 0.0f, 1.0f, 0.0f); // Unit vector along Z
    const Vector4 Vector4::UP     = Vector4(0.0f, 1.0f, 0.0f, 0.0f); // Same as +Y
    const Vector4 Vector4::DOWN   = Vector4(0.0f, -1.0f, 0.0f, 0.0f);
    const Vector4 Vector4::LEFT   = Vector4(-1.0f, 0.0f, 0.0f, 0.0f);
    const Vector4 Vector4::RIGHT  = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
    const Vector4 Vector4::TOWARD = Vector4(0.0f, 0.0f, -1.0f, 0.0f); // Forward in -Z
    const Vector4 Vector4::AWAY   = Vector4(0.0f, 0.0f, 1.0f, 0.0f);  // Backward in +Z
}
