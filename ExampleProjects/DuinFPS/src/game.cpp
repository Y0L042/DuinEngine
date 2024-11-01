#include "game.h"

#include <Duin.h>
#include <Duin/entrypoint.h>

Duin::Application* Duin::CreateApplication() { return new Game(); }


int fpsCameraEnabled = 0;
Camera3D fps_camera = { 0 };



void InitializeCamera(Camera3D *camera);



void Game::Initialize()
{
}

void Game::Ready()
{
    SetWindowTitle("DuinFPS");

    InitializeCamera(&fps_camera);
    SetActiveCamera3D(&fps_camera);
}

void Game::HandleInputs()
{
    if (IsKeyPressed(KEY_P)) {
        fpsCameraEnabled = 1;
        DisableCursor();
    }
    if (IsKeyPressed(KEY_O)) {
        fpsCameraEnabled = 0;
        EnableCursor();
    }
}

void Game::Update(double rDelta)
{
}

void Game::PhysicsUpdate(double pDelta)
{
    if (fpsCameraEnabled) {
        UpdateCamera(&fps_camera, CAMERA_FIRST_PERSON);
    }
}

void Game::Draw()
{
    DrawGrid(100, 1.0f);
}

void InitializeCamera(Camera3D *camera)
{
    camera->position = { 0.0f, 10.0f, 10.0f };
    camera->target = { 0.0f, 0.0f, 0.0f };
    camera->up = { 0.0f, 1.0f, 0.0f };
    camera->fovy = 45.0f;
    camera->projection = CAMERA_PERSPECTIVE;
}
