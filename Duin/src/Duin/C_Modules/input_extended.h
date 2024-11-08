#ifndef INPUT_EXTENDED_H
#define INPUT_EXTENDED_H

#include <raylib.h>
#include <raymath.h>

#if defined(__cplusplus)
extern "C" {            // Prevents name mangling of functions
#endif

Vector2 GetInputVector2D(int up, int down, int left, int right);

#if defined(__cplusplus)
}
#endif

#endif /* INPUT_EXTENDED_H */
