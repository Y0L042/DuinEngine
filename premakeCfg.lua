-- premakeCfg.lua — build-system configuration singleton
-- Require this file from any .lua in the build system; Lua's module cache
-- guarantees it is evaluated only once regardless of how many files require it.
--
--   local Cfg = require "premakeCfg"
--   print(Cfg.VISUAL_STUDIO)   --> "VS2026"

local Cfg = {}

Cfg.VISUAL_STUDIO = "VS2026"
Cfg.CRT          = "MT"

return Cfg
