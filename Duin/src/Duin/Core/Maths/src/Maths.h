#pragma once
#include <corecrt_math.h>

namespace Duin
{
	#define EPSILON 0.05

	class Maths
	{
	public:
		static bool AreSame(double a, double b)
		{
			return fabs(a - b) < EPSILON;
		}

		static bool AreSame(float a, float b)
		{
			return fabs(a - b) < EPSILON;
		}
	};
}