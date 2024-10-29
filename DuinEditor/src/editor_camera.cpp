#include "editor_camera.h"

void EditorCamera_UpdatePosition(Camera3D *camera, double delta)
{
    static float rotation_angle = 0.0f;      // Angle around the Y-axis
    static float tilt_angle = -PI / 4;        // Tilt angle around the X-axis
    static float rotation_speed = 0.005f;    // Adjust this for sensitivity
    static float tilt_speed = 0.005f;
    static float cam_distance = 25.0f;       // Default distance from the target

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
    if (cam_distance < 2.0f) cam_distance = 2.0f;  // Minimum distance

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



// Function to calculate the yaw angle from the camera's direction
float CalculateCameraYaw(Camera3D *camera)
{
    Vector3 direction = GetCameraForward(camera);
    float yaw = atan2f(direction.x, direction.z);
    return yaw;
}

// Function to initialize the compass camera
Camera3D InitializeCompassCamera()
{
    Camera3D compassCamera = { 0 };
    compassCamera.position = { 0.0f, 150.0f, 0.0f }; // Positioned in front of the origin
    compassCamera.target = { 0.0f, 0.0f, 0.0f };   // Looking at the origin
    compassCamera.up = { 0.0f, 1.0f, 0.0f };       // World's up direction
    compassCamera.fovy = 45.0f;
    compassCamera.projection = CAMERA_ORTHOGRAPHIC;

    return compassCamera;
}

// Function to update the compass camera based on main camera's orientation
void UpdateCompassCamera(Camera3D *compassCamera, Camera3D *mainCamera)
{
    // Calculate the yaw and pitch from the main camera's direction
    Vector3 direction = GetCameraForward(mainCamera);
    float yaw = atan2f(direction.x, direction.z);
    float pitch = asinf(direction.y);

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
    BeginTextureMode(compassTexture); // Start drawing to the texture
        ClearBackground({ 255, 255, 255, 0 }); // Clear the texture background

        BeginMode3D(compassCamera);
            // Define the length of the compass axes
            float axisLength = 15.0f; // Adjust as needed for visibility

            // Draw X-axis in Red
            DrawLine3D(
                {0, 0, 0}, 
                {axisLength, 0, 0}, 
                RED
            );

            // Draw Y-axis in Green
            DrawLine3D(
                {0, 0, 0}, 
                {0, axisLength, 0}, 
                GREEN
            );

            // Draw Z-axis in Blue
            DrawLine3D(
                {0, 0, 0}, 
                {0, 0, axisLength}, 
                BLUE
            );

            // Draw axis labels as small spheres
            float radius = 2.5f;

            Color xPColor = Color{ 230, 41, 55, 255 };
            Color xSColor = Color{ 200, 21, 25, 225 };

            Color yPColor = Color{ 0, 228, 48, 255 };
            Color ySColor = Color{ 0, 128, 28, 225 };

            Color zPColor = Color{ 0, 121, 241, 255 };
            Color zSColor = Color{ 0, 60, 141, 225 };


            DrawSphere({axisLength, 0, 0}, radius, xPColor);   // X-label
            DrawSphere({-axisLength, 0, 0}, radius, xSColor);   // X-label

            DrawSphere({0, axisLength, 0}, radius, yPColor); // Y-label
            DrawSphere({0, -axisLength, 0}, radius, ySColor); // Y-label

            DrawSphere({0, 0, axisLength}, radius, zPColor);  // Z-label
            DrawSphere({0, 0, -axisLength}, radius, zSColor);  // Z-label

        EndMode3D();
    EndTextureMode(); // Finish drawing to the texture
}

// Function to draw the compass UI by drawing the RenderTexture2D
void DrawCompassUI(RenderTexture2D compassTexture, Rectangle compassArea)
{
    // Draw the texture at the specified compassArea position
    DrawTexturePro(
        compassTexture.texture, // Source texture
        { 0, 0, (float)compassTexture.texture.width, -(float)compassTexture.texture.height }, // Source rectangle (flipped Y-axis)
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
        DARKGRAY
    );
}
