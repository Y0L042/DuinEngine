#include "dnpch.h"

#include "Camera2D.h"

namespace Duin
{
	Camera2D::Camera2D()
	{
	}

	Camera2D::~Camera2D()
	{
	}

	::Camera2D Camera2D::GetRaylibCamera()
	{
		::Camera2D rlCamera;
		rlCamera.offset = { offset.x, offset.y };
		rlCamera.target = { target.x, target.y };
		rlCamera.rotation = rotation_deg;
		rlCamera.zoom = zoom;

		return rlCamera;
	}
}