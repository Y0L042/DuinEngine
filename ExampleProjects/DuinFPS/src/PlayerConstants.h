#pragma once

namespace PlayerConstants
{
    constexpr int TARGET_FRAMERATE = 244;

    constexpr float MOUSE_SENSITIVITY = 0.001f;

    constexpr float PITCH_MAX_DEG = 89.0f;
    constexpr float PITCH_MIN_DEG = -87.0f;

    constexpr float IDLE_SMOOTH_FACTOR = 20.75f;
    constexpr float RUN_SMOOTH_FACTOR = 10.75f;
    constexpr float SPRINT_SMOOTH_FACTOR = 4.5f;

    constexpr float AIR_SPEED_FACTOR = 0.65f;
    constexpr float AIR_ACCELERATION = 0.25f;
    constexpr float GROUND_ACCELERATION = 1.0f;

    constexpr float GROUND_FRICTION = 1.0f;
    constexpr float AIR_FRICTION = 0.01f;

    constexpr float DEBUG_CAMERA_SPEED = 5.0f;
} // namespace PlayerConstants
