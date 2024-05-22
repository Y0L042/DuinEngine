#pragma once

#include "Vector2.h"
#include <cmath>

namespace Duin
{
    struct Matrix2x2 
    {
        float m[2][2];

        Matrix2x2() 
        {
            for (int i = 0; i < 2; ++i)
            {
                for (int j = 0; j < 2; ++j)
                {
                    m[i][j] = (i == j) ? 1.0f : 0.0f; // Identity matrix
                }
            }
        }

        Matrix2x2(float a, float b, float c, float d) 
        {
            m[0][0] = a;
            m[0][1] = b;
            m[1][0] = c;
            m[1][1] = d;
        }

        static Matrix2x2 Rotate(float angle) 
        {
            float c = cos(angle);
            float s = sin(angle);
            return Matrix2x2(c, -s, s, c);
        }

        static Matrix2x2 Scale(float sx, float sy) 
        {
            return Matrix2x2(sx, 0, 0, sy);
        }

        Matrix2x2 operator+(const Matrix2x2& other) const
        {
            return Matrix2x2(
                m[0][0] + other.m[0][0],
                m[0][1] + other.m[0][1],
                m[1][0] + other.m[1][0],
                m[1][1] + other.m[1][1]
            );
        }

        Matrix2x2 operator-(const Matrix2x2& other) const
        {
            return Matrix2x2(
                m[0][0] - other.m[0][0],
                m[0][1] - other.m[0][1],
                m[1][0] - other.m[1][0],
                m[1][1] - other.m[1][1]
            );
        }

        Matrix2x2 operator+=(const Matrix2x2& other)
        {
            m[0][0] += other.m[0][0];
            m[0][1] += other.m[0][1];
            m[1][0] += other.m[1][0];
            m[1][1] += other.m[1][1];
            return Matrix2x2(
                m[0][0],
                m[0][1],
                m[1][0],
                m[1][1]
            );
        }

        Matrix2x2 operator-=(const Matrix2x2& other)
        {
            m[0][0] -= other.m[0][0];
            m[0][1] -= other.m[0][1];
            m[1][0] -= other.m[1][0];
            m[1][1] -= other.m[1][1];
            return Matrix2x2(
                m[0][0],
                m[0][1],
                m[1][0],
                m[1][1]
            );
        }

        Matrix2x2 operator*(const Matrix2x2& other) const 
        {
            return Matrix2x2(
                m[0][0] * other.m[0][0] + m[0][1] * other.m[1][0],
                m[0][0] * other.m[0][1] + m[0][1] * other.m[1][1],
                m[1][0] * other.m[0][0] + m[1][1] * other.m[1][0],
                m[1][0] * other.m[0][1] + m[1][1] * other.m[1][1]
            );
        }

        Vector2 operator*(const Vector2& v) const 
        {
            return Vector2(
                m[0][0] * v.x + m[0][1] * v.y,
                m[1][0] * v.x + m[1][1] * v.y
            );
        }
    };

}