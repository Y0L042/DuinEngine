



local vendorDeps = {}

-- Utility functions
local function fileExists(path)
    local file = io.open(path, "r")
    if file then file:close() end
    return file ~= nil
end

local function runCommand(command)
    print("Running: " .. command)
    local result = os.execute(command)
    if result ~= 0 then
        print("Warning: Command gave non-zero return: " .. command)
    end
    return result
end

local function printCurrentDir()
    local handle = io.popen("pwd")          
    local current_dir = handle:read("*a")   
    handle:close()                          
    print("Current dir: " .. current_dir)                      
end

-- Cross-platform directory creation function
local function makeDir(path)
    if os.target() == "windows" then
        -- Use Windows-style path separators and command
        return runCommand("mkdir " .. path:gsub("/", "\\"))
    else
        -- On Unix-like systems, use -p to create parent directories as needed
        return runCommand("mkdir -p " .. path)
    end
end

local function deleteFolder(path)
    if os.isdir(path) then  -- Changed: Check if the directory exists
        if os.target() == "windows" then  -- Changed: Use Windows-specific command
            os.execute('rd /s /q "' .. path .. '"')  -- Changed: Remove directory recursively on Windows
        else
            os.execute('rm -rf "' .. path .. '"')  -- Changed: Remove directory recursively on Unix-like systems
        end
        print("Deleted folder: " .. path)
    else
        print("Folder does not exist: " .. path)
    end
end

local function gitCloneOrUpdate(repo, branch, folder)
    if not os.isdir(folder) then
        print("Cloning " .. folder .. "...")
        runCommand("git clone --branch " .. branch .. " --recursive " .. repo .. " " .. folder)
    else
        print("Updating " .. folder .. "...")
        local currentDir = os.getcwd()
        os.chdir(folder)
        runCommand("git pull")
        os.chdir(currentDir)
    end
end

local function fixVsWherePath(batchFilePath)
    if not fileExists(batchFilePath) then
        error("Batch file not found: " .. batchFilePath)
    end

    local inFile = io.open(batchFilePath, "r")
    local content = inFile:read("*all")
    inFile:close()

    -- Replace "\VsWhere.exe" with "/VsWhere.exe"
    local updatedContent = content:gsub("\\VsWhere%.exe", "/VsWhere.exe")

    local outFile = io.open(batchFilePath, "w")
    outFile:write(updatedContent)
    outFile:close()
    print("Updated batch file: Removed '\\' before 'VsWhere.exe'.")
end

local function buildWithMSBuild(solutionPath, config, platform)
    print("Building solution with MSBuild: " .. solutionPath)
    local msbuildPath = "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\MSBuild\\Current\\Bin\\MSBuild.exe"
    if not fileExists(msbuildPath) then
        error("MSBuild.exe not found! Ensure Visual Studio Build Tools are installed.")
    end

    local command = string.format('"%s" "%s" /p:Configuration=%s /p:Platform=%s',
        msbuildPath, solutionPath:gsub("/", "\\"), config, platform)
    runCommand(command)
    print("MSBuild completed for: " .. solutionPath)
end

local function runBatchScript(scriptPath, args)
    local winPath = scriptPath:gsub("/", "\\")
    local command = 'cmd /c "' .. winPath .. '"'
    if args then
        command = command .. " " .. args
    end
    runCommand(command)
end




