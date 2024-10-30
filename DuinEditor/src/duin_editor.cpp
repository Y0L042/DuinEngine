#include "duin_editor.h"

#include <duin.h>
#include <Duin/entrypoint.h>

#include "editor_camera.h"

Duin::Application* Duin::CreateApplication() { return new DuinEditor(); }

Vector3 cubeSize = { 2.0f, 2.0f, 2.0f };
Vector3 cubePosition = { 0.0f, 0.0f, 0.0f };

Camera3D camera = { 0 };
Camera3D compass_camera =  { 0 };
RenderTexture2D compassRenderTexture;
Rectangle compassTarget = { 25, 25, 300, 300 };
Ray selectionRay = { 0 };
RayCollision selectionCollision = { 0 };

void DuinEditor::Initialize()
{
    SetWindowName("Duin Editor");

    camera.position = { 0.0f, 10.0f, 10.0f };
    camera.target = { 0.0f, 0.0f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    SetActiveCamera3D(&camera);

    compass_camera = InitializeCompassCamera();
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

    int isMouseOnCompassArea = CheckCollisionPointRec(
        GetMousePosition(), compassTarget);

    UpdateCompassCamera(&compass_camera, &camera);
}

void DuinEditor::PhysicsUpdate(double pDelta)
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (!selectionCollision.hit) {
            selectionRay = GetScreenToWorldRay(GetMousePosition(), camera);

            // Check collision between ray and box
            selectionCollision = GetRayCollisionBox(selectionRay,
                        {{ cubePosition.x - cubeSize.x/2, cubePosition.y - cubeSize.y/2, cubePosition.z - cubeSize.z/2 },
                                      { cubePosition.x + cubeSize.x/2, cubePosition.y + cubeSize.y/2, cubePosition.z + cubeSize.z/2 }});
        }
        else selectionCollision.hit = false;
    }
}

void DuinEditor::Draw()
{
    DrawGrid(10, 1.0f);

    if (selectionCollision.hit)
    {
        DrawCube(cubePosition, cubeSize.x, cubeSize.y, cubeSize.z, RED);
        DrawCubeWires(cubePosition, cubeSize.x, cubeSize.y, cubeSize.z, MAROON);

        DrawCubeWires(cubePosition, cubeSize.x + 0.2f, cubeSize.y + 0.2f, cubeSize.z + 0.2f, GREEN);
    }
    else
    {
        DrawCube(cubePosition, cubeSize.x, cubeSize.y, cubeSize.z, GRAY);
        DrawCubeWires(cubePosition, cubeSize.x, cubeSize.y, cubeSize.z, DARKGRAY);
    }

    float len = 10.0f;
    DrawLine3D({ 0.0f, 0.0f, 0.0f }, { len, 0.0f, 0.0f }, RED);
    DrawLine3D({ 0.0f, 0.0f, 0.0f }, { 0.0f, len, 0.0f }, GREEN);
    DrawLine3D({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, len }, BLUE);
}

void DuinEditor::DrawUI()
{                
    DrawCompass3D(compass_camera, compassRenderTexture);
    DrawCompassUI(compassRenderTexture, compassTarget);
    DrawFPS(15, 15);
}

