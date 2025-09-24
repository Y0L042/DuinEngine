#pragma once

#include <Duin/ECS/ECSModule.h>

// Test components for ECS tests
struct Position {
    int x = 0, y = 0;
    Position() = default;
    Position(int x_, int y_) : x(x_), y(y_) {}
    bool operator==(const Position& other) const { return x == other.x && y == other.y; }
};

struct Velocity {
    float vx = 0, vy = 0;
    Velocity() = default;
    Velocity(float vx_, float vy_) : vx(vx_), vy(vy_) {}
    bool operator==(const Velocity& other) const { return vx == other.vx && vy == other.vy; }
};

struct Tag {};

struct Health {
    int value = 100;
    Health() = default;
    Health(int v) : value(v) {}
    bool operator==(const Health& other) const { return value == other.value; }
};