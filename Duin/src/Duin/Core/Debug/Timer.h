#pragma once

#include "Duin/Core/Core.h"

#include <chrono>

namespace Duin
{
	class Timer
	{
	public:
		Timer()
		{
			startTimePoint = std::chrono::high_resolution_clock::now();
			lapTimePoint = startTimePoint;
		}

		~Timer()
		{

		}

		double Lap()
		{
			std::chrono::time_point<std::chrono::steady_clock> endTimePoint = std::chrono::high_resolution_clock::now();
			long long lap = std::chrono::time_point_cast<std::chrono::microseconds>(lapTimePoint).time_since_epoch().count();
			long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();

			duration = end - lap;
			lapTimePoint = endTimePoint;
			return duration;
		}

		double Time()
		{
			std::chrono::time_point<std::chrono::steady_clock> endTimePoint = std::chrono::high_resolution_clock::now();
			long long start = std::chrono::time_point_cast<std::chrono::microseconds>(startTimePoint).time_since_epoch().count();
			long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();

			duration = end - start;
			return duration;
		}

		double Stop()
		{
			auto endTimePoint = std::chrono::high_resolution_clock::now();
			long long start = std::chrono::time_point_cast<std::chrono::microseconds>(startTimePoint).time_since_epoch().count();
			long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();

			duration = end - start;
			startTimePoint = std::chrono::high_resolution_clock::now();
			return duration;
		}

	private:
		std::chrono::time_point<std::chrono::steady_clock> startTimePoint;
		std::chrono::time_point<std::chrono::steady_clock> lapTimePoint;
		double duration = 0.0;
		
	};
}