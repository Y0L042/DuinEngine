#include "SceneViewport.h"

#include <imgui.h>
#include <functional>

static RenderTexture2D viewportRenderTexture;
static ImVec2 viewportPosition;
static ImVec2 viewportSize;
static int isSceneViewportHovered = 0;
static int isSceneViewportFocused = 0;

RenderTexture2D& GetViewportRenderTexture()
{
    return viewportRenderTexture;
}

float GetSceneViewportX()
{
    return viewportPosition.x;
}

float GetSceneViewportY()
{
    return viewportPosition.y;
}

int IsSceneViewportHovered()
{
    return isSceneViewportHovered;
}

int IsSceneViewportFocused()
{
    return isSceneViewportFocused;
}

float ScaleToSceneViewportX(float width)
{
    return width / GetScreenWidth() * viewportSize.x;
}

float ScaleToSceneViewportY(float height)
{
    return height / GetScreenHeight() * viewportSize.y;
}

void InitializeSceneViewport()
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    viewportRenderTexture = LoadRenderTexture(screenWidth, screenHeight);
}

void DrawSceneViewport()
{
    ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    isSceneViewportHovered = ImGui::IsWindowHovered();
    isSceneViewportFocused = ImGui::IsWindowFocused();
        viewportPosition = ImGui::GetCursorScreenPos();
        viewportSize = ImGui::GetContentRegionAvail();

        rlImGuiImageRenderTextureFit(&viewportRenderTexture, true);
    ImGui::End();
}

void DrawToSceneViewport(std::function<void(void)> f)
{
    BeginTextureMode(viewportRenderTexture);
        f();
    EndTextureMode();
}
