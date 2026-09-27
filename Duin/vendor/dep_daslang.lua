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
-- daslang explicitly rejects in modules/dasLLVM/CMakeLists.txt. Resolved in premakeCfg.
local llvm_bin = Cfg.llvm_bin

function dep_daslang.build()
    print("START: " .. name)
    local fetched = true

    if not os.isdir(folder) then
        print("\t\tClone")
        fetched = utils.runCommand("git clone --recursive " .. repo .. " " .. folder) and fetched
        fetched = utils.runCommand("cd " .. folder .. " && git checkout " .. checkout) and fetched
    else
        print("\t\tFetch")
        utils.pushDir(folder)
        utils.runCommand("git stash")
        utils.runCommand("git pull")
        fetched = utils.runCommand("git checkout " .. checkout) and fetched
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

    -- dasHV (needed by DuinDasHost's libhv) links OpenSSL. vcpkg is only used for a
    -- prebuilt one: classic-mode vcpkg has it under installed/x64-windows; VS's bundled
    -- vcpkg is manifest-only and never does.
    local build_path_prefix = nil
    local vcpkg = Cfg.vcpkg_root
    local vcpkg_openssl = vcpkg and (vcpkg .. "/installed/x64-windows")
    if vcpkg_openssl and os.isdir(vcpkg_openssl .. "/include/openssl") then
        print("\t\tUsing vcpkg OpenSSL at " .. vcpkg_openssl)
        cmake_flags = cmake_flags
            .. ' -DCMAKE_TOOLCHAIN_FILE="' .. vcpkg .. '/scripts/buildsystems/vcpkg.cmake"'
            .. " -DVCPKG_TARGET_TRIPLET=x64-windows"
            .. ' -DOPENSSL_ROOT_DIR="' .. vcpkg_openssl .. '"'
            .. ' -DOPENSSL_INCLUDE_DIR="' .. vcpkg_openssl .. '/include"'
    else
        -- Otherwise dasHV builds OpenSSL 3.5.1 from source into OPENSSL_ROOT_DIR. Point it
        -- outside build/ (deleted after a successful build) so it is built only once.
        local openssl = os.getenv("DASLANG_OPENSSL_DIR")
        openssl = (openssl and openssl ~= "") and path.translate(openssl, "/") or path.getabsolute("_openssl")
        cmake_flags = cmake_flags .. ' -DOPENSSL_ROOT_DIR="' .. openssl .. '"'

        if os.isfile(openssl .. "/lib/libcrypto.lib") then
            print("\t\tUsing previously built OpenSSL at " .. openssl)
        else
            -- `perl Configure` runs inside the MSBuild step and resolves perl from PATH,
            -- where Git Bash's MSYS perl fails. Fail now rather than mid-build.
            if not Cfg.perl_bin then
                utils.popDir()
                error("dep_daslang: OpenSSL must be built from source, which needs a native Windows perl.\n" ..
                      "  Install Strawberry Perl (winget install StrawberryPerl.StrawberryPerl) or set PERL_BIN.\n" ..
                      "  Alternatively set DASLANG_OPENSSL_DIR to an existing OpenSSL install.")
            end
            print("\t\tBuilding OpenSSL from source into " .. openssl .. " (perl: " .. Cfg.perl_bin .. ")")
            build_path_prefix = path.translate(Cfg.perl_bin, "\\")
            if Cfg.nasm_bin then
                build_path_prefix = build_path_prefix .. ";" .. path.translate(Cfg.nasm_bin, "\\")
            else
                print("\t\tWarning: NASM not found - OpenSSL Configure may fail. Install NASM or set NASM_BIN.")
            end
        end
    end

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
    -- the whole command in one more pair survives that, for spaced paths too. With a
    -- PATH prefix the line starts with `set`, so no stripping happens and no wrap is needed.
    local function cmake_cmd(args)
        local cmd = '"' .. Cfg.cmake_exe .. '" ' .. args
        if build_path_prefix then
            return 'set "PATH=' .. build_path_prefix .. ';%PATH%" && ' .. cmd
        end
        return '"' .. cmd .. '"'
    end

    -- A leftover build/ (failed run, or copied from another machine) keeps cached values
    -- like CMAKE_TOOLCHAIN_FILE that the flags above no longer set. Drop the cache so
    -- configure starts clean, while keeping already-built objects (e.g. OpenSSL).
    if os.isfile("build/CMakeCache.txt") then
        os.remove("build/CMakeCache.txt")
        os.rmdir("build/CMakeFiles")
    end

    print("\t\tConfiguring with: " .. cmake_flags)
    if not utils.runCommand(cmake_cmd("-S . -B build " .. cmake_flags)) then
        utils.popDir()
        error("dep_daslang: CMake configure failed - see output above.")
    end

    -- daslang compiles its utility exes with its own `daslang -exe`, whose JIT backend
    -- shells out to lld-link.exe. module_jit.cpp find_linker probes
    -- <das_root>/bin/lld-link.exe (FLAT bin/, not bin/<Config>/ -- getDasRoot strips the
    -- config subdir) then falls back to PATH. daslang's own CMake would copy it, but
    -- modules/dasLLVM/CMakeLists.txt gates that on a sentinel keyed to lib/LLVM.dll
    -- alone -- with LLVM.dll already present the copy never runs. Stage it ourselves.
    if os.target() == "windows" and not utils.fileExists("bin/lld-link.exe") then
        if not llvm_bin then
            error("dep_daslang: MSVC-targeting LLVM (lld-link.exe + LLVM-C.dll) not found. " ..
                  "Install LLVM for Windows or set LLVM_ROOT to its install folder.")
        end
        local src = llvm_bin .. "/lld-link.exe"
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
    utils.reportBuildStatus(name, ok and fetched)
    print("END: " .. name)
end

return dep_daslang
