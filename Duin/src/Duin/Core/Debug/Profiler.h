#pragma once

#include "Duin/Core/Core.h"

#include <RLImGuiComponent.h>

#include <vector>

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

		void Draw();

		void DrawText(const char* text);

	private:
		bool enabled = true;
		std::vector<float> fpsBuffer;
		std::vector<float> frametimeBuffer;
		std::vector<const char*> usertextBuffer;

		void UpdateFPSBuffer(float fps);
		void UpdateFrametimeBuffer(float frametime);
	};
}