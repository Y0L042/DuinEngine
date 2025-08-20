#pragma once

// RenderTexture2D& GetViewportRenderTexture();
float GetSceneViewportX();
float GetSceneViewportY();
int IsSceneViewportHovered();
int IsSceneViewportFocused();
float ScaleToSceneViewportX(float width);
float ScaleToSceneViewportY(float height);
void InitializeSceneViewport();
void DrawSceneViewport();
// void DrawToSceneViewport(std::function<void(void)>);
