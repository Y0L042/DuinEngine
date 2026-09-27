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

-- Absolute path to the native Windows cmake. Resolved explicitly rather than by
-- PATH lookup for two reasons: premake may be launched from a shell that has no
-- cmake at all, and an MSYS2 shell puts C:/msys64/ucrt64/bin/cmake.exe first,
-- which defaults to Ninja + MinGW gcc. Falls back to bare "cmake" if absent.
-- Tool locations differ per machine, so each is resolved from (in order) an env
-- var, PATH, then common install locations. Returns a forward-slashed path or nil.
local function norm(p)
    return (path.translate(p, "/"):gsub("/+$", ""))
end

local function has_all(dir, files)
    for _, f in ipairs(files) do
        if not os.isfile(dir .. "/" .. f) then return false end
    end
    return true
end

-- env_var: root dir override; subdir: appended to it; files: must all exist in the dir.
local function find_dir(env_var, subdir, files, candidates, search_path)
    local env = os.getenv(env_var)
    if env and env ~= "" then
        local dir = norm(env) .. (subdir or "")
        if has_all(dir, files) then return dir end
        print("premakeCfg: " .. env_var .. "=" .. env .. " does not contain " .. table.concat(files, ", "))
    end
    if search_path then
        local on_path = os.pathsearch(files[1], os.getenv("PATH"))
        if on_path and has_all(norm(on_path), files) then return norm(on_path) end
    end
    for _, dir in ipairs(candidates) do
        if has_all(dir, files) then return dir end
    end
    return nil
end

-- Latest Visual Studio install dir (via vswhere), used as a fallback location for
-- the cmake / vcpkg / LLVM that VS bundles. Outer quote pair: see cmake_cmd note.
Cfg.vs_path = nil
local vswhere = "C:/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe"
if os.isfile(vswhere) then
    local out = os.outputof('""' .. vswhere .. '" -latest -prerelease -products * -property installationPath"')
    if out and out ~= "" then
        Cfg.vs_path = norm(out:match("[^\r\n]+"))
    end
end
local vs = Cfg.vs_path or "<no-vs>"

-- PATH deliberately not searched for cmake: see the MSYS2 note above.
local cmake_bin = find_dir("CMAKE_ROOT", "/bin", { "cmake.exe" }, {
    "C:/Program Files/CMake/bin",
    "C:/Programs/CMake/bin",
    vs .. "/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin",
}, false)
Cfg.cmake_exe = cmake_bin and (cmake_bin .. "/cmake.exe") or "cmake"

-- MSBuild for genie-generated solutions (bgfx). Only on PATH inside a VS developer
-- prompt, so fall back to the VS install. Set MSBUILD_BIN to the dir holding MSBuild.exe to override.
local msbuild_bin = find_dir("MSBUILD_BIN", nil, { "MSBuild.exe" }, {
    vs .. "/MSBuild/Current/Bin/amd64",
    vs .. "/MSBuild/Current/Bin",
}, true)
Cfg.msbuild_exe = msbuild_bin and (msbuild_bin .. "/MSBuild.exe") or "msbuild"

-- vcpkg root (folder holding vcpkg.exe and scripts/). Set VCPKG_ROOT to override.
Cfg.vcpkg_root = find_dir("VCPKG_ROOT", nil, { "vcpkg.exe", "scripts/buildsystems/vcpkg.cmake" }, {
    "C:/vcpkg",
    "C:/Programs/vcpkg",
    "D:/Projects/_Tools/vcpkg",
    vs .. "/VC/vcpkg",
}, true)

-- MSVC-targeting LLVM bin/ (lld-link.exe + LLVM-C.dll). Requiring LLVM-C.dll rejects
-- an MSYS2 clang64 lld-link that may be on PATH. Set LLVM_ROOT to override.
Cfg.llvm_bin = find_dir("LLVM_ROOT", "/bin", { "lld-link.exe", "LLVM-C.dll" }, {
    "C:/Program Files/LLVM/bin",
    "C:/Programs/LLVM/bin",
    vs .. "/VC/Tools/Llvm/x64/bin",
}, true)

-- Native Windows perl for OpenSSL's `perl Configure` (daslang dasHV source build).
-- PATH is not searched: Git Bash/MSYS2 put an MSYS perl there that lacks core modules
-- (IPC::Cmd) and fails Configure. Set PERL_BIN to the dir holding perl.exe to override.
Cfg.perl_bin = find_dir("PERL_BIN", nil, { "perl.exe" }, {
    "C:/Strawberry/perl/bin",
    "C:/Programs/Strawberry/perl/bin",
    "C:/Perl64/bin",
}, false)

-- NASM for OpenSSL's assembly; optional. Set NASM_BIN to override.
Cfg.nasm_bin = find_dir("NASM_BIN", nil, { "nasm.exe" }, {
    "C:/Program Files/NASM",
    "C:/Programs/NASM",
}, true)

print("premakeCfg: cmake=" .. Cfg.cmake_exe
    .. "  msbuild=" .. Cfg.msbuild_exe
    .. "  vcpkg=" .. tostring(Cfg.vcpkg_root)
    .. "  llvm=" .. tostring(Cfg.llvm_bin)
    .. "  perl=" .. tostring(Cfg.perl_bin)
    .. "  nasm=" .. tostring(Cfg.nasm_bin))

Cfg.cmake_crt_debug   = (Cfg.CRT == "MT") and "MultiThreadedDebug"     or "MultiThreadedDebugDLL"
Cfg.cmake_crt_release = (Cfg.CRT == "MT") and "MultiThreaded"          or "MultiThreadedDLL"
Cfg.premake_staticrt  = (Cfg.CRT == "MT") and "On"                     or "Off"

return Cfg
