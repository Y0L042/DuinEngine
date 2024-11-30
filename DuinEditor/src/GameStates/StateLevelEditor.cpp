#include "StateLevelEditor.h"
#include "States.h"

#include "../Singletons.h"
#include "../Project.h"
#include "../FileManager.h"

#include "../EditorCamera.h"

#include <string>



Camera3D camera = { { 0 } };
Camera3D compass_camera =  { { 0 } };
RenderTexture2D compassRenderTexture;
Rectangle compassTarget = { 25, 25, 300, 300 };

Vector3 cubeSize = { 2.0f, 2.0f, 2.0f };
Vector3 cubePosition = { 0.0f, 0.0f, 0.0f };
Ray selectionRay = { { 0 } };
RayCollision selectionCollision = { 0 };
Model test {};

StateLevelEditor::StateLevelEditor(duin::GameStateMachine& owner)
    : GameState(owner)
{
    Enter = [this]() { State_Enter(); };
    Exit = [this]() { State_Exit(); };
    HandleInput = [this]() { State_HandleInput(); };
    Update = [this](double delta) { State_Update(delta); };
    PhysicsUpdate = [this](double delta) { State_PhysicsUpdate(delta); };
    Draw = [this]() { State_Draw(); };
    DrawUI = [this]() { State_DrawUI(); };
}

StateLevelEditor::~StateLevelEditor()
{}

void StateLevelEditor::State_Enter()
{
    debugConsole.LogEx(duin::LogLevel::Info, "ENTERING LEVELEDITOR");

    fileManager.BuildFileSystemTree();
    fileManager.PrintTree();

    camera.position = { 0.0f, 10.0f, 10.0f };
    camera.target = { 0.0f, 0.0f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;


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
    compass_camera = InitializeCompassCamera();

    test = LoadModel("C:\\Projects\\CPP_Projects\\Duin\\ExampleProjects\\DuinFPS\\assets\\cube.glb");
}

void StateLevelEditor::State_Exit()
{}

void StateLevelEditor::State_HandleInput()
{}

void StateLevelEditor::State_Update(double delta)
{
    HandleInfiniteMouseDragging(MOUSE_BUTTON_RIGHT);

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        UpdateCamera(&camera, CAMERA_THIRD_PERSON);
    }
    UpdateCompassCamera(&compass_camera, &camera);
}

void StateLevelEditor::State_PhysicsUpdate(double delta)
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
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

void StateLevelEditor::State_Draw()
{
    duin::SetActiveCamera3D(camera);

    DrawGrid(100, 1.0f);

    DrawModel(test, {1, 1, 1}, 1, BLACK);

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

void StateLevelEditor::State_DrawUI()
{
    std::string projectFileDir = GetActiveProject().projectDir.string();
    GuiLabel(Rectangle{ 10, 10, 1075, 75 }, projectFileDir.c_str());
    fileManager.DrawGUI();

    DrawCompass3D(compass_camera, compassRenderTexture);
    DrawCompassUI(compassRenderTexture, compassTarget);

    DrawFPS(15, 15);
}


