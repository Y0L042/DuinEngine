#ifndef EDITOR_CAMERA_H
#define EDITOR_CAMERA_H

#include <duin.h>


void EditorCamera_UpdatePosition(Camera3D *camera, double delta);
float CalculateCameraYaw(Camera3D *camera);
Camera3D InitializeCompassCamera();
void UpdateCompassCamera(Camera3D *compassCamera, Camera3D *mainCamera);
void DrawCompass3D(Camera3D compassCamera, RenderTexture2D compassTexture);
void DrawCompassUI(RenderTexture2D compassTexture, Rectangle compassArea);

#endif /* EDITOR_CAMERA_H */
