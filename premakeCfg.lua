-- premakeCfg.lua — build-system configuration singleton
-- Require this file from any .lua in the build system; Lua's module cache
-- guarantees it is evaluated only once regardless of how many files require it.
--
--   local Cfg = require "premakeCfg"
--   print(Cfg.VISUAL_STUDIO)   --> "VS2026"

local Cfg = {}

Cfg.VISUAL_STUDIO = "VS2026"
Cfg.CRT          = "MD"

Cfg.cmake_crt_debug   = (Cfg.CRT == "MT") and "MultiThreadedDebug"     or "MultiThreadedDebugDLL"
Cfg.cmake_crt_release = (Cfg.CRT == "MT") and "MultiThreaded"          or "MultiThreadedDLL"
Cfg.premake_staticrt  = (Cfg.CRT == "MT") and "On"                     or "Off"

return Cfg
