#pragma once

namespace Duin {

static inline float QuakeRoot(float x)
{
    /*
    * Store floating-point x in an integer.
    * Take an initial guess for Newton's method.
    * Convert answer into a float.
    * Do one round of Newton's method.
    * return x.
    */
    float xHalf = 0.5f * x;
    int i = *(int *)&x;
    i = 0x5F3759DF - (i >> 1);
    x = *(float *)&i;
    x = x * (1.5f - (xHalf * x * x));
    return x;
}

}
