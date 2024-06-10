#pragma once

#include "../MathsCore.h"
#include "./Maths.h"

#include <cmath>

namespace Duin
{
    struct DUIN_MATHS_API Vector2 {
        #define ZERO Vector2(0.0f, 0.0f)
        #define ONE Vector2(1.0f, 1.0f)
        #define UP Vector2(0.0f, -1.0f)
        #define DOWN Vector2(0.0f, 1.0f)
        #define LEFT Vector2(-1.0f, 0.0f)
        #define RIGHT Vector2(1.0f, 0.0f)

        float x, y;

        static Vector2 AngleToVector2(float radians)
        {
            return Vector2(std::cos(radians), std::sin(radians));
        }

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

        bool operator==(const Vector2& other) const
        {
            return (Maths::AreSame(x, other.x) && Maths::AreSame(y, other.y));
        }

        bool operator!=(const Vector2& other) const
        {
            return !(Maths::AreSame(x, other.x) && Maths::AreSame(y, other.y));
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
            
        float Length() const
        {
            return sqrt(x * x + y * y);
        }

        float LengthSqrd() const
        {
            return (x * x + y * y);
        }

        Vector2 Normalized() const
        {
            float mag = Length();
            if (Maths::AreSame(mag, 0))
            {
                return ZERO;
            }
            float invMag = 1.0f / mag;
            return Vector2(x * invMag, y * invMag);
        }

        Vector2 LimitLength(float minLength, float maxLength) const
        {
            float mag = LengthSqrd();
            if (mag < (minLength * minLength))
            {
                return Normalized() * minLength;
            }
            else if (mag > (maxLength * maxLength))
            {
                return Normalized() * maxLength;
            }
            return *this;
        }

        float ToAngle() const
        {
            return std::atan2f(y, x);
        }
    };
}