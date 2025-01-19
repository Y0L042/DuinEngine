function fixVsWherePath(batchFilePath)
    -- Ensure the batch file exists
    if not os.isfile(batchFilePath) then
        error("Batch file not found: " .. batchFilePath)
    end

    -- Read the content of the batch file
    local inFile = io.open(batchFilePath, "r")
    local content = inFile:read("*all")
    inFile:close()

    -- Replace all occurrences of `\VsWhere.exe` with `VsWhere.exe`
    local updatedContent = content:gsub("\\VsWhere%.exe", "/VsWhere.exe")

    -- Write the updated content back to the file
    local outFile = io.open(batchFilePath, "w")
    outFile:write(updatedContent)
    outFile:close()

    print("Batch file updated: Removed '\\' in front of 'VsWhere.exe'.")
end

function buildWithMSBuild(solutionPath, config, platform)
    print("Building solution with MSBuild...")

    -- Locate MSBuild.exe
    local msbuildPath = "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\MSBuild\\Current\\Bin\\MSBuild.exe"
    if not os.isfile(msbuildPath) then
        error("MSBuild.exe not found! Ensure Visual Studio Build Tools are installed.")
    end

    -- Build command
    local command = "\"".. msbuildPath .."\" \"" .. solutionPath:gsub("/", "\\") .. "\""
        .. " /p:Configuration=" .. config
        .. " /p:Platform=" .. platform
    print("Running: " .. command)

    -- Execute the build
    local result = os.execute(command)
    if result ~= 0 then
        error("MSBuild failed for solution: " .. solutionPath)
    end

    print("Build completed successfully!")
end

function runBatchScript(scriptPath, args)
    -- Convert POSIX path to Windows path
    local winPath = scriptPath:gsub("/", "\\")
    local command = 'cmd /c "' .. winPath .. '"'
    -- local command = scriptPath
    if args then
        command = command .. " " .. args
    end

    print("Running batch script: " .. command)
    local result = os.execute(command)
end


function PhysX()
    print("Vendor Build START: PhysX")

    -- Download
    if not os.isdir("PhysX") then
        print("Cloning START")
        os.execute("git clone --recursive https://github.com/NVIDIA-Omniverse/PhysX PhysX")
        print("Cloning END")

    -- Generate CMake projects
    if not os.isdir("PhysX/physx/compiler") then
        print("Generation START")
        
        local generateProjects = "PhysX/physx/generate_projects.bat"
        fixVsWherePath(generateProjects)
        -- Ensure Visual Studio tools are available
        -- Run PhysX generate_projects script
        runBatchScript(generateProjects, "vc17win64")
        os.execute("cd PhysX/physx/buildtools && python3 cmake_generate_projects.py 1")

        print("Generation END")
    end

    printf("Preset Update START")
    local presetFile = "PhysX/physx/buildtools/presets/public/vc17win64.xml"
    local presetData = io.open(presetFile, "r"):read("*all")

    -- Enable static libraries and static runtime
    presetData = presetData:gsub(
        '<cmakeSwitch name="PX_GENERATE_STATIC_LIBRARIES" value="False"',
        '<cmakeSwitch name="PX_GENERATE_STATIC_LIBRARIES" value="True"'
    )
    presetData = presetData:gsub(
        '<cmakeSwitch name="NV_USE_STATIC_WINCRT" value="False"',
        '<cmakeSwitch name="NV_USE_STATIC_WINCRT" value="True"'
    )

    -- Write updated data back to the file
    local presetFileOut = io.open(presetFile, "w")
    presetFileOut:write(presetData)
    presetFileOut:close()
    print("Updated PhysX presets for static libraries and static runtime.")
    printf("Preset Update END")


    if not os.isdir("PhysX/physx/bin") then
        print("Building START")
        os.execute("cd PhysX/physx && cmake --build compiler/public --config Release")
        os.execute("cd PhysX/physx && cmake --build compiler/public --config Debug")
        print("Building END")
    end

    -- if os.isdir("PhysX/physx/compiler/vc17win64") then
    --     buildPath = "PhysX/physx/compiler/vc17win64"
    --     solutionPath = buildPath .. "/PhysX.sln"
    --     buildWithMSBuild(solutionPath, "Release", "x64")
    --     buildWithMSBuild(solutionPath, "Debug", "x64")
    -- end
    
    print("Vendor Build END: PhysX")
    end
end

function RLImGUI()
    print("Vendor DL START: RLImGUI")

    -- Download
    if not os.isdir("rlimgui") then
        print("Cloning START")
        os.execute("git clone --recursive https://github.com/Y0L042/rlImGui")
        print("Cloning END")
    end

    print("Vendor DL END: RLImGUI")
end

function FMT()
    print("Vendor DL START: FMT")
    -- Download
    if not os.isdir("fmt") then
        print("Cloning START")
        os.execute("git clone --branch 11.0.2 https://github.com/fmtlib/fmt")
        print("Cloning END")
    end
    print("Vendor DL END: FMT")
end

function RapidJSON()
    print("Vendor DL START: RapidJSON")
    -- Download
    if not os.isdir("rapidjson") then
        print("Cloning START")
        os.execute("git clone --branch v1.1.0 https://github.com/Tencent/rapidjson")
        print("Cloning END")
    end
    print("Vendor DL END: RapidJSON")
end

function Spdlog()
    print("Vendor DL START: Spdlog")
    -- Download
    if not os.isdir("spdlog") then
        print("Cloning START")
        os.execute("git clone --branch v1.15.0 https://github.com/gabime/spdlog")
        print("Cloning END")
    end
    print("Vendor DL END: Spdlog")
end

function Flecs()
    print("Vendor Build START: Flecs")

    -- Download
    if not os.isdir("flecs") then
        print("Cloning START")
        os.execute("git clone --branch v4.0.3 https://github.com/SanderMertens/flecs")
        print("Cloning END")
    end

    -- Build Flecs
    if not os.isdir("flecs/build") then
        print("Building START")

        os.execute("mkdir flecs/build")
        os.execute('cmake -S flecs -B flecs/build -DBUILD_SHARED_LIBS=OFF -DCMAKE_CXX_FLAGS="/MT" -DCMAKE_C_FLAGS="/MT"')
        os.execute("cmake --build flecs/build --config Release")
        os.execute("cmake --build flecs/build --config Debug")

        print("Building END")
    end

    print("Vendor Build END: Flecs")
end

function main()
    -- PhysX()
    -- RLImGUI()
    -- FMT()
    -- RapidJSON()
    -- Spdlog()
    -- Flecs()
end

main()
