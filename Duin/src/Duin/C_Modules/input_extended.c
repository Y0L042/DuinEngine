#include "input_extended.h"

Vector2 GetInputVector2D(int up, int down, int left, int right)
{
    int U, D, L, R = 0;
    float x, y = 0.0f;

    U = IsKeyDown(up);
    D = IsKeyDown(down);
    L = IsKeyDown(left);
    R = IsKeyDown(right);
    x = R - L;
    y = U - D;
    
    return Vector2Normalize((Vector2){ (float)x, -(float)y });
}