-- Dependency configuration table
local dependencies = {
    SDL3 = {
        repo = "https://github.com/libsdl-org/SDL",
        tag = "release-3.2.8",
        folder = "sdl",
        steps = function()
            print("Dep SDL3.steps() START")
            print("SDL3 downloaded.")
            runCommand("cd sdl && cmake -S . -B build -DSDL_SHARED=OFF -DSDL_STATIC=ON -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDebug")
            runCommand("cd sdl && cmake --build build")
            print("Dep SDL3.steps() END")
        end
    },

    BX = {
        repo = "https://github.com/bkaradzic/bx",
        branch = "master",
        folder = "bx",
        steps = function()
            print("Dep BX.steps() START")
            print("BX downloaded.")
            print("Dep BX.steps() END")
        end
    },
    BIMG = {
        repo = "https://github.com/bkaradzic/bimg",
        branch = "master",
        folder = "bimg",
        steps = function()
            print("Dep BIMG.steps() START")
            print("BIMG downloaded.")
            print("Dep BIMG.steps() END")
        end
    },
    BGFX = {
        repo = "https://github.com/bkaradzic/bgfx",
        branch = "master",
        commit = "93e6125",
        folder = "bgfx",
        steps = function()
            print("Dep BGFX.steps() START")
            print("BGFX downloaded.")
            runCommand("cd bgfx && git submodule update --init --recursive")
            runCommand("cd bgfx && ..\\bx\\tools\\bin\\windows\\genie vs2022")
            os.execute('cd bgfx\\.build\\projects\\vs2022 && msbuild "bgfx.sln" /p:Configuration=Debug /p:Platform=x64 /p:RuntimeLibrary=MultiThreadedDebug /p:CLCompileAdditionalOptions="/Zc:__cplusplus"')
            print("Dep BGFX.steps() END")
        end
    },

    PHYSX = {
        repo = "https://github.com/NVIDIA-Omniverse/PhysX",
        tag = "106.5-physx-5.5.1-cy",
        folder = "PhysX",
        steps = function()
            print("Dep PHYSX.steps() START")

            -- Update presets for static libraries and runtime
            local presetFile = "PhysX/physx/buildtools/presets/public/vc17win64.xml"
            if fileExists(presetFile) then
                local presetData = io.open(presetFile, "r"):read("*all")
                presetData = presetData:gsub(
                    '<cmakeSwitch name="PX_GENERATE_STATIC_LIBRARIES" value="False"',
                    '<cmakeSwitch name="PX_GENERATE_STATIC_LIBRARIES" value="True"'
                )
                presetData = presetData:gsub(
                    '<cmakeSwitch name="PX_BUILDSNIPPETS" value="True" comment="Generate the snippets" />',
                    '<cmakeSwitch name="PX_BUILDSNIPPETS" value="False" comment="Generate the snippets" />"'
                )

                presetData = presetData:gsub(
                    '<cmakeSwitch name="NV_USE_STATIC_WINCRT" value="False"',
                    '<cmakeSwitch name="NV_USE_STATIC_WINCRT" value="True"'
                )
                local outFile = io.open(presetFile, "w")
                outFile:write(presetData)
                outFile:close()
                print("Updated PhysX presets for static libraries and runtime.")
            end

            -- Generate projects
            local generateProjects = "PhysX/physx/generate_projects.bat"
            fixVsWherePath(generateProjects)
            runBatchScript(generateProjects, "vc17win64")
            runCommand("cd PhysX/physx/buildtools && python3 cmake_generate_projects.py 1")


            -- Build projects automatically (if applicable)
            deleteFolder("PhysX/physx/bin")
            if os.isdir("PhysX/physx/bin") == false then
                runCommand("cd PhysX/physx && cmake -S compiler/public -B compiler/vc17win64")
                -- runCommand("cd PhysX/physx && cmake --build compiler/vc17win64 --config Release")
                runCommand("cd PhysX/physx && cmake --build compiler/vc17win64 --config Debug")
                printCurrentDir()
                -- os.execute('cd PhysX/physx && msbuild "compiler/vc17win64/PhysXSDK.sln" /p:Configuration=Debug /p:Platform=x64')
                -- os.execute('cd PhysX/physx && msbuild "compiler/vc17win64/PhysXSDK.sln" /t:PhysX_static_64 /p:Configuration=Debug /p:Platform=x64')
                -- os.execute('msbuild "PhysXSDK.sln" /p:Configuration=Debug /p:Platform=x64')
            end
            print("Dep PHYSX.steps() END")
        end
    },

    RLIMGUI = {
        repo = "https://github.com/Y0L042/rlImGui",
        branch = "master",
        folder = "rlimgui",
        steps = function()
            print("Dep RLIMGUI.steps() START")
            -- RLImGUI only requires cloning for now
            print("RLImGUI downloaded.")
            print("Dep RLIMGUI.steps() END")
        end
    },

    FMT = {
        repo = "https://github.com/fmtlib/fmt",
        tag = "11.0.2",
        folder = "fmt",
        steps = function()
            print("Dep FMT.steps() START")
            print("FMT downloaded.")
            print("Dep FMT.steps() END")
        end
    },

    RAPIDJSON = {
        repo = "https://github.com/Tencent/rapidjson",
        tag = "v1.1.0",
        folder = "rapidjson",
        steps = function()
            print("Dep RAPIDJSON.steps() START")
            print("RapidJSON downloaded.")
            print("Dep RAPIDJSON.steps() END")
        end
    },

    SPDLOG = {
        repo = "https://github.com/gabime/spdlog",
        tag = "v1.15.0",
        folder = "spdlog",
        steps = function()
            print("Dep SPDLOG.steps() START")
            print("Spdlog downloaded.")
            print("Dep SPDLOG.steps() END")
        end
    },

    FLECS = {
        repo = "https://github.com/SanderMertens/flecs",
        tag = "v4.0.5",
        folder = "flecs",
        steps = function()
            print("Dep FLECS.steps() START")
            deleteFolder("flecs/build_vs2022")
            if not os.isdir("flecs/build_vs2022") then
                -- makeDir("flecs/build_vs2022")
                -- runCommand('cmake -S flecs -B flecs/build_vs2022 -DBUILD_SHARED_LIBS=OFF -DCMAKE_CXX_FLAGS="/MT" -DCMAKE_C_FLAGS="/MT"')
                runCommand('cmake -S flecs -B flecs/build_vs2022 -DBUILD_SHARED_LIBS=OFF -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDebug -DCMAKE_C_FLAGS_DEBUG="/MTd" -DCMAKE_CXX_FLAGS_DEBUG="/MTd"')
                -- runCommand("cmake --build flecs/build_vs2022 --config Release")
                runCommand("cmake --build flecs/build_vs2022 --config Debug")
            end
            print("Dep FLECS.steps() END")
        end
    },

    IMGUI = {
        repo = "https://github.com/ocornut/imgui",
        branch = "docking",
        folder = "imgui",
        steps = function()
            print("Dep IMGUI.steps() START")
            print("Dep IMGUI.steps() END")
    end
}

}



