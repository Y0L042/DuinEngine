# Duin Rendering Pipeline

## Overview

Duin uses **BGFX** as its cross-platform rendering backend, supporting D3D11/12, OpenGL, Vulkan, and Metal. The rendering system provides:

- 3D rendering with camera-based view/projection matrices
- Render-to-texture for offscreen rendering (viewports, post-processing)
- Debug drawing for development visualization
- ImGui integration for UI rendering

## Architecture

### Core Components

| Component | File | Purpose |
|-----------|------|---------|
| `Renderer` | `Renderer.h/cpp` | Main rendering interface |
| `Camera` | `Camera.h/cpp` | 3D camera with view/projection |
| `RenderTexture` | `Renderer.h` | Offscreen render target |
| `RenderState` | `Renderer.h` | Current rendering configuration |
| `RenderGeometry` | `RenderGeometry.h` | Primitive geometry definitions |
| `ShaderProgram` | `Renderer.h` | Compiled shader programs |

### BGFX View System

BGFX uses a **multi-view** architecture where each view represents a render pass with its own:
- Framebuffer target (backbuffer or texture)
- Viewport dimensions
- Clear settings
- Transform matrices

**View ID Convention:**
- **View 0**: Main display (backbuffer)
- **View 1+**: Render textures (offscreen targets)

## Render State Machine

The renderer maintains a global `RenderState` that tracks the current rendering context:

```cpp
struct RenderState {
    RenderTexture *target = nullptr;  // Active render target (nullptr = backbuffer)
    Camera *camera = nullptr;         // Active camera
    bool in3DMode = false;            // True between BeginDraw3D/EndDraw3D
    bool inTextureMode = false;       // True between BeginTextureMode/EndTextureMode
    DebugDrawState debugDrawState;
    Matrix viewMatrix;
    Matrix projectionMatrix;
    UUID stateUUID;
    size_t viewID = 0;                // Current BGFX view ID
};
```

### State Stack

The renderer uses a stack-based state machine to support nested rendering modes:

```
globalRenderStateStack: [State0, State1, State2, ...]
                                               ↑
                              globalRenderState (current)
```

When entering a new mode (e.g., texture mode), the current state is pushed onto the stack. When exiting, the previous state is popped and restored.

## Rendering Modes

### 1. Main Display Rendering (View 0)

Standard 3D rendering to the screen backbuffer.

```cpp
// Begin 3D rendering with camera
duin::BeginDraw3D(camera);

// Draw calls go to View 0 (main display)
duin::DrawBox(position, rotation, size);
duin::DrawSphere(position, rotation, size);
duin::DrawGrid(100.0f);

// End 3D rendering
duin::EndDraw3D();

// Present frame
duin::ExecuteRenderPipeline();
```

**What happens in `BeginDraw3D`:**
1. Clears the render state stack
2. Sets view/projection matrices from camera
3. Configures BGFX view 0 (transform, viewport, clear)
4. Sets `in3DMode = true`, `camera = &camera`
5. Pushes state to stack
6. Begins BGFX encoder and debug draw

### 2. Render-to-Texture Mode (View 1+)

Renders to an offscreen texture instead of the screen.

```cpp
// Create render texture with unique view ID
duin::RenderTexture texture(width, height, uuid);
texture.viewID = 1;  // IMPORTANT: Must be > 0 to avoid conflict with main display

// Begin texture mode
duin::BeginTextureMode(texture);

// All draw calls now go to View 1 (texture)
duin::DrawGrid(100.0f);
duin::DrawDebugSphere({0, 0, 0}, 5.0f);

// End texture mode
duin::EndTextureMode();

// Display texture in ImGui
duin::DrawIMGUITexture(texture, {width, height});
```

**What happens in `BeginTextureMode`:**
1. Pushes current state to stack
2. Sets `inTextureMode = true`, `target = &texture`
3. Sets `viewID = texture.viewID`
4. Binds BGFX framebuffer to the texture's view ID
5. Sets viewport and clear for the texture's view

**What happens in `EndTextureMode`:**
1. Pops previous state from stack
2. Restores `viewID`, `target`, mode flags

