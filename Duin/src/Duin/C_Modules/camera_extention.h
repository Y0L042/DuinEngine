#ifndef C_CAMERA_EXTENTION_H
#define C_CAMERA_EXTENTION_H

#include <raylib.h>
#include <raymath.h>
#include <rcamera.h>

#if defined(__cplusplus)
extern "C" {            // Prevents name mangling of functions
#endif

float CalculateCameraYaw(Camera3D *camera);
float CalculateCameraPitch(Camera3D *camera);

#if defined(__cplusplus)
}
#endif


#endif /* C_CAMERA_EXTENTION_H */