-- Automatically register a new option for each dependency so you can trigger a rebuild
for depName, dep in pairs(dependencies) do
    newoption {
        trigger     = depName,
        description = "Rebuild " .. depName .. " dependency (rename existing folder to " .. dep.folder .. "_BAK and re-download)"
    }
end




local function processDependency(depName, depConfig)
    print(string.format("Processing dependency: %s", depName))
    gitCloneOrUpdate(depConfig.repo, depConfig.branch, depConfig.folder)
    if depConfig.steps then
        depConfig.steps()
    end
    print(string.format("Finished processing: %s", depName))
end

local function gitExecute(depName)
    local dep = dependencies[depName]
    print("Executing git " .. depName .. "...")
    if dep.tag ~= nil then
        print("\tTag " .. dep.tag)
        if not os.isdir(dep.folder) then
            print("\t\tClone")
            -- Clone the repository and then checkout the tag:
            os.execute("git clone --recursive " .. dep.repo .. " " .. dep.folder)  -- Changed: Normal clone without branch restrictions
            os.execute("cd " .. dep.folder .. " && git checkout tags/" .. dep.tag .. "")  -- Changed: Checkout the tag into a new branch
        else
            print("\t\tFetch")
            local currentDir = os.getcwd()
            os.chdir(dep.folder)

            os.execute("git stash")
            os.execute("git fetch --all --tags")  -- Changed: Fetch all tags from remote
            os.execute("git checkout tags/" .. dep.tag .. "")  -- Changed: Re-checkout the desired tag

            os.chdir(currentDir)
        end

    elseif dep.commit ~= nil then
        print("\tCommit " .. dep.commit)
        if not os.isdir(dep.folder) then
            print("\t\tClone")
            -- Clone the dep.repository:
            os.execute("git clone --recursive " .. dep.repo .. " " .. dep.folder)  -- Changed: Normal clone without branch restrictions
            os.execute("cd " .. dep.folder .. " && git checkout " .. dep.commit)       -- Changed: Checkout the specific commit
        else
            print("\t\tFetch")
            local currentDir = os.getcwd()
            os.chdir(dep.folder)

            os.execute("git stash")
            os.execute("git fetch --all")  -- Changed: Fetch all commits from remote
            os.execute("git checkout " .. dep.commit)  -- Changed: Re-checkout the specific commit

            os.chdir(currentDir)
        end

    elseif dep.branch ~= nil then
        print("\tBranch " .. dep.branch)
        if not os.isdir(dep.folder) then
            print("\t\tClone")
            -- Clone the specified branch only (single branch) and with submodules:
            os.execute("git clone --branch " .. dep.branch .. " --single-branch --recursive " .. dep.repo .. " " .. dep.folder)  -- Changed: Use --branch and --single-branch
        else
            print("\t\tFetch")
            local currentDir = os.getcwd()
            os.chdir(dep.folder)

            os.execute("git stash")
            os.execute("git fetch origin " .. dep.branch)       -- Changed: Fetch updates for the branch
            os.execute("git checkout " .. dep.branch)           -- Changed: Ensure we are on the specified branch
            os.execute("git pull origin " .. dep.branch)          -- Changed: Pull the latest changes

            os.chdir(currentDir)
        end
    else
        print("No tag/branch/commit found!")
    end
    print("Git execution finished.")
