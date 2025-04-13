
/*============================================================================

This C source file is part of the SoftFloat IEEE Floating-Point Arithmetic
Package, Release 3e, by John R. Hauser.

Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 The Regents of the
University of California.  All rights reserved.

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
#include "specialize.h"
#include "softfloat.h"
#include <stdlib.h>
#include <stdio.h>

#ifndef MY_F16
float16_t softfloat_subMagsF16( uint_fast16_t uiA, uint_fast16_t uiB )
{
    int_fast8_t expA;
    uint_fast16_t sigA;
    int_fast8_t expB;
    uint_fast16_t sigB;
    int_fast8_t expDiff;
    uint_fast16_t uiZ;
    int_fast16_t sigDiff;
    bool signZ;
    int_fast8_t shiftDist, expZ;
    uint_fast16_t sigZ, sigX, sigY;
    uint_fast32_t sig32Z;
    int_fast8_t roundingMode;
    union ui16_f16 uZ;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    expA = expF16UI( uiA );
    sigA = fracF16UI( uiA );
    expB = expF16UI( uiB );
    sigB = fracF16UI( uiB );
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    // expDiff = -31 ~ 31
    expDiff = expA - expB;
    if ( ! expDiff ) {
        /*--------------------------------------------------------------------
        *--------------------------------------------------------------------*/
        if ( expA == 0x1F ) {
            if ( sigA | sigB ) goto propagateNaN;
            softfloat_raiseFlags( softfloat_flag_invalid );
            uiZ = defaultNaNF16UI;
            goto uiZ;
        }
        // may < 0
        sigDiff = sigA - sigB;
        // A = B
        if ( ! sigDiff ) {
            uiZ =
                packToF16UI(
                    (softfloat_roundingMode == softfloat_round_min), 0, 0 );
            goto uiZ;
        }
        // 1.A - 1.B = 0.X
        if ( expA ) --expA;
        signZ = signF16UI( uiA );
        if ( sigDiff < 0 ) {
            signZ = ! signZ;
            sigDiff = -sigDiff;
        }
        // move 1 to 1.X, e.g. 0x0080, sigDiff = 8-5 = 3, 1 ~ 10
        shiftDist = softfloat_countLeadingZeros16( sigDiff ) - 5;
        expZ = expA - shiftDist;
        // subnormal!!
        if ( expZ < 0 ) {
            shiftDist = expA;
            expZ = 0;
        }
        sigZ = sigDiff<<shiftDist;
        goto pack;
    } else {
        /*--------------------------------------------------------------------
        *--------------------------------------------------------------------*/
        signZ = signF16UI( uiA );
        // B > A, expB = 1 ~ 31, expA = 0 ~ 30
        if ( expDiff < 0 ) {
            /*----------------------------------------------------------------
            *----------------------------------------------------------------*/
            signZ = ! signZ;
            if ( expB == 0x1F ) {
                if ( sigB ) goto propagateNaN;
                uiZ = packToF16UI( signZ, 0x1F, 0 );
                goto uiZ;
            }
            if ( expDiff <= -13 ) {
                uiZ = packToF16UI( signZ, expB, sigB );
                if ( expA | sigA ) goto subEpsilon;
                goto uiZ;
            }
            // expZ = 19 ~ 48
            expZ = expA + 19;
            sigX = sigB | 0x0400;
            sigY = sigA + (expA ? 0x0400 : sigA);
            expDiff = -expDiff;
        // B < A, expA = 1 ~ 31, expB = 0 ~ 30
        } else {
            /*----------------------------------------------------------------
            *----------------------------------------------------------------*/
            uiZ = uiA;
            if ( expA == 0x1F ) {
                if ( sigA ) goto propagateNaN;
                goto uiZ;
            }
            if ( 13 <= expDiff ) {
                if ( expB | sigB ) goto subEpsilon;
                goto uiZ;
            }
            // expZ = 19 ~ 48
            expZ = expB + 19;
            sigX = sigA | 0x0400;
            sigY = sigB + (expB ? 0x0400 : sigB);
        }
        // expDiff = 1 ~ 12
        // 1.X(10)0(expDiff) - 1.Y, > 0
        sig32Z = ((uint_fast32_t) sigX<<expDiff) - sigY;
        // move 1 to 1.X, e.g. 0x0000 0080, sigDiff = 24-1 = 23
        shiftDist = softfloat_countLeadingZeros32( sig32Z ) - 1;
        // move 1 to 1.X, 31.W
        sig32Z <<= shiftDist;
        // expZ - shiftDist may < 0
        expZ -= shiftDist;
        sigZ = sig32Z>>16;
        if ( sig32Z & 0xFFFF ) {
            sigZ |= 1;
        } else {
            if ( ! (sigZ & 0xF) && ((unsigned int) expZ < 0x1E) ) {
                sigZ >>= 4;
                goto pack;
            }
        }
        return softfloat_roundPackToF16( signZ, expZ, sigZ );
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 propagateNaN:
    uiZ = softfloat_propagateNaNF16UI( uiA, uiB );
    goto uiZ;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 subEpsilon:
    roundingMode = softfloat_roundingMode;
    if ( roundingMode != softfloat_round_near_even ) {
        if (
            (roundingMode == softfloat_round_minMag)
                || (roundingMode
                        == (signF16UI( uiZ ) ? softfloat_round_max
                                : softfloat_round_min))
        ) {
            --uiZ;
        }
#ifdef SOFTFLOAT_ROUND_ODD
        else if ( roundingMode == softfloat_round_odd ) {
            uiZ = (uiZ - 1) | 1;
        }
#endif
    }
    softfloat_exceptionFlags |= softfloat_flag_inexact;
    goto uiZ;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 pack:
    uiZ = packToF16UI( signZ, expZ, sigZ );
 uiZ:
    uZ.ui = uiZ;
    return uZ.f;

}

