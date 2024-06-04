#pragma once

#include "Duin/Core/Core.h"

#include <RLImGuiComponent.h>
#include <fmt/core.h>

#include <vector>
#include <string>

namespace Duin
{
	class DUIN_API Profiler
	{
	public:
		Profiler();
		~Profiler();

		float GetFramerate();
		float GetFrametime();

		void ShowFPS();
		void ShowUserDebug();

		void Draw();

		void DrawText(std::string text);

	private:
		bool profilerEnabled = true;
		bool fpsDisplayEnabled = true;
		bool userDebugEnabled = true;
		std::vector<float> fpsBuffer;
		std::vector<float> frametimeBuffer;
		std::vector<std::string> usertextBuffer;

		void UpdateFPSBuffer(float fps);
		void UpdateFrametimeBuffer(float frametime);
	};
}