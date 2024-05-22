#pragma once

#include "./Maths.h"

#include <cmath>

namespace Duin
{
    struct Vector2 {
        float x, y;

        Vector2()
            : x(0), y(0)
        {}

        Vector2(float x, float y)
            : x(x), y(y)
        {}

        Vector2 operator+(const Vector2& other) const
        {
            return Vector2(x + other.x, y + other.y);
        }

        Vector2 operator-(const Vector2& other) const
        {
            return Vector2(x - other.x, y - other.y);
        }

        Vector2 operator*(float scalar) const
        {
            return Vector2(x * scalar, y * scalar);
        }

        Vector2 operator/(float scalar) const
        {
            return Vector2(x / scalar, y / scalar);
        }

        Vector2& operator=(const Vector2& other)
        {
            if (this != &other)
            {
                x = other.x;
                y = other.y;
            }
            return *this;
        }

        bool operator==(const Vector2& other)
        {
            return (Maths::AreSame(x, other.x) && Maths::AreSame(y, other.y));
        }

        Vector2& operator+=(const Vector2& other)
        {
            x += other.x;
            y += other.y;
            return *this;
        }

        Vector2& operator-=(const Vector2& other)
        {
            x -= other.x;
            y -= other.y;
            return *this;
        }

        Vector2& operator*=(float scalar)
        {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        Vector2& operator/=(float scalar)
        {
            x /= scalar;
            y /= scalar;
            return *this;
        }

        float Dot(const Vector2& other) const
        {
            return x * other.x + y * other.y;
        }

        float Magnitude() const
        {
            return sqrt(x * x + y * y);
        }

        Vector2 Normalized() const
        {
            float mag = Magnitude();
            return Vector2(x / mag, y / mag);
        }
    };
}