#ifndef INPUT_EXTENDED_H
#define INPUT_EXTENDED_H

typedef struct InputVector2DKeys {
    int up, down, left, right;
} InputVector2DKeys;

#if defined(__cplusplus)
extern "C" {            // Prevents name mangling of functions
#endif

//Vector2 GetInputVector2D(int up, int down, int left, int right);
//Vector2 GetInputVector2DStruct(InputVector2DKeys keys);
//int IsInputVector2DPressed(int up, int down, int left, int right);
//int IsInputVector2DPressedStruct(InputVector2DKeys keys);

#if defined(__cplusplus)
}
#endif

#endif /* INPUT_EXTENDED_H */
