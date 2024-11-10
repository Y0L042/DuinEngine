#pragma once

#include "../MathsCore.h"
#include "./Maths.h"

#include <cmath>
#include <raymath.h>

namespace Duin {

struct DUIN_MATHS_API Vector2 {

    /*    
    TODO:
    Vector2 Vector2Negate(Vector2 v);                                           // Negate vector
    Vector2 Vector2Divide(Vector2 v1, Vector2 v2);                              // Divide vector by vector
    Vector2 Vector2Transform(Vector2 v, Matrix mat);                            // Transforms a Vector2 by a given Matrix
    Vector2 Vector2Lerp(Vector2 v1, Vector2 v2, float amount);                  // Calculate linear interpolation between two vectors
    Vector2 Vector2Reflect(Vector2 v, Vector2 normal);                          // Calculate reflected vector to normal
    Vector2 Vector2Rotate(Vector2 v, float angle);                              // Rotate vector by angle
    Vector2 Vector2MoveTowards(Vector2 v, Vector2 target, float maxDistance);   // Move Vector towards target
    Vector2 Vector2Invert(Vector2 v);                                           // Invert the given vector
    Vector2 Vector2Clamp(Vector2 v, Vector2 min, Vector2 max);                  // Clamp the components of the vector between min and max values specified by the given vectors
    Vector2 Vector2ClampValue(Vector2 v, float min, float max);                 // Clamp the magnitude of the vector between two min and max values
    int Vector2Equals(Vector2 p, Vector2 q);                                    // Check whether two given vectors are almost equal

    DONE:
    Vector2 Vector2Add(Vector2 v1, Vector2 v2);                                 // Add two vectors (v1 + v2)
    Vector2 Vector2AddValue(Vector2 v, float add);                              // Add vector and float value
    Vector2 Vector2Subtract(Vector2 v1, Vector2 v2);                            // Subtract two vectors (v1 - v2)
    Vector2 Vector2SubtractValue(Vector2 v, float sub);                         // Subtract vector by float value
    float Vector2Distance(Vector2 v1, Vector2 v2);                              // Calculate distance between two vectors
    float Vector2DistanceSqr(Vector2 v1, Vector2 v2);                           // Calculate square distance between two vectors
    float Vector2Angle(Vector2 v1, Vector2 v2);                                 // Calculate angle from two vectors
    Vector2 Vector2Normalize(Vector2 v);                                        // Normalize provided vector
    Vector2 Vector2Zero(void);                                                  // Vector with components value 0.0f
    Vector2 Vector2One(void);                                                   // Vector with components value 1.0f
    float Vector2DotProduct(Vector2 v1, Vector2 v2);                            // Calculate two vectors dot product
    float Vector2Length(Vector2 v);                                             // Calculate vector length
    float Vector2LengthSqr(Vector2 v);                                          // Calculate vector square length
    Vector2 Vector2Scale(Vector2 v, float scale);                               // Scale vector (multiply by value)
    Vector2 Vector2Multiply(Vector2 v1, Vector2 v2);                            // Multiply vector by vector
*/

    /*------------------------------------------------------------------------*/
    //      Defines
    /*------------------------------------------------------------------------*/
#define ZERO Vector2(0.0f, 0.0f)
#define ONE Vector2(1.0f, 1.0f)
#define UP Vector2(0.0f, -1.0f)
#define DOWN Vector2(0.0f, 1.0f)
#define LEFT Vector2(-1.0f, 0.0f)
#define RIGHT Vector2(1.0f, 0.0f)

    /*------------------------------------------------------------------------*/
    //      Properties
    /*------------------------------------------------------------------------*/
    float x, y;

    /*------------------------------------------------------------------------*/
    //      Methods
    /*------------------------------------------------------------------------*/



    Vector2()
    : x(0), y(0)
    {}

    Vector2(float x, float y)
    : x(x), y(y)
    {}



    /*------------------------------------------------------------------------*/
    //      Operators
    /*------------------------------------------------------------------------*/
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

};

inline Vector2 Vector2Add(const Vector2 v1, const Vector2 v2)
{
    return Vector2(v1.x + v2.x, v1.y + v2.y);
}

inline Vector2 Vector2AddValue(const Vector2 v, const float val)
{
    return Vector2(v.x + val, v.y + val);
}

inline Vector2 Vector2Subtract(const Vector2 v1, const Vector2 v2)
{
    return Vector2(v1.x - v2.x, v1.y - v2.y);
}

inline Vector2 Vector2SubtractValue(const Vector2 v, const float val)
{
    return Vector2(v.x - val, v.y - val);
}

inline float Vector2Distance(const Vector2 v1, const Vector2 v2)
{
    return sqrtf(
        ((v1.x - v2.x) * (v1.x - v2.x))
        + ((v1.y - v2.y) * (v1.y - v2.y))
    );
}

inline float Vector2DistanceSqrd(const Vector2 v1, const Vector2 v2)
{
    return (
        ((v1.x - v2.x) * (v1.x - v2.x))
        + ((v1.y - v2.y) * (v1.y - v2.y))
    );
}

inline float Vector2Dot(const Vector2 v1, const Vector2 other)
{
    return v1.x * other.x + v1.y * other.y;
}

inline float Vector2Length(const Vector2 v)
{
    return sqrt(v.x * v.x + v.y * v.y);
}

inline float Vector2LengthSqrd(const Vector2 v)
{
    return (v.x * v.x + v.y * v.y);
}

inline float Vector2AngleBetween(const Vector2 v1, const Vector2 v2)
{
    return atan2f(
        (v1.x * v2.x + v1.y * v2.y),    // dot
        (v1.x * v2.y - v1.y * v2.x)     // det
    );
}

inline float Vector2ToAngle(const Vector2 v)
{
    return std::atan2f(v.y, v.x);
}

inline Vector2 Vector2Scale(const Vector2 v, const float scale)
{
    return Vector2(v.x * scale, v.y * scale);
}

inline Vector2 Vector2Multiply(const Vector2 v1, const Vector2 v2)
{
    return Vector2(v1.x * v2.x, v1.y * v2.y);
}

inline Vector2 Vector2Normalized(const Vector2 v)
{
    Vector2 result;
    float length = sqrtf((v.x * v.x) + (v.y * v.y));

    if (length > 0) {
        float ilength = 1.0f / length;
        result.x = v.x * ilength;
        result.y = v.y * ilength;
    }
    return result;
}

}
