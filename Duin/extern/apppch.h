#pragma once

// --- Standard Library ---
#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <chrono>
#include <variant>
#include <cstdint>
#include <any>
#include <vector>
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <string>

// --- SDL ---
#include <SDL3/SDL.h>

// --- bgfx ---
#define BGFX_NOPIC
#include <bgfx/bgfx.h>

// --- ImGui ---
#include <external/imgui.h>

// --- JSON / TOML ---
#include <rapidjson/rapidjson.h>
#include <toml.hpp>
#include <rfl.hpp>

// --- flecs ECS ---
#include <flecs.h>

// --- spdlog ---
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <spdlog/common.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/bundled/core.h>
#include <spdlog/fmt/bundled/format.h>
#include <spdlog/details/registry.h>
#include <spdlog/logger.h>

// --- PhysX ---
#ifndef JOLT_PHYSICS
#ifndef PX_PHYSX_STATIC_LIB
#define PX_PHYSX_STATIC_LIB
#endif
//#include <PxPhysicsAPI.h>
//#include <PxPhysics.h>
//#include <characterkinematic/PxController.h>
//#include <characterkinematic/PxControllerManager.h>
#endif // JOLT_PHYSICS

#include <Jolt/Jolt.h>

#ifndef BX_CONFIG_DEBUG
#define BX_CONFIG_DEBUG 0
#endif
