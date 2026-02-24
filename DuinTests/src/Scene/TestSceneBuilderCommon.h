#pragma once

#include <string>

namespace TestSceneBuilder
{
// Common JSON test data
extern const std::string FULL_ENTITY_JSON;
extern const std::string SIMPLE_ENTITY_JSON;

// Common test structures
struct Vec3
{
    float x, y, z;
    bool operator==(const Vec3 &other) const
    {
        return (x == other.x && y == other.y && z == other.z);
    }
};

struct TestStructX
{
    int x;
};

struct Transform
{
    Vec3 position, rotation, scale;
};

struct Camera
{
    float fov, nearPlane, farPlane;
    bool isPrimary;

    bool operator==(const Camera &other)
    {
        return (fov == other.fov && nearPlane == other.nearPlane && farPlane == other.farPlane &&
                isPrimary == other.isPrimary);
    }
};
struct TAG_Main
{
};
struct TAG_Camera
{
};

} // namespace TestSceneBuilder
