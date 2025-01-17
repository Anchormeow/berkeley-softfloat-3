
/*============================================================================

This C source file is part of the SoftFloat IEEE Floating-Point Arithmetic
Package, Release 3e, by John R. Hauser.

Copyright 2011, 2012, 2013, 2014, 2015, 2016 The Regents of the University of
California.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
    this list of conditions, and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions, and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

 3. Neither the name of the University nor the names of its contributors may
    be used to endorse or promote products derived from this software without
    specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS", AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ARE
DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=============================================================================*/

#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "softfloat.h"

#include <stdio.h>

#ifndef MY_F32
float32_t f32_add( float32_t a, float32_t b )
{
    union ui32_f32 uA;
    uint_fast32_t uiA;
    union ui32_f32 uB;
    uint_fast32_t uiB;
#if ! defined INLINE_LEVEL || (INLINE_LEVEL < 1)
    float32_t (*magsFuncPtr)( uint_fast32_t, uint_fast32_t );
#endif

    uA.f = a;
    uiA = uA.ui;
    uB.f = b;
    uiB = uB.ui;
#if defined INLINE_LEVEL && (1 <= INLINE_LEVEL)
    if ( signF32UI( uiA ^ uiB ) ) {
        return softfloat_subMagsF32( uiA, uiB );
    } else {
        return softfloat_addMagsF32( uiA, uiB );
    }
#else
    magsFuncPtr =
        signF32UI( uiA ^ uiB ) ? softfloat_subMagsF32 : softfloat_addMagsF32;
    return (*magsFuncPtr)( uiA, uiB );
#endif

}

#else

uint_fast32_t log2floor(uint_fast32_t value) {
    int result = 0;
    while (value >>= 1) {  // Shift right until value becomes 0
        result++;
    }
    return (uint_fast32_t)(result);
}

float32_t f32_add( float32_t a, float32_t b )
{
    union ui32_f32 uA;
    uint_fast32_t uiA;
    bool signA;
    int_fast16_t expA;
    uint_fast32_t sigA;
    union ui32_f32 uB;
    uint_fast32_t uiB;
    bool signB;
    int_fast16_t expB;
    uint_fast32_t sigB;
    bool signZ;
    int_fast16_t expZ;
    uint_fast32_t sigZ, uiZ;
    union ui32_f32 uZ;

    uA.f = a;
    uiA = uA.ui;
    signA = signF32UI( uiA );
    expA  = expF32UI( uiA );
    sigA  = fracF32UI( uiA );
    uB.f = b;
    uiB = uB.ui;
    signB = signF32UI( uiB );
    expB  = expF32UI( uiB );
    sigB  = fracF32UI( uiB );

    // p0
    bool y_gt_x_0 = (b.v & 0x7FFFFFFF) > (a.v & 0x7FFFFFFF); // 31 bits
    bool zs_0 = y_gt_x_0 ? signB : signA;
    uint_fast16_t ze_0 = y_gt_x_0 ? expB : expA;
    bool ye0 = expB != 0;
    bool xe0 = expA != 0; 
    uint_fast32_t xf1_0 = y_gt_x_0 ? (sigB | (ye0 ? 0x00800000 : 0)) : (sigA | (xe0 ? 0x00800000 : 0)); // 23+1 bits, bigger one
    uint_fast32_t yf1_0 = y_gt_x_0 ? (sigA | (xe0 ? 0x00800000 : 0)) : (sigB | (ye0 ? 0x00800000 : 0)); // 23+1 bits, smaller one
    uint_fast16_t xe_ye_0 = (y_gt_x_0 ? expB : expA) - (y_gt_x_0 ? expA : expB); // 8 bits, >0
    bool xs_ys_a = signA == signB;

    uint_fast16_t xe_ye_reduced = xe_ye_0 & 0x1F; // 5 bits
    bool xe_ye_32 = ((xe_ye_0 >> 5) & 0x07) != 0x00;
    uint_fast32_t yf2_a0 = xe_ye_32 ? 0x00000000 : yf1_0;
    uint_fast32_t yf2_a1 = ((xe_ye_reduced >> 4) & 0x01) ? (yf2_a0 >> 16) : yf2_a0;
    uint_fast32_t yf2_a2 = ((xe_ye_reduced >> 3) & 0x01) ? (yf2_a1 >> 8) : yf2_a1;
    uint_fast32_t yf2_a3 = ((xe_ye_reduced >> 2) & 0x01) ? (yf2_a2 >> 4) : yf2_a2;
    uint_fast32_t yf2_a4 = ((xe_ye_reduced >> 1) & 0x01) ? (yf2_a3 >> 2) : yf2_a3;
    uint_fast32_t yf2_a5 = (xe_ye_reduced & 0x01) ? (yf2_a4 >> 1) : yf2_a4;

    // p1
    uint_fast32_t zf_1 = xs_ys_a ? (xf1_0 + yf2_a5) : (xf1_0 - yf2_a5); // sign bit

    // p2
    uint_fast32_t cal0 = (zf_1 >> 1) & 0x007FFFFF; // 23 bits
    uint_fast32_t offset2 = 0x16 - log2floor(cal0); // 22 - 5bits

    // p3
    bool underflow_3 = ze_0 < offset2;
    bool shift_right_3 = ((zf_1 >> 24) & 0x00000001);
    uint_fast32_t ze1_3;
    if (underflow_3) {
        ze1_3 = 0x00000000;
    } else if (shift_right_3) {
        ze1_3 = ze_0 + 0x00000001;
    } else {
        ze1_3 = ze_0 - offset2;
    }
    uint_fast32_t zf1_3;
    if (underflow_3) {
        zf1_3 = 0x00000000;
    } else if (shift_right_3) {
        zf1_3 = (zf_1 >> 1) & 0x007FFFFF; // 23 bits
    } else {
        zf1_3 = (zf_1 << offset2) & 0x007FFFFF; // 23 bits
    }
    uiZ = packToF32UI(zs_0, ze1_3, zf1_3);
    uZ.ui = uiZ;
    return uZ.f;
}
#endif