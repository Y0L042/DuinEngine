#pragma once

#include <Duin.h>

#include <memory>

struct RenderableCMP
{
    Duin::Color color;
    float size;
    std::vector<Duin::Vector2> vertices;

    RenderableCMP(Duin::Color color, float size, std::vector<Duin::Vector2>& vertices)
        : color(color), size(size), vertices(vertices) {}
};