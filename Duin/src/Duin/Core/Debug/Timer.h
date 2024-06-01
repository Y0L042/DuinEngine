#pragma once

#include "Duin/Core/Core.h"

#include <chrono>

namespace Duin
{
	class DUIN_API Timer
	{
	public:
		Timer()
		{
			startTimePoint = std::chrono::high_resolution_clock::now();
		}

		~Timer()
		{

		}

		double Stop()
		{
			auto endTimePoint = std::chrono::high_resolution_clock::now();
			long long start = std::chrono::time_point_cast<std::chrono::microseconds>(startTimePoint).time_since_epoch().count();
			long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();

			duration = end - start;
			return duration;
		}

		double GetTime()
		{
			return duration;
		}

	private:
		std::chrono::time_point<std::chrono::steady_clock> startTimePoint;
		double duration = 0.0;
	};
}