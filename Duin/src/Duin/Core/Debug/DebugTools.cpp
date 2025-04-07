#include "dnpch.h"
#include "DebugTools.h"

#include <chrono>
#include <external/imgui.h>
#include "Duin/Core/Debug/DNLog.h" 



namespace duin {

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
    newMessageAdded = 1;
}

void DebugConsole::Draw(const char* title) 
{
    ImGuiWindowFlags windowFlags;
    // TODO
    // if(IsKeyPressed(KEY_O)) {
    //     ToggleEditing();
    //     DN_CORE_INFO("DebugConsoleEditing {}", enableEditing);
    // }
    if (!enableEditing && enableSilent) {
        ImGui::SetWindowFocus(NULL);
        windowFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove;
    } else {
        windowFlags = ImGuiWindowFlags_MenuBar;
    }
    // Set semitransparent background for the entire console window

    float bgAlpha = 0.8f;
    float linesAlpha = 0.875f;
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, bgAlpha));
    // ImGui::Begin(title, nullptr, ImGuiWindowFlags_MenuBar);

    ImGui::Begin(title, nullptr, windowFlags);

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
   

    // Scroll to bottom if new messages are added
    if (scrollToBottom_ && newMessageAdded) {
        ImGui::SetScrollHereY(1.0f);
        newMessageAdded = false;
    }
    ImGui::EndChild();

    ImGui::End();
    ImGui::PopStyleColor();
}

void DebugConsole::ToggleEditing()
{
    enableEditing = !enableEditing;
}


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

namespace duin {
DebugWatchlist::DebugWatchlist()
{}

DebugWatchlist::~DebugWatchlist()
{}

void DebugWatchlist::Clear()
{
    // std::lock_guard<std::mutex> lock(mutex_);

    watchlist.clear();
    order.clear();
}

void DebugWatchlist::Post(const std::string description, const std::string format, ...)
{
    std::lock_guard<std::mutex> lock(mutex_);

    // Initialize variadic arguments
    va_list args;
    va_start(args, format);

    // Estimate required buffer size
    // +1 for null terminator
    int size = std::vsnprintf(nullptr, 0, format.c_str(), args) + 1; 
    va_end(args); // End args before restarting

    std::vector<char> buffer(size);
    // Restart args for actual formatting
    va_start(args, format); 
    std::vsnprintf(buffer.data(), buffer.size(), format.c_str(), args);
    va_end(args);
    // End variadic arguments

    // Convert buffer to std::string
    std::string value(buffer.data());

    if (watchlist.find(description) == watchlist.end()) {
        watchlist[description] = value;
        order.push_back(description);
    } else {
        watchlist[description] = value;
    }

}

void DebugWatchlist::Draw(const char *title)
{
    ImGuiWindowFlags windowFlags;
	// TODO
    //if(IsKeyPressed(KEY_O)) {
    //    ToggleEditing();
    //    DN_CORE_INFO("DebugWatchlistEditing {}", enableEditing);
    //}
    if (!enableEditing && enableSilent) {
        ImGui::SetWindowFocus(NULL);
        windowFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove;
    } else {
        windowFlags = ImGuiWindowFlags_MenuBar;
    }

    std::lock_guard<std::mutex> lock(mutex_); // Ensure thread safety

    // Begin a new ImGui window with the given title
    float bgAlpha = 0.8f;
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, bgAlpha));
    // if ( ImGui::Begin(title, nullptr, ImGuiWindowFlags_MenuBar)) {

    if (ImGui::Begin(title, nullptr, windowFlags)) {

        // Title bar menu with dropdown for AutoScroll, Copy, and Clear
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Options")) {

                // Clear log
                if (ImGui::MenuItem("Clear")) {
                    Clear();
                }
                ImGui::Separator();

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // Begin a two-column table for the description-value pairs
        if (ImGui::BeginTable("WatchlistTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            // Iterate over `order` to preserve insertion order
            for (const auto& key : order) {
                // Fetch the value from `watchlist`
                const std::string& value = watchlist[key];

                // Start a new row
                ImGui::TableNextRow();

                // First column: Description
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(key.c_str());

                // Second column: Value
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(value.c_str());
            }

            // End the table
            ImGui::EndTable();
        }
        ImGui::End(); // End the ImGui window
        ImGui::PopStyleColor();
    }
}

void DebugWatchlist::ToggleEditing()
{
    enableEditing = !enableEditing;
}

}