#else
float16_t softfloat_subMagsF16( uint_fast16_t uiA, uint_fast16_t uiB )
{
    int_fast8_t expA;
    uint_fast16_t sigA;
    int_fast8_t expB;
    uint_fast16_t sigB;
    int_fast8_t expDiff;
    uint_fast16_t uiZ;
    int_fast16_t sigDiff;
    bool signZ;
    int_fast8_t shiftDist, expZ;
    uint_fast16_t sigZ, sigX, sigY;
    uint_fast32_t sig32Z;
    // int_fast8_t roundingMode;
    union ui16_f16 uZ;
    int_fast8_t expZ_1;
    bool subnormal;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    expA = expF16UI( uiA );
    sigA = fracF16UI( uiA );
    expB = expF16UI( uiB );
    sigB = fracF16UI( uiB );
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    // expDiff = -31 ~ 31
    // expDiff = expA - expB;
    expDiff = (expA >= expB) ? expA - expB : expB - expA;
    bool aGTb = expA >= expB;
    if ( ! expDiff ) {
        /*--------------------------------------------------------------------
        *--------------------------------------------------------------------*/
        if ( expA == 0x1F ) {
#ifdef INPUT_SUBNORMAL_CHECK
            printf("sub input NaN or inf\n");
#endif
            exit(16);
        }
        // may < 0
        // sigDiff = sigA - sigB;
        sigDiff = (sigA >= sigB) ? (sigA - sigB) : (sigB - sigA);
        // printf("sigDiff = %x\n", sigDiff);
        bool sigAgtB = sigA >= sigB;
        // A = B
        if ( ! sigDiff ) {
            uiZ =
                packToF16UI(
                    0, 0, 0 );
            goto uiZ;
        }
        // 1.A - 1.B = 0.X
        if ( expA ) --expA;
        // printf("expA = %x\n", expA);
        signZ = signF16UI( uiA );
        // if ( sigDiff < 0 ) {
        if ( sigAgtB == 0 ) {
            signZ = ! signZ;
            // sigDiff = -sigDiff;
        }
        // move 1 to 1.X, e.g. 0x0080, sigDiff = 8-5 = 3, 1 ~ 10
        shiftDist = softfloat_countLeadingZeros16( sigDiff ) - 5;
        // printf("shiftDist = %x\n", shiftDist);
        // expZ = expA - shiftDist;
        expZ = (expA >= shiftDist) ? (expA - shiftDist) : (shiftDist - expA);
        // printf("expZ = %x\n", expZ);
        subnormal = !(expA >= shiftDist);
        // printf("subnormal = %x\n", subnormal);
        // if ( expZ < 0 ) {
        if ( subnormal == true) {
            // shiftDist = expA;
            // expZ = 0;
            uiZ = packToF16UI( signZ, 0, 0 );
            goto uiZ;
        }
        sigZ = sigDiff<<shiftDist;
        // printf("sigZ = %x\n", sigZ);
        goto pack;
    } else {
        /*--------------------------------------------------------------------
        *--------------------------------------------------------------------*/
        signZ = signF16UI( uiA );
        // B > A, expB = 1 ~ 31, expA = 0 ~ 30
        // if ( expDiff < 0 ) {
        if ( aGTb == 0 ) {
            /*----------------------------------------------------------------
            *----------------------------------------------------------------*/
            signZ = ! signZ;
            if ( expB == 0x1F ) {
#ifdef INPUT_SUBNORMAL_CHECK
                printf("sub input NaN or inf\n");
#endif
                exit(16);
            }
            // if ( expDiff <= -13 ) {
            if ( expDiff >= 13 ) {
                // uiZ = packToF16UI( signZ, expB, sigB );
                uiZ = uiB;
                goto uiZ;
            }
            // expZ = 19 ~ 48
            expZ = expA + 19;
            sigX = sigB | 0x0400;
            // sigY = sigA | 0x0400;
            // A = 0 case
            sigY = (expA == 0) ? 0 : (sigA | 0x0400);
            // printf("sigX = %x\n", sigX);
            // printf("sigY = %x\n", sigY);
            // printf("expZ = %x\n", expZ);
            // expDiff = -expDiff;
        // B < A, expA = 1 ~ 31, expB = 0 ~ 30
        } else {
            /*----------------------------------------------------------------
            *----------------------------------------------------------------*/
            // uiZ = uiA;
            if ( expA == 0x1F ) {
#ifdef INPUT_SUBNORMAL_CHECK
                printf("sub input NaN or inf\n");
#endif
                exit(16);
            }
            if ( 13 <= expDiff ) {
                uiZ = uiA;
                goto uiZ;
            }
            // expZ = 19 ~ 48
            expZ = expB + 19;
            sigX = sigA | 0x0400;
            // sigY = sigB | 0x0400;
            // B = 0 case
            sigY = (expB == 0) ? 0 : (sigB | 0x0400);
        }
        // expDiff = 1 ~ 12
        // 1.X(10)0(expDiff) - 1.Y, > 0
        sig32Z = ((uint_fast32_t) sigX<<expDiff) - sigY;
        // printf("sig32Z = %x\n", sig32Z);
        // move 1 to 1.X, e.g. 0x0000 0080, sigDiff = 24-1 = 23
        shiftDist = softfloat_countLeadingZeros32( sig32Z ) - 1;
        // printf("shiftDist = %x\n", shiftDist);
        // move 1 to 1.X, 31.W
        sig32Z <<= shiftDist;
        // printf("sig32Z = %x\n", sig32Z);
        // expZ - shiftDist may < 0
        // expZ -= shiftDist;
        expZ_1 = (expZ >= shiftDist) ? (expZ - shiftDist) : (shiftDist - expZ);
        // printf("expZ_1 = %x\n", expZ_1);
        subnormal = !(expZ >= shiftDist);
        // printf("subnormal = %x\n", subnormal);
        sigZ = sig32Z>>16;
        // printf("sigZ = %x\n", sigZ);
        if ( sig32Z & 0xFFFF ) {
            sigZ |= 1;
        } else {
            // if ( ! (sigZ & 0xF) && ((unsigned int) expZ < 0x1E) ) {
            //     sigZ >>= 4;
            //     goto pack;
            // }
        }
        return softfloat_roundPackToF16( signZ, expZ_1, sigZ, subnormal );
    }

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 pack:
    uiZ = packToF16UI( signZ, expZ, sigZ );
 uiZ:
    uZ.ui = uiZ;
    return uZ.f;

}
#endif