### 3. Nested Texture Modes

Texture modes can be nested (e.g., render texture A, then within that render to texture B):

```cpp
duin::RenderTexture texA, texB;
texA.viewID = 1;
texB.viewID = 2;

duin::BeginTextureMode(texA);  // viewID = 1
    // draws go to View 1

    duin::BeginTextureMode(texB);  // viewID = 2
        // draws go to View 2
    duin::EndTextureMode();  // viewID restored to 1

    // draws go to View 1 again
duin::EndTextureMode();  // viewID restored to 0
```

## Draw Functions

All draw functions submit to `globalRenderState.viewID`, ensuring they render to the correct target:

| Function | Description |
|----------|-------------|
| `DrawBox()` | Draws a box primitive |
| `DrawSphere()` | Draws a sphere primitive |
| `DrawSquare()` | Draws a plane/square primitive |
| `DrawPlane()` | Draws a plane at origin |
| `DrawGrid()` | Draws a debug grid |
| `DrawDebugSphere()` | Draws a debug wireframe sphere |

### Internal Flow

```
DrawBox(pos, rot, size)
    ↓
QueueRender(RenderGeometryType::BOX, pos, rot, size)
    ↓
encoder->submit(globalRenderState.viewID, program)
    ↓
BGFX renders to the view's framebuffer
```

## RenderTexture

A `RenderTexture` is an offscreen render target backed by a BGFX texture and framebuffer:

```cpp
struct RenderTexture {
    uint16_t width, height;              // Texture dimensions
    UUID textureUUID;                    // Unique identifier
    bgfx::ViewId viewID = 0;             // BGFX view ID for this texture
    bgfx::TextureHandle texture;         // BGFX texture handle
    bgfx::FrameBufferHandle frameBuffer; // BGFX framebuffer handle
};
```

### Creating a RenderTexture

```cpp
// Create with dimensions and UUID
duin::RenderTexture rt(800, 600, duin::UUID());

// CRITICAL: Assign unique viewID > 0
rt.viewID = 1;

// Verify validity before use
if (rt.IsValid()) {
    // Safe to render
}
```

### ViewID Best Practices

| ViewID | Usage |
|--------|-------|
| 0 | Reserved for main display |
| 1-255 | Available for render textures |

**Rules:**
1. Each `RenderTexture` must have a unique `viewID`
2. Never use `viewID = 0` for render textures (conflicts with main display)
3. Manage viewIDs to avoid collisions when using multiple textures

### Cleanup

```cpp
// Destroy when no longer needed
if (rt.IsValid()) {
    rt.Destroy();
}
```

## Camera System

The `Camera` struct defines 3D view parameters:

```cpp
struct Camera {
    UUID uuid;
    Vector3 position;    // Camera position in world space
    Vector3 target;      // Point the camera looks at
    Vector3 up;          // Computed up vector (orthonormal)
    float fovy = 72.0f;  // Vertical field of view in degrees
};
```

### Coordinate System

Duin uses a **right-hand** coordinate system with **+Z toward the viewer** (same as Godot).

### Camera Matrix Computation

```cpp
// View matrix: world → camera space
Matrix view = GetCameraViewMatrix(&camera);

// Projection matrix: camera → clip space (perspective)
Matrix proj = GetCameraProjectionMatrix(&camera);
```

### Camera Movement

```cpp
CameraMoveForward(&camera, distance);  // Move along forward axis
CameraMoveUp(&camera, distance);       // Move along up axis
CameraMoveRight(&camera, distance);    // Move along right axis
CameraMoveToTarget(&camera, delta);    // Move toward/away from target
```

### Camera Rotation

```cpp
CameraYaw(&camera, angle, rotateAroundTarget);   // Rotate around vertical axis
CameraPitch(&camera, angle, lockView, rotateAroundTarget, rotateUp);  // Rotate around horizontal axis
CameraRoll(&camera, angle);  // Rotate around forward axis
```

## Debug Drawing

The debug draw system uses `bgfx-debugdraw` for immediate-mode debug visualization:

