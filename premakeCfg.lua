-- premakeCfg.lua — build-system configuration singleton
-- Require this file from any .lua in the build system; Lua's module cache
-- guarantees it is evaluated only once regardless of how many files require it.
--
--   local Cfg = require "premakeCfg"
--   print(Cfg.VISUAL_STUDIO)   --> "VS2026"

local Cfg = {}

Cfg.VISUAL_STUDIO = "VS2026"
Cfg.CRT          = "MD"

-- CMake generator matching Cfg.VISUAL_STUDIO. Must be passed explicitly (-G):
-- when premake is run from an MSYS2 shell, CMake otherwise defaults to Ninja
-- and picks up MSYS2's gcc instead of MSVC.
Cfg.cmake_generator = ({
    VS2026 = "Visual Studio 18 2026",
    VS2022 = "Visual Studio 17 2022",
    VS2019 = "Visual Studio 16 2019",
})[Cfg.VISUAL_STUDIO] or error("premakeCfg: no cmake generator mapped for " .. tostring(Cfg.VISUAL_STUDIO))
Cfg.cmake_arch = "x64"

Cfg.cmake_crt_debug   = (Cfg.CRT == "MT") and "MultiThreadedDebug"     or "MultiThreadedDebugDLL"
Cfg.cmake_crt_release = (Cfg.CRT == "MT") and "MultiThreaded"          or "MultiThreadedDLL"
Cfg.premake_staticrt  = (Cfg.CRT == "MT") and "On"                     or "Off"

return Cfg
