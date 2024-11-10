#pragma once

#include "../MathsCore.h"
#include <corecrt_math.h>
#include <cmath>

namespace Duin
{
	#define EPSILON std::numeric_limits<float>::epsilon()

	class DUIN_MATHS_API Maths
	{
	public:
		static bool AreSame(double a, double b)
		{
			return std::fabs(a - b) < EPSILON;
		}

		static bool AreSame(float a, float b)
		{
			return std::fabs(a - b) < EPSILON;
		}

		static double RadiansToDegrees(double radians) {
			return radians * (180.0 / 3.14159265358979323846f);
		}

		static double DegreesToRadians(double degrees) {
			return degrees * (3.14159265358979323846f / 180.0);
		}
	};
}