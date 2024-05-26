#pragma once

#include "../MathsCore.h"
#include "Vector2.h"
#include "Mat2x2.h"

#include <cmath>

namespace Duin 
{
    struct DUIN_MATHS_API Transform2D
    {
        Vector2 origin;
        Matrix2x2 transform;

        Transform2D() 
            : origin(0, 0), transform()
        {}

        Transform2D(const Vector2& origin, const Matrix2x2& transform)
            : origin(origin), transform(transform) 
        {}

        static Transform2D Identity() 
        {
            return Transform2D(Vector2(0, 0), Matrix2x2());
        }

        Transform2D Translated(const Vector2& offset) const 
        {
            Transform2D result = *this;
            result.origin = result.origin + offset;
            return result;
        }

        Transform2D Rotated(float angle) const 
        {
            Transform2D result = *this;
            Matrix2x2 rotation = Matrix2x2::Rotate(angle);
            result.transform = result.transform * rotation;
            return result;
        }

        Transform2D Scaled(float sx, float sy) const 
        {
            Transform2D result = *this;
            Matrix2x2 scale = Matrix2x2::Scale(sx, sy);
            result.transform = result.transform * scale;
            return result;
        }

        Vector2 Apply(const Vector2& point) const 
        {
            return transform * point + origin;
        }

        Transform2D operator*(const Transform2D& other) const 
        {
            Transform2D result;
            result.transform = transform * other.transform;
            result.origin = transform * other.origin + origin;
            return result;
        }

        Transform2D operator+(const Transform2D& other) const
        {
            Transform2D result;
            result.origin = origin + other.origin;
            //result.transform = transform + other.transform; // Assuming Matrix2x2 has an operator+ defined
            return result;
        }

        Transform2D operator-(const Transform2D& other) const
        {
            Transform2D result;
            result.origin = origin - other.origin;
            //result.transform = transform - other.transform; // Assuming Matrix2x2 has an operator+ defined
            return result;
        }

        Transform2D& operator+=(const Transform2D& other)
        {
            origin += other.origin;
            //transform += other.transform; // Assuming Matrix2x2 has an operator+= defined
            return *this;
        }

        Transform2D& operator-=(const Transform2D& other)
        {
            origin -= other.origin;
            //transform -= other.transform; // Assuming Matrix2x2 has an operator+= defined
            return *this;
        }
    };
}
