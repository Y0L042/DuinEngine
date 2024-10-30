#include "editor_camera.h"


static int isDragging = 0;
void HandleInfiniteMouseDragging(int mouse_button)
{
    static Vector2 dragStartPos = {0};
    static int current_mouse_button = -1;

    // Start drag when the right mouse button is pressed
    if (!isDragging && IsMouseButtonDown(mouse_button))
    {
        isDragging = 1;
        current_mouse_button = mouse_button;
        dragStartPos = GetMousePosition();  // Store initial click position
        DisableCursor();  // Hide the cursor
    }

    // Stop drag when the right mouse button is released
    if (isDragging && !IsMouseButtonDown(current_mouse_button))
    {
        isDragging = 0;
        current_mouse_button = -1;
        EnableCursor();  // Show the cursor again
        SetMousePosition(dragStartPos.x, dragStartPos.y); // Reset cursor to original position
        dragStartPos = {0};  // Reset starting position
    }

    return;
    // If dragging, capture and reposition the mouse
    if (isDragging)
    {
        Vector2 currentPos = GetMousePosition();
        Vector2 mouseDelta = Vector2Subtract(currentPos, dragStartPos);

        // Process the mouseDelta for camera rotation or other movement logic

        // Recenter the mouse to dragStartPos after processing delta
        SetMousePosition(dragStartPos.x, dragStartPos.y);
    }
}

int IsMouseDragging()
{
    return isDragging;
}

void UpdateEditorCameraPosition(Camera3D *camera, double delta)
{
    static float rotation_angle = 0.0f;
    static float tilt_angle = -PI / 4;
    static float rotation_speed = 0.005f;
    static float tilt_speed = 0.005f;
    static float cam_distance = 25.0f;

    static Vector2 old_pos = { 0.0f, 0.0f };
    if (!IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        old_pos = GetMousePosition();
    }

    // Update only on right-click (for rotation)
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        Vector2 new_pos = GetMousePosition();
        
        // Calculate the delta movement from the mouse
        rotation_angle += (new_pos.x - old_pos.x) * rotation_speed;
        // Invert to make dragging up/down intuitive
        tilt_angle -= (new_pos.y - old_pos.y) * tilt_speed;  

        // Clamp tilt to avoid gimbal lock (keep within a reasonable range)
        if (tilt_angle > PI / 2 - 0.1f)
            tilt_angle = PI / 2 - 0.1f;
        else if (tilt_angle < -PI / 2 + 0.1f)
            tilt_angle = -PI / 2 + 0.1f;

        old_pos = new_pos;
    }

    // Adjust camera distance using mouse wheel
    cam_distance += GetMouseWheelMove() * 1.5f;
    if (cam_distance < 2.0f) cam_distance = 2.0f;

    // Calculate the new camera position using spherical coordinates
    Vector3 camera_position = {
        cam_distance * cosf(tilt_angle) * sinf(rotation_angle),
        cam_distance * sinf(tilt_angle),
        cam_distance * cosf(tilt_angle) * cosf(rotation_angle)
    };

    // Set the camera target (pivot point) to the center or chosen point
    camera->target = { 0.0f, 0.0f, 0.0f };

    // Set the camera position by adding the calculated offset
    camera->position = Vector3Add(camera->target, camera_position);

    // Set the up vector (keeps camera rotation stable around the Y-axis)
    camera->up = { 0.0f, 1.0f, 0.0f };
}




// Function to initialize the compass camera
Camera3D InitializeCompassCamera()
{
    Camera3D compassCamera = { 0 };
    compassCamera.position = { 0.0f, 150.0f, 0.0f };
    compassCamera.target = { 0.0f, 0.0f, 0.0f };
    compassCamera.up = { 0.0f, 1.0f, 0.0f };
    compassCamera.fovy = 45.0f;
    compassCamera.projection = CAMERA_ORTHOGRAPHIC;

    return compassCamera;
}

// Function to update the compass camera based on main camera's orientation
void UpdateCompassCamera(Camera3D *compassCamera, Camera3D *mainCamera)
{
    // Calculate the yaw and pitch from the main camera's direction
    Vector3 direction = GetCameraForward(mainCamera);
    float yaw = atan2f(-direction.x, -direction.z);
    float pitch = -asinf(direction.y);

    // Update compass camera's position to always look at the origin
    float distance = 17.5f;
    compassCamera->position = {
        distance * cosf(pitch) * sinf(yaw),
        distance * sinf(pitch),
        distance * cosf(pitch) * cosf(yaw)
    };
    compassCamera->target = { 0.0f, 0.0f, 0.0f };
    compassCamera->up = { 0.0f, 1.0f, 0.0f };
}

