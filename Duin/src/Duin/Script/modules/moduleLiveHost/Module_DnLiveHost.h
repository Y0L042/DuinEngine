#pragma once

#include <daScript/daScript.h>
#include <string>
#include <unordered_map>
#include <vector>

struct DnLiveHostState
{
    bool live_mode = false;
    bool is_reload = false;
    bool exit_requested = false;
    bool reload_requested = false;
    bool paused = false;
    float dt = 0.0f;
    float uptime = 0.0f;
    float fps = 0.0f;
    std::string last_error;
    std::unordered_map<std::string, std::vector<uint8_t>> store;
    std::vector<das::SimFunction *> beforeReloadFns;
    std::vector<das::SimFunction *> afterReloadFns;
};

extern DnLiveHostState g_DnLiveHostState;
