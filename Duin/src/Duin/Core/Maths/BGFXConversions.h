#pragma once

#include "DuinMaths.h"

#include <bx/math.h>
#include <bgfx/bgfx.h>

namespace duin {
    /**
     * Pass initialized array of size 16 (4x4)
     */
    inline void MatrixAsArray(Matrix m, float *r)
    {
        // Transpose the row-major Raymath matrix to column-major format for bgfx
        r[0]  = m.m0;  r[1]  = m.m4;  r[2]  = m.m8;  r[3]  = m.m12;
        r[4]  = m.m1;  r[5]  = m.m5;  r[6]  = m.m9;  r[7]  = m.m13;
        r[8]  = m.m2;  r[9]  = m.m6;  r[10] = m.m10; r[11] = m.m14;
        r[12] = m.m3;  r[13] = m.m7;  r[14] = m.m11; r[15] = m.m15;
    }
}
