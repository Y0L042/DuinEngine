#pragma once

#include "../MathsCore.h"
#include "Vector2.h"
#include <cmath>

namespace Duin
{
    struct DUIN_MATHS_API Matrix3x3
    {
        float m[3][3];

        Matrix3x3() // Identity matrix
        {
            for (int i = 0; i < 3; ++i)
            {
                for (int j = 0; j < 3; ++j)
                {
                    m[i][j] = (i == j) ? 1.0f : 0.0f;
                }
            }
        }

        static Matrix3x3 Translate(const Vector2& v) 
        {
            Matrix3x3 result;
            result.m[0][2] = v.x;
            result.m[1][2] = v.y;
            return result;
        }

        static Matrix3x3 Scale(const Vector2& v) 
        {
            Matrix3x3 result;
            result.m[0][0] = v.x;
            result.m[1][1] = v.y;
            return result;
        }

        static Matrix3x3 Rotate(float angle) 
        {
            Matrix3x3 result;
            float c = cos(angle);
            float s = sin(angle);
            result.m[0][0] = c;
            result.m[0][1] = -s;
            result.m[1][0] = s;
            result.m[1][1] = c;
            return result;
        }

        Matrix3x3 operator*(const Matrix3x3& other) const 
        {
            Matrix3x3 result;
            for (int i = 0; i < 3; ++i) 
            {
                for (int j = 0; j < 3; ++j) 
                {
                    result.m[i][j] = 0;
                    for (int k = 0; k < 3; ++k) 
                    {
                        result.m[i][j] += m[i][k] * other.m[k][j];
                    }
                }
            }
            return result;
        }

        Vector2 operator*(const Vector2& v) const 
        {
            return Vector2(
                m[0][0] * v.x + m[0][1] * v.y + m[0][2],
                m[1][0] * v.x + m[1][1] * v.y + m[1][2]
            );
        }
    };
}