end

-- Internal function to process a single dependency
local function process(depName)
    local dep = dependencies[depName]
    if not dep then
        error("Unknown dependency: " .. depName)
    end

    -- Save the current working directory
    local original_dir = os.getcwd()  -- Added: Save current directory

    -- Change directory to the vendor folder (adjust the path if needed)
    os.chdir("vendor")  -- Added: Change working directory to vendor
    -- If the option for this dependency is given, rebuild it

    local handle = io.popen("pwd")          -- Open a process to run the shell command "pwd" (prints working directory) -- Changed: Added command to open process
    local current_dir = handle:read("*a")   -- Read all output from the process -- Changed: Reading the process output
    handle:close()                          -- Close the process handle -- Changed: Closing the process
    print(current_dir)                      -- Print the current working directory

    if _OPTIONS[depName] then
        print("Dependancy being built: " .. depName .. ", folder: " .. dep.folder)
        gitExecute(depName)
    end

    -- Run any additional build or setup steps
    if dep.steps then
        dep.steps()
    end

 -- Change back to the original directory
    os.chdir(original_dir)  -- Added: Return to the original working directory

end

-- Process all dependencies
function vendorDeps.processAllDependencies()
    local flaggedDeps = {}
    -- Check for any dependency flags
    for depName, _ in pairs(dependencies) do
        if _OPTIONS[depName] then
            table.insert(flaggedDeps, depName)
        end
    end

    if #flaggedDeps > 0 then
        print("Processing only flagged dependencies:")
        for _, depName in ipairs(flaggedDeps) do
            process(depName)
        end
    else
        print("No specific dependency flags provided. Processing all dependencies:")
        for depName, _ in pairs(dependencies) do
            process(depName)
        end
    end
end

-- Main process
function main()
    -- Here you can add command-line argument processing to allow building a specific dependency.
    -- For now, we iterate over all dependencies.
    for name, config in pairs(dependencies) do
        processDependency(name, config)
    end

    -- Optionally, build main project after dependencies are ready:
    -- buildWithMSBuild("Path/To/YourSolution.sln", "Release", "x64")
end

return vendorDeps;
