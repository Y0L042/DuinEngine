#include "duin_editor.h"

#include <duin.h>
#include <Duin/entrypoint.h>

#include "editor_camera.h"
#include "file_browser.h"

#define MAX_DIR_LEN 512

Duin::Application* Duin::CreateApplication() { return new DuinEditor(); }

const char *EDITOR_CFG_PATH = "./data/editor_cfg.json\0";

Camera3D camera = { { 0 } };
Camera3D compass_camera =  { { 0 } };
RenderTexture2D compassRenderTexture;
Rectangle compassTarget = { 25, 25, 300, 300 };



Vector3 cubeSize = { 2.0f, 2.0f, 2.0f };
Vector3 cubePosition = { 0.0f, 0.0f, 0.0f };
Ray selectionRay = { { 0 } };
RayCollision selectionCollision = { 0 };
Model test {};

Duin::DebugConsole dbgConsole;

FileExplorer explorer;



static void LoadCurrentProject();


void DuinEditor::Initialize()
{
    SetWindowName("Duin Editor");
    SetFramerate(60);
    SetWindowStartupSize(1600, 900);

    camera.position = { 0.0f, 10.0f, 10.0f };
    camera.target = { 0.0f, 0.0f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    SetActiveCamera3D(&camera);

    compass_camera = InitializeCompassCamera();

    LoadCurrentProject();

}



void DuinEditor::Ready()
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    int compassTargetSide = (int)(screenHeight * 0.175f);
    int compassTargetPosX = (int)((screenWidth - compassTargetSide) * 0.95f);
    int compassTargetPosY = (int)((screenHeight - compassTargetSide) * 0.05f);
    compassTarget = {
        (float)compassTargetPosX,
        (float)compassTargetPosY,
        (float)compassTargetSide,
        (float)compassTargetSide,
    };
    compassRenderTexture = LoadRenderTexture(600, 600);

    test = LoadModel("C:\\Projects\\CPP_Projects\\Duin\\ExampleProjects\\DuinFPS\\assets\\cube.glb");
}



void DuinEditor::HandleInputs()
{
}



void DuinEditor::Update(double rDelta)
{        
    HandleInfiniteMouseDragging(MOUSE_BUTTON_RIGHT);

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        UpdateCamera(&camera, CAMERA_THIRD_PERSON);
    }
    UpdateCompassCamera(&compass_camera, &camera);
}



void DuinEditor::PhysicsUpdate(double pDelta)
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        dbgConsole.LogEx(Duin::LogLevel::Info, "Left Click!");
        if (!selectionCollision.hit) {
            selectionRay = GetScreenToWorldRay(GetMousePosition(), camera);

            // Check collision between ray and box
            selectionCollision = GetRayCollisionBox(selectionRay,
                        {
                            { 
                                cubePosition.x - cubeSize.x/2, 
                                cubePosition.y - cubeSize.y/2, 
                                cubePosition.z - cubeSize.z/2 
                            },
                            { 
                                cubePosition.x + cubeSize.x/2, 
                                cubePosition.y + cubeSize.y/2, 
                                cubePosition.z + cubeSize.z/2 
                            }
                        });
        }
        else selectionCollision.hit = false;
    }
}



void DuinEditor::Draw()
{
    DrawGrid(100, 1.0f);

    DrawModel(test, {1, 1, 1}, 1, WHITE);

    if (selectionCollision.hit)
    {
        DrawCube(cubePosition, cubeSize.x, cubeSize.y, cubeSize.z, RED);
        DrawCubeWires(cubePosition, cubeSize.x, cubeSize.y, cubeSize.z, MAROON);

        DrawCubeWires(
            cubePosition, 
            cubeSize.x + 0.2f, 
            cubeSize.y + 0.2f, 
            cubeSize.z + 0.2f, 
            GREEN);
    }
    else
    {
        DrawCube(cubePosition, cubeSize.x, cubeSize.y, cubeSize.z, GRAY);
        DrawCubeWires(
            cubePosition, 
            cubeSize.x, 
            cubeSize.y, 
            cubeSize.z, 
            DARKGRAY);
    }

    float len = 10.0f;
    DrawLine3D({ 0.0f, 0.0f, 0.0f }, { len, 0.0f, 0.0f }, RED);
    DrawLine3D({ 0.0f, 0.0f, 0.0f }, { 0.0f, len, 0.0f }, GREEN);
    DrawLine3D({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, len }, BLUE);
}



void DuinEditor::DrawUI()
{                
    dbgConsole.Draw("Console");

    GuiLabel(Rectangle{ 10, 10, 75, 75 }, "TEST");

    DrawCompass3D(compass_camera, compassRenderTexture);
    DrawCompassUI(compassRenderTexture, compassTarget);

    DrawFPS(15, 15);
}


static void LoadCurrentProject()
{
    rapidjson::Document doc;
    if (ReadJSONFile(EDITOR_CFG_PATH, &doc)) {
        DN_WARN("Config empty. Select project root.\n");
        return;
    }

    // Ensure "EditorConfig" exists and is an object
    if (!doc.HasMember("EditorConfig") || !doc["EditorConfig"].IsObject()) {
        DN_WARN("\"EditorConfig\" not found. Select project root.\n");
        return;
    }

    const rapidjson::Value& editorConfig = doc["EditorConfig"];

    // Ensure "CurrentProjectDir" exists and is a string
    if (!editorConfig.HasMember("CurrentProjectDir") 
        || !editorConfig["CurrentProjectDir"].IsString()) {
        DN_WARN("\"CurrentProjectDir\" not found. Select project root.\n");
        dbgConsole.Log("Directory not found!");
        return;
    }

    std::string currentProjectDir = 
        editorConfig["CurrentProjectDir"].GetString();

    if (!currentProjectDir.empty()) {
        fs::path projectPath = currentProjectDir;
        if (fs::exists(projectPath) && fs::is_directory(projectPath)) {
            DN_INFO("Loaded Project: {0}", projectPath.string().c_str());
            dbgConsole.Log("Project loadied: %s", projectPath.string().c_str());
            dbgConsole.LogEx(Duin::LogLevel::Info, "Project loded: %s", projectPath.string().c_str());
        } else {
            DN_WARN("Project directory invalid. Select project root.\n");
        }
    } else {
        DN_WARN("CurrentProjectDir is empty. Select project root.\n");
    }
}

