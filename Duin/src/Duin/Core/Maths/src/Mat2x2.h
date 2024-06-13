#pragma once
#include "../MathsCore.h"
#include "Vector2.h"
#include <cmath>

namespace Duin
{
    struct DUIN_MATHS_API Mat2x2 
    {
        #define Mat2x2_ZERO Mat2x2(0, 0, 0, 0)
        #define Mat2x2_IDENTITY Mat2x2(1, 0, 0, 1)
        #define Mat2x2_ONE Mat2x2(1, 1, 1, 1)

        float m[2][2];

        Mat2x2() 
        {
            for (int i = 0; i < 2; ++i)
            {
                for (int j = 0; j < 2; ++j)
                {
                    m[i][j] = (i == j) ? 1.0f : 0.0f; // Identity matrix
                }
            }
        }

        Mat2x2(float a, float b, float c, float d) 
        {
            m[0][0] = a;
            m[0][1] = b;
            m[1][0] = c;
            m[1][1] = d;
        }

        static Mat2x2 Rotate(float angle) 
        {
            float c = cos(angle);
            float s = sin(angle);
            return Mat2x2(c, -s, s, c);
        }

        static Mat2x2 Scale(float sx, float sy) 
        {
            return Mat2x2(sx, 0, 0, sy);
        }

        Mat2x2 operator+(const Mat2x2& other) const
        {
            return Mat2x2(
                m[0][0] + other.m[0][0],
                m[0][1] + other.m[0][1],
                m[1][0] + other.m[1][0],
                m[1][1] + other.m[1][1]
            );
        }

        Mat2x2 operator-(const Mat2x2& other) const
        {
            return Mat2x2(
                m[0][0] - other.m[0][0],
                m[0][1] - other.m[0][1],
                m[1][0] - other.m[1][0],
                m[1][1] - other.m[1][1]
            );
        }

        Mat2x2 operator+=(const Mat2x2& other)
        {
            m[0][0] += other.m[0][0];
            m[0][1] += other.m[0][1];
            m[1][0] += other.m[1][0];
            m[1][1] += other.m[1][1];
            return Mat2x2(
                m[0][0],
                m[0][1],
                m[1][0],
                m[1][1]
            );
        }

        Mat2x2 operator-=(const Mat2x2& other)
        {
            m[0][0] -= other.m[0][0];
            m[0][1] -= other.m[0][1];
            m[1][0] -= other.m[1][0];
            m[1][1] -= other.m[1][1];
            return Mat2x2(
                m[0][0],
                m[0][1],
                m[1][0],
                m[1][1]
            );
        }

        Mat2x2 operator*(const Mat2x2& other) const 
        {
            return Mat2x2(
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