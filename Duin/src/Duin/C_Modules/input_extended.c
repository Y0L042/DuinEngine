#include "input_extended.h"

void GrabCursor()
{
}

void ReleaseCursor()
{
}

// Vector2 GetInputVector2D(int up, int down, int left, int right)
//{
//     int U, D, L, R = 0;
//     float x, y = 0.0f;
//
//     U = IsKeyDown(up);
//     D = IsKeyDown(down);
//     L = IsKeyDown(left);
//     R = IsKeyDown(right);
//     x = R - L;
//     y = U - D;
//
//     return Vector2Normalize((Vector2){ (float)x, -(float)y });
// }
//
// Vector2 GetInputVector2DStruct(InputVector2DKeys keys)
//{
//     int U, D, L, R = 0;
//     float x, y = 0.0f;
//
//     U = IsKeyDown(keys.up);
//     D = IsKeyDown(keys.down);
//     L = IsKeyDown(keys.left);
//     R = IsKeyDown(keys.right);
//     x = R - L;
//     y = U - D;
//
//     return Vector2Normalize((Vector2){ (float)x, -(float)y });
// }

int IsInputVector2DPressed(int up, int down, int left, int right)
{
    int U, D, L, R;
    U = 0;
    D = 0;
    L = 0;
    R = 0;

    // U = IsKeyDown(up);
    // D = IsKeyDown(down);
    // L = IsKeyDown(left);
    // R = IsKeyDown(right);

    return (U || D || L || R);
}

int IsInputVector2DPressedStruct(InputVector2DKeys keys)
{
    int U, D, L, R = 0;

    // U = IsKeyDown(keys.up);
    // D = IsKeyDown(keys.down);
    // L = IsKeyDown(keys.left);
    // R = IsKeyDown(keys.right);

    return (U || D || L || R);
}