```cpp
// Debug draw is automatically started in BeginDraw3D
duin::DrawGrid(100.0f);           // Grid on Y plane
duin::DrawDebugSphere(pos, 5.0f); // Wireframe sphere
```

Debug drawing respects the current `viewID`, so debug shapes render to the correct target (main display or texture).

## Frame Lifecycle

A typical frame follows this pattern:

```cpp
void Draw() {
    // 1. Render scene to texture (for editor viewport)
    duin::BeginTextureMode(sceneTexture);
        duin::ClearBackground(duin::GRAY, sceneTexture.viewID);
        duin::DrawGrid(100.0f);
        // ... draw scene objects
    duin::EndTextureMode();

    // 2. Main 3D pass (optional, for game view)
    duin::BeginDraw3D(mainCamera);
        // ... draw game objects
    duin::EndDraw3D();
}

void DrawUI() {
    // 3. ImGui pass
    ImGui::Begin("Viewport");
        duin::DrawIMGUITexture(sceneTexture, {800, 600});
    ImGui::End();
}

void PostDraw() {
    // 4. Present frame
    duin::ExecuteRenderPipeline();  // Calls bgfx::frame()
}
```

## Geometry System

Primitive geometry is defined in `RenderGeometry.h`:

```cpp
namespace RenderGeometryType {
    enum Type {
        BOX = 0,
        SPHERE,
        CAPSULE,
        PLANE,
        CONE,
        CYLINDER,
        DISK,
        TRIANGLE,
        TRIANGLEMESH,
        NONE,
        Count
    };
}
```

Geometry buffers are created at initialization and reused:

```cpp
// Vertex format: Position (float3) + Color (uint32 ABGR)
struct PosColorVertex {
    float x, y, z;
    uint32_t abgr;
};
```

## Shader System

Shaders are managed via `ShaderProgram`:

```cpp
struct ShaderProgram {
    UUID uuid;
    bgfx::ShaderHandle vsh;      // Vertex shader
    bgfx::ShaderHandle fsh;      // Fragment shader
    bgfx::ProgramHandle program; // Combined program
    uint8_t isValid = 0;
};
```

Default shaders are loaded at initialization from precompiled `.bin` files.

## Test Accessors

For unit testing render state without GPU context:

```cpp
// Get current state (copy)
RenderState state = duin::GetCurrentRenderState();

// Get stack depth
size_t depth = duin::GetRenderStateStackDepth();

// Reset state for testing
duin::ResetRenderStateForTesting();

// Simulate texture mode (no GPU calls)
duin::BeginTextureModeForTesting(texture);
duin::EndTextureModeForTesting();

// Verify viewID usage
size_t expected = duin::GetExpectedDrawViewID();
size_t queueRender = duin::GetQueueRenderViewID();
size_t debugDraw = duin::GetDebugDrawViewID();
```

## Common Issues

### Render-to-Texture Shows on Main Screen

**Cause:** `viewID` set to 0 (same as main display) or draw calls ignoring `globalRenderState.viewID`.

**Solution:** Ensure `texture.viewID > 0` and that all draw functions use `globalRenderState.viewID` for submissions.

### Flickering After EndTextureMode

**Cause:** Framebuffer binding not properly restored.

**Solution:** Use different viewIDs for each render target. View 0 should only be used for the main backbuffer.

### Debug Draws Not Appearing in Texture

**Cause:** `BeginDebugDraw` using wrong viewID.

**Solution:** Verify `BeginDebugDraw` uses `globalRenderState.viewID`.

## File Locations

| File | Path |
|------|------|
| Renderer | `Duin/src/Duin/Render/Renderer.h`, `Renderer.cpp` |
| Camera | `Duin/src/Duin/Render/Camera.h`, `Camera.cpp` |
| RenderGeometry | `Duin/src/Duin/Render/RenderGeometry.h`, `RenderGeometry.cpp` |
| Shader | `Duin/src/Duin/Render/Shader.h`, `Shader.cpp` |
| Color | `Duin/src/Duin/Render/Color.h` |
| Tests | `DuinTests/src/Render/TestRenderer.cpp` |
