#include "dnpch.h"

#include "Profiler.h"

namespace Duin
{
    Profiler::Profiler()
    {
        fpsBuffer = std::vector<float>(600, 0.0f);
        frametimeBuffer = std::vector<float>(600, 0.0f);
    }

    Profiler::~Profiler()
    {
    }

    float Profiler::GetFramerate()
    {
        return (float)(::GetFPS());
    }

    float Profiler::GetFrametime()
    {
        return ::GetFrameTime();
    }

    void Profiler::ShowFPS()
    {
        float framerate = GetFramerate();
        float frametime = GetFrametime() * 1000.0f;
        UpdateFrametimeBuffer(frametime);

        ImGui::Begin("FPS", &fpsDisplayEnabled, ImGuiWindowFlags_MenuBar);

        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Framerate: %.0f fps", framerate);
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Frametime: %.1f ms", frametime);

        // Set the graph size and scale
        ImVec2 graphSize(250, 100); // Width and height of the graph
        float minScale = 0.0f; // Minimum value of the scale
        float maxScale = 100.0f; // Maximum value of the scale

        // Generate samples and plot them
        ImGui::PlotLines("Frametime", frametimeBuffer.data(), frametimeBuffer.size(), 0, nullptr, minScale, maxScale, graphSize);
    
        ImGui::End();
    }

    void Profiler::ShowUserDebug()
    {
        ImGui::Begin("User Output", &userDebugEnabled, ImGuiWindowFlags_MenuBar);
        for (auto& text : usertextBuffer)
        {
            ImGui::TextColored(ImVec4(1, 1, 0, 1), text.c_str());
        }
        usertextBuffer.clear();
        ImGui::End();
    }

    void Profiler::Draw()
    {
        ImGui::Begin("Menu", &profilerEnabled, ImGuiWindowFlags_MenuBar);

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Options"))
            {
                if (ImGui::Checkbox("FPS Menu", &fpsDisplayEnabled)) {}
                if (ImGui::Checkbox("User Debug", &userDebugEnabled)) {}
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        ImGui::End();

        if (fpsDisplayEnabled) { ShowFPS(); }
        if (userDebugEnabled) { ShowUserDebug(); }
    }

    void Profiler::DrawText(std::string text)
    {
        usertextBuffer.push_back(text);
    }

    void Profiler::UpdateFPSBuffer(float fps)
    {
        std::rotate(fpsBuffer.begin(), fpsBuffer.begin() + 1, fpsBuffer.end());
        fpsBuffer.back() = fps;
    }
    void Profiler::UpdateFrametimeBuffer(float frametime)
    {
        std::rotate(frametimeBuffer.begin(), frametimeBuffer.begin() + 1, frametimeBuffer.end());
        frametimeBuffer.back() = frametime;
    }
}