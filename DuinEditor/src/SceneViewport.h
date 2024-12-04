#pragma once
#include <Duin.h>

RenderTexture2D& GetViewportRenderTexture();
float GetSceneViewportX();
float GetSceneViewportY();
float ScaleToSceneViewportX(float width);
float ScaleToSceneViewportY(float height);
void InitializeSceneViewport();
void DrawSceneViewport();
void DrawToSceneViewport(std::function<void(void)>);