// Function to draw the 3D Compass into a RenderTexture2D
void DrawCompass3D(Camera3D compassCamera, RenderTexture2D compassTexture)
{
    BeginTextureMode(compassTexture);
        ClearBackground({ 255, 255, 255, 0 });
        BeginMode3D(compassCamera);
            // Define the length of the compass axes
            float axisLength = 15.0f;
            float c_rad = 1.0f;


            // Draw X-axis in Red
            DrawCylinderEx(
                {0, 0, 0}, 
                {axisLength, 0, 0}, 
                c_rad, 
                c_rad * 0.25f, 
                1, 
                MAROON
            );
            DrawLine3D(
                {0, 0, 0}, 
                {axisLength, 0, 0}, 
                RED
            );

            // Draw Y-axis in Green
            DrawCylinderEx(
                {0, 0, 0}, 
                {0, axisLength, 0}, 
                c_rad, 
                c_rad * 0.25f, 
                1, 
                DARKGREEN
            );
            DrawLine3D(
                {0, 0, 0}, 
                {0, axisLength, 0}, 
                GREEN
            );

            // Draw Z-axis in Blue
            DrawCylinderEx(
                {0, 0, 0}, 
                {0, 0, axisLength}, 
                c_rad, 
                c_rad * 0.25f, 
                1, 
                DARKBLUE
            );
            DrawLine3D(
                {0, 0, 0}, 
                {0, 0, axisLength}, 
                BLUE
            );

            // Draw axis labels as small spheres
            float yaw = CalculateCameraYaw(&compassCamera);
            float pitch = CalculateCameraPitch(&compassCamera);
            float radius = 2.5f;
            

            Color xPColor = Color{ 230, 41, 55, 255 };
            Color xSColor = Color{ 200, 31, 35, 235 };

            Color yPColor = Color{ 0, 228, 48, 255 };
            Color ySColor = Color{ 0, 138, 38, 235 };

            Color zPColor = Color{ 0, 121, 241, 255 };
            Color zSColor = Color{ 0, 70, 151, 235 };


            DrawSphere({axisLength, 0, 0}, radius, xPColor);   // X-label
            DrawSphere({-axisLength, 0, 0}, radius, xSColor);   // X-label

            DrawSphere({0, axisLength, 0}, radius, yPColor); // Y-label
            DrawSphere({0, -axisLength, 0}, radius, ySColor); // Y-label

            DrawSphere({0, 0, axisLength}, radius, zPColor);  // Z-label
            DrawSphere({0, 0, -axisLength}, radius, zSColor);  // Z-label

            // This cylinder must be more opaque when viewed from the bottom,
            // and more transparent when viewed from the top.  
            unsigned char T_COL = 128;
            unsigned char B_COL = 50;
            int T_OPACITY = 175; // Top opacity
            int B_OPACITY = 240; // Bottom opacity
            float opacity = pitch < 0 ? T_OPACITY : B_OPACITY;
            unsigned char colorGrad = pitch < 0 ? T_COL : B_COL;
            Color cylinderColor = { 
                colorGrad, 
                colorGrad, 
                colorGrad, 
                (unsigned char)opacity 
            }; 
            DrawCylinderEx(
                {0, -0.1f, 0}, 
                {0, 0.1f, 0}, 
                axisLength + radius, 
                axisLength + radius, 
                32, 
                cylinderColor
            );

        EndMode3D();
    EndTextureMode(); // Finish drawing to the texture
}

// Function to draw the compass UI by drawing the RenderTexture2D
void DrawCompassUI(RenderTexture2D compassTexture, Rectangle compassArea)
{
    // Draw the texture at the specified compassArea position
    Rectangle targetRect = {
        0,
        0,
        (float)compassTexture.texture.width, 
        -(float)compassTexture.texture.height 
    };
    DrawTexturePro(
        compassTexture.texture, // Source texture
        targetRect,
        compassArea, // Destination rectangle
        { 0, 0 }, // Origin
        0.0f, // Rotation
        { 255, 255, 255, 255 } // Tint color
    );


    // Draw compass border for better visibility
    DrawRectangleLines(
        compassArea.x, 
        compassArea.y, 
        compassArea.width, 
        compassArea.height, 
        {75, 75, 75, 0}
    );
}

void UpdateMainCameraFromCompass(Camera3D* mainCamera, Rectangle compassArea) 
{
    int isMouseOnCompassArea = CheckCollisionPointRec(
        GetMousePosition(), compassArea);


    if (isMouseOnCompassArea) {
        static Vector2 lastMousePosition = { 0, 0 };
        if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            lastMousePosition = GetMousePosition();
            return;
        }
        Vector2 mouseDelta = Vector2Subtract(GetMousePosition(), lastMousePosition);

        // Adjust main camera's yaw and pitch based on mouse movement over the compass
        float rotationSpeed = 0.001f;
        float yawDelta = mouseDelta.x * rotationSpeed;
        float pitchDelta = mouseDelta.y * rotationSpeed;
        Vector3 xRotation = Vector3RotateByAxisAngle({1, 1, 1}, {1, 0, 0}, pitchDelta);
        Vector3 yRotation = Vector3RotateByAxisAngle({1, 1, 1}, {0, 1, 0}, yawDelta);

        CameraYaw(mainCamera, yawDelta, 1);
        CameraPitch(mainCamera, pitchDelta, 1, 1, 1);

        lastMousePosition = GetMousePosition();
    }
}

