#include "duin_editor.h"

#include <Duin.h>
#include <Duin/entrypoint.h>

#include "editor_camera.h"

Duin::Application* Duin::CreateApplication() { return new DuinEditor(); }

Camera3D camera = { 0 };
Camera3D compass_camera =  { 0 };
RenderTexture2D compassRenderTexture;
Rectangle compassTarget = { 25, 25, 300, 300 };

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
    compassRenderTexture = LoadRenderTexture(300, 300);
}

void DuinEditor::HandleInputs()
{
}

void DuinEditor::Update(double rDelta)
{        
    // if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
    //     UpdateCamera(&camera, CAMERA_THIRD_PERSON);
    // }
    EditorCamera_UpdatePosition(&camera, rDelta);
    UpdateCompassCamera(&compass_camera, &camera);
}

void DuinEditor::PhysicsUpdate(double pDelta)
{
}

void DuinEditor::Draw()
{
    DrawGrid(10, 1.0f);
    Vector3 cubePosition = { 0.0f, 0.0f, 0.0f };
    DrawCube(cubePosition, 2.0f, 2.0f, 2.0f, RED);
    DrawCubeWires(cubePosition, 2.0f, 2.0f, 2.0f, MAROON);

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
