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
}