#pragma once

#include <cmath>

namespace Duin
{
	struct Vector3
	{
        float x, y, z;

        Vector3() 
            : x(0), y(0), z(0) 
        {}

        Vector3(float x, float y, float z) 
            : x(x), y(y), z(z) 
        {}

        Vector3 operator+(const Vector3& other) const        
        {
            return Vector3(x + other.x, y + other.y, z + other.z);
        }

        Vector3 operator-(const Vector3& other) const 
        {
            return Vector3(x - other.x, y - other.y, z - other.z);
        }

        Vector3 operator*(float scalar) const 
        {
            return Vector3(x * scalar, y * scalar, z * scalar);
        }

        Vector3 operator/(float scalar) const 
        {
            return Vector3(x / scalar, y / scalar, z / scalar);
        }

        float Dot(const Vector3& other) const 
        {
            return x * other.x + y * other.y + z * other.z;
        }

        Vector3 Cross(const Vector3& other) const 
        {
            return Vector3(
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
            );
        }

        float Magnitude() const 
        {
            return sqrt(x * x + y * y + z * z);
        }

        Vector3 Normalized() const 
        {
            float mag = Magnitude();
            return Vector3(x / mag, y / mag, z / mag);
        }
	};
}