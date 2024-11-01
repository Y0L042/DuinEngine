#include "dnpch.h"
#include "debugging_panel.h"

#include <chrono>

#include "Duin/core/debug/dn_log.h" 



namespace Duin {
    DebugConsole::DebugConsole()
    {
        startTime = std::chrono::steady_clock::now();
    }

    DebugConsole::~DebugConsole() {}

    void DebugConsole::Clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        logMessages_.clear();
    }

    void DebugConsole::Log(const char* format, ...) {
        std::lock_guard<std::mutex> lock(mutex_);

        // Buffer to store the formatted string
        char buffer[1024]; // Adjust size as needed

        // Initialize variadic arguments
        va_list args;
        va_start(args, format);

        // Format the message using vsnprintf
        vsnprintf(buffer, sizeof(buffer), format, args);

        // End variadic arguments
        va_end(args);

        // Get time since game start
        std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = currentTime - startTime; // Floating-point seconds

        // Format the log message with the timestamp to 1 decimal place
        std::ostringstream timestampedMessage;
        timestampedMessage << "[" << std::fixed << std::setprecision(1) << elapsed.count() << "s] " << buffer;

        logMessages_.emplace_back(timestampedMessage.str());
        newMessageAdded_ = 1;
    }

    void DebugConsole::Draw(const char* title) {
        // Set semitransparent background for the entire console window

        float bgAlpha = 0.8f;
        float linesAlpha = 0.875f;
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, bgAlpha));
        ImGui::Begin(title, nullptr, ImGuiWindowFlags_MenuBar);

        // Title bar menu with dropdown for AutoScroll, Copy, and Clear
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Options")) {

                // AutoScroll toggle
                if (ImGui::MenuItem("AutoScroll", nullptr, scrollToBottom_)) {
                    scrollToBottom_ = !scrollToBottom_;
                }

                // Copy to Clipboard
                ImGui::Separator();
                if (ImGui::MenuItem("Copy")) {
                    std::string clipboardContent;
                    for (const auto& message : logMessages_) {
                        clipboardContent += message + "\n";
                    }
                    ImGui::SetClipboardText(clipboardContent.c_str());
                }

                // Clear log
                ImGui::Separator();
                if (ImGui::MenuItem("Clear")) {
                    Clear();
                }

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

/*
        // Right-align Clear button
        if (ImGui::BeginChild("TitleBar", ImVec2(0, 25), false)) {
            ImGui::SetCursorPosX(ImGui::GetWindowWidth());

            ImGui::SameLine();
            if (ImGui::Button("Copy")) {
                std::string clipboardContent;
                for (const auto& message : logMessages_) {
                    clipboardContent += message + "\n";
                }
                ImGui::SetClipboardText(clipboardContent.c_str());
            }

            ImGui::SameLine();
            ImGui::Checkbox("AutoScroll", &scrollToBottom_);

            // Clear button on the right
            float rightPosX = ImGui::GetWindowWidth() - 60;
            ImGui::SameLine();
            ImGui::SetCursorPosX(rightPosX);
            if (ImGui::Button("Clear")) Clear();
        }
        ImGui::EndChild();
*/
        // Scroll region for messages
        ImGui::Separator();
        ImGui::BeginChild("ScrollRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);


        
        // Define alternating background colors for rows
        ImVec4 color1 = ImVec4(0.2f, 0.2f, 0.2f, linesAlpha); // Light gray
        ImVec4 color2 = ImVec4(0.15f, 0.15f, 0.15f, linesAlpha); // Darker gray
        bool alternateColor = false;

        // Display each log message with an alternating row background
        for (const auto& message : logMessages_) {
            // Calculate the position and size of each row
            ImVec2 cursorPos = ImGui::GetCursorScreenPos();
            ImVec2 textSize = ImGui::CalcTextSize(message.c_str());
            ImVec2 rowSize = ImVec2(ImGui::GetContentRegionAvail().x, textSize.y + ImGui::GetStyle().ItemSpacing.y);

            // Draw a filled rectangle behind each row for the background color
            ImGui::GetWindowDrawList()->AddRectFilled(cursorPos, ImVec2(cursorPos.x + rowSize.x, cursorPos.y + rowSize.y),
                                                      ImGui::GetColorU32(alternateColor ? color2 : color1));

            // Draw the text
            ImGui::TextUnformatted(message.c_str());

            // Toggle background color for the next row
            alternateColor = !alternateColor;
        }
       

/*
        // Display each log message with alternate shading
        bool alternateColor = false;
        for (std::vector<std::string>::const_iterator it = logMessages_.begin(); it != logMessages_.end(); ++it) {
            if (alternateColor) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f)); // Gray color for alternating lines
            }
            ImGui::TextUnformatted(it->c_str());
            if (alternateColor) {
                ImGui::PopStyleColor();
            }
            alternateColor = !alternateColor; // Toggle color
        }
*/
        // Scroll to bottom if new messages are added
        if (scrollToBottom_ && newMessageAdded_) {
            ImGui::SetScrollHereY(1.0f);
            newMessageAdded_ = false;
        }
        ImGui::EndChild();

        ImGui::End();
        ImGui::PopStyleColor();
    }

    /* void DebugConsole::Draw(const char* title) {
        ImGui::Begin(title);

        // Scroll to the bottom when new messages are added
        if (scrollToBottom_) {
            ImGui::SetScrollHereY(1.0f);
            scrollToBottom_ = false;
        }

        // Display each log message
        for (const auto& message : logMessages_) {
            ImGui::TextUnformatted(message.c_str());
        }

        // Auto-scroll checkbox and clear button
        if (ImGui::Button("Clear")) Clear();
        ImGui::End();
    } */

    void DebugConsole::SetScrollToBottom() { scrollToBottom_ = true; }


    void DebugConsole::LogEx(LogLevel level, const char* format, ...) {
        char buffer[1024];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
       

        // Log to the main console logger
        switch (level) {
        case LogLevel::Fatal:
            DN_FATAL("{0}", buffer);
            break;
        case LogLevel::Error:
            DN_ERROR("{0}", buffer);
            break;
        case LogLevel::Warn:
            DN_WARN("{0}", buffer);
            break;
        case LogLevel::Info:
            DN_INFO("{0}", buffer);
            break;
        case LogLevel::Trace:
            DN_TRACE("{0}", buffer);
            break;
        }
        this->Log("%s", buffer);
    }

}
