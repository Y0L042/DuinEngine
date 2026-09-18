local Cfg = require "premakeCfg"
local utils = require "utils"
local dep_daslang = {}
local name = "DASLANG"

local repo   = "https://github.com/GaijinEntertainment/daScript"
local commit    = "v0.6.4" -- "6e38cb3" -- "v0.6.2-RC3"
local checkout = commit
local folder = "daslang"

-- Source for lld-link.exe. Must be an MSVC-targeting LLVM (x86_64-pc-windows-msvc)
-- with bin/lld-link.exe + bin/LLVM-C.dll; the MSYS2 clang64 mingw layout is the one
-- daslang explicitly rejects in modules/dasLLVM/CMakeLists.txt.
local llvm_bin = "C:/Programs/LLVM/bin"

function dep_daslang.build()
    print("START: " .. name)

    if not os.isdir(folder) then
        print("\t\tClone")
        utils.runCommand("git clone --recursive " .. repo .. " " .. folder)
        utils.runCommand("cd " .. folder .. " && git checkout " .. checkout)
    else
        print("\t\tFetch")
        utils.pushDir(folder)
        utils.runCommand("git stash")
        utils.runCommand("git pull")
        utils.runCommand("git checkout " .. checkout)
        utils.popDir()
    end
    print(name .. " downloaded.")

    utils.pushDir(folder)

    local build_type = "Debug"

    local cmake_flags = "-DCMAKE_BUILD_TYPE=" .. build_type
        .. " -DDAS_TUTORIAL_DISABLED=ON"
        .. " -DDAS_TESTS_DISABLED=ON"
        .. " -DDAS_AOT_EXAMPLES_DISABLED=ON"
        .. " -DDAS_GLFW_DISABLED=ON"
        .. " -DDAS_IMGUI_DISABLED=OFF"
        .. " -DDAS_CLANG_BIND_DISABLED=ON"
        .. " -DDAS_LLVM_DISABLED=OFF"
        .. " -DDAS_HV_DISABLED=OFF"
        .. " -DDAS_SQLITE_DISABLED=OFF"
        .. " -DCMAKE_TOOLCHAIN_FILE=D:/Projects/_Tools/vcpkg/scripts/buildsystems/vcpkg.cmake"
        .. " -DVCPKG_TARGET_TRIPLET=x64-windows"
        .. " -DOPENSSL_ROOT_DIR=D:/Projects/_Tools/vcpkg/installed/x64-windows"
        .. " -DOPENSSL_INCLUDE_DIR=D:/Projects/_Tools/vcpkg/installed/x64-windows/include"

    cmake_flags = cmake_flags .. " -DCMAKE_CXX_FLAGS_DEBUG=\"/DDAS_SMART_PTR_DEBUG=1 /DDAS_ENABLE_EXCEPTIONS=1\""

    if os.target() == "windows" then
        cmake_flags = cmake_flags .. " -DCMAKE_MSVC_RUNTIME_LIBRARY=" .. Cfg.cmake_crt_debug
        -- daslang's CMakeCommon.txt branches on WIN32, not on the compiler, and emits
        -- MSVC-only flags (/HEAP, /arch:AVX2, /Z7, /std:c++17). The vcpkg x64-windows
        -- triplet also supplies MSVC .lib import libraries. A MinGW gcc picked up from
        -- PATH -- MSYS2's cmake.exe defaults to Ninja and finds it first -- dies at the
        -- libhv compiler check with "cannot find /HEAP:536870912".
        cmake_flags = ' -G "' .. Cfg.cmake_generator .. '" -A ' .. Cfg.cmake_arch
            .. " " .. cmake_flags
    end

    -- cmd.exe strips the outer quote pair when a command STARTS with a quote, then
    -- re-parses -- splitting the line at the -G "Visual Studio ..." quotes. Wrapping
    -- the whole command in one more pair survives that, for spaced paths too.
    local function cmake_cmd(args)
        return '""' .. Cfg.cmake_exe .. '" ' .. args .. '"'
    end

    print("\t\tConfiguring with: " .. cmake_flags)
    utils.runCommand(cmake_cmd("-S . -B build " .. cmake_flags))

    -- daslang compiles its utility exes with its own `daslang -exe`, whose JIT backend
    -- shells out to lld-link.exe. module_jit.cpp find_linker probes
    -- <das_root>/bin/lld-link.exe (FLAT bin/, not bin/<Config>/ -- getDasRoot strips the
    -- config subdir) then falls back to PATH. daslang's own CMake would copy it, but
    -- modules/dasLLVM/CMakeLists.txt gates that on a sentinel keyed to lib/LLVM.dll
    -- alone -- with LLVM.dll already present the copy never runs. Stage it ourselves.
    if os.target() == "windows" and not utils.fileExists("bin/lld-link.exe") then
        local src = llvm_bin .. "/lld-link.exe"
        if not utils.fileExists(src) then
            error("dep_daslang: lld-link.exe not found at " .. llvm_bin ..
                  ". Install LLVM for Windows or adjust llvm_bin.")
        end
        if not os.isdir("bin") then
            os.mkdir("bin")
        end
        -- os.copyfile over utils.copyFiles: the latter's xcopy destination ends in
        -- a quoted trailing backslash, which xcopy reads as part of the path and
        -- rejects with "Invalid path" -- silently, since it redirects to nul.
        local copied, cperr = os.copyfile(src, "bin/lld-link.exe")
        if not copied then
            error("dep_daslang: failed to copy lld-link.exe: " .. tostring(cperr))
        end
        print("\t\tStaged lld-link.exe from " .. llvm_bin)
    end

    local ok = utils.runCommand(cmake_cmd("--build build --config " .. build_type .. " -j 1")) -- -j 1 avoids build concurrency errors

    if ok then
        utils.deleteFolder("build")
    else
        print("dep_daslang: build FAILED - leaving build/ for diagnosis.")
    end

    utils.popDir()
    print("END: " .. name)
end

return dep_daslang
