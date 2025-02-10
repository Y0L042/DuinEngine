#ifndef EDITOR_CAMERA_H
#define EDITOR_CAMERA_H

#include <Duin.h>

class Draggable
{
    public:
        Draggable(int mouseKey)
            : mouseButton(mouseKey) {};
        
        Vector2 HandleInfiniteDragging();

    private:
        int isDragging = 0;
        ::Vector2 dragStartPos = {0};
        int mouseButton = -1;
};

void HandleInfiniteMouseDragging(int mouse_button);
int IsMouseDragging();

float CalculateCameraYaw(Camera3D *camera);
float CalculateCameraPitch(Camera3D *camera);

void UpdateEditorCameraPosition(Camera3D *camera, double delta);

Camera3D InitializeCompassCamera();
void UpdateCompassCamera(Camera3D *compassCamera, Camera3D *mainCamera);
void DrawCompass3D(Camera3D compassCamera, RenderTexture2D compassTexture);
void DrawCompassUI(RenderTexture2D compassTexture, Rectangle compassArea);
void UpdateMainCameraFromCompass(Camera3D* mainCamera, Rectangle compassArea);

#endif /* EDITOR_CAMERA_H */
