
/*============================================================================

This C source file is part of the SoftFloat IEEE Floating-Point Arithmetic
Package, Release 3e, by John R. Hauser.

Copyright 2011, 2012, 2013, 2014, 2015, 2017 The Regents of the University of
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
#include <stdlib.h>

#ifndef MY_F16
float16_t
 softfloat_roundPackToF16( bool sign, int_fast16_t exp, uint_fast16_t sig )
{
    uint_fast8_t roundingMode;
    bool roundNearEven;
    uint_fast8_t roundIncrement, roundBits;
    bool isTiny;
    uint_fast16_t uiZ;
    union ui16_f16 uZ;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    // roundingMode = 0
    roundingMode = softfloat_roundingMode;
    // true
    roundNearEven = (roundingMode == softfloat_round_near_even);
    roundIncrement = 0x8;
    // roundIncrement = 0x8
    if ( ! roundNearEven && (roundingMode != softfloat_round_near_maxMag) ) {
        roundIncrement =
            (roundingMode
                 == (sign ? softfloat_round_min : softfloat_round_max))
                ? 0xF
                : 0;
    }
    // LSB 4bits
    roundBits = sig & 0xF;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    // mul:
    // exp = -14 ~ 45
    // exp < 0 / exp >= 29
    // add:
    // exp = ?
    if ( 0x1D <= (unsigned int) exp ) {
        if ( exp < 0 ) {
            // printf("0x1D <= (unsigned int) exp, exp < 0\n");
            /*----------------------------------------------------------------
            *----------------------------------------------------------------*/
            isTiny =
                (softfloat_detectTininess == softfloat_tininess_beforeRounding)
                    || (exp < -1) || (sig + roundIncrement < 0x8000); // ARM 1/8086 0 || (exp < -1) || (sig + 8 < 0x8000)
            // mul:
            // -exp = 1 ~ 14
            sig = softfloat_shiftRightJam32( sig, -exp );
            exp = 0;
            roundBits = sig & 0xF;
            if ( isTiny && roundBits ) {
                softfloat_raiseFlags( softfloat_flag_underflow );
            }
        // too big number, no need
        // mul:
        // exp = 30 ~ 45 || exp = 29 but carry
        // add:
        // exp = ?
        } else if ( (0x1D < exp) || (0x8000 <= sig + roundIncrement) ) {
            printf("(0x1D < exp) || (0x8000 <= sig + roundIncrement)\n");
            /*----------------------------------------------------------------
            *----------------------------------------------------------------*/
            softfloat_raiseFlags(
                softfloat_flag_overflow | softfloat_flag_inexact );
            uiZ = packToF16UI( sign, 0x1F, 0 ) - ! roundIncrement;
            goto uiZ;
        }
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    sig = (sig + roundIncrement)>>4;
    if ( roundBits ) {
        softfloat_exceptionFlags |= softfloat_flag_inexact;
#ifdef SOFTFLOAT_ROUND_ODD
        if ( roundingMode == softfloat_round_odd ) {
            sig |= 1;
            goto packReturn;
        }
#endif
    }
    sig &= ~(uint_fast16_t) (! (roundBits ^ 8) & roundNearEven);
    if ( ! sig ) {
        printf("special: exp: %x sig: %x\n", exp, sig);
        exp = 0;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 packReturn:
    uiZ = packToF16UI( sign, exp, sig );
 uiZ:
    uZ.ui = uiZ;
    return uZ.f;

}

#else
float16_t
 softfloat_roundPackToF16( bool sign, int_fast16_t exp, uint_fast16_t sig, bool subnormal)
{
    uint_fast8_t roundingMode;
    bool roundNearEven;
    uint_fast8_t roundIncrement, roundBits;
    // bool isTiny;
    uint_fast16_t uiZ;
    union ui16_f16 uZ;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    // roundingMode = 0
    // roundingMode = softfloat_roundingMode;
    // true
    // roundNearEven = 1;
    roundIncrement = 0x8;
    // LSB 4bits
    roundBits = sig & 0xF;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    // mul:
    // exp = -14 ~ 45
    // exp < 0 / exp >= 29
    // add:
    // exp = ?
    // if ( 0x1D <= (unsigned int) exp ) {
    if (( 0x1D <= exp )||(subnormal == true)) {
        // if ( exp < 0 ) {
        if ( subnormal == true) {
            // printf("0x1D <= (unsigned int) exp, exp < 0\n");
            /*----------------------------------------------------------------
            *----------------------------------------------------------------*/
            uiZ = packToF16UI( sign, 0, 0 );
            goto uiZ;
        // too big number, no need
        // mul:
        // exp = 30 ~ 45 || exp = 29 but carry
        // add:
        // exp = ?
        } else if ( (0x1D < exp) || (0x8000 <= sig + roundIncrement) ) {
            printf("(0x1D < exp) || (0x8000 <= sig + roundIncrement)\n");
            printf("exp: %d sig: %x\n", exp, sig);
            exit(16);
        }
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    sig = (sig + roundIncrement)>>4;

    // sig &= ~(uint_fast16_t) (! (roundBits ^ 8) & roundNearEven);
    sig &= ~(uint_fast16_t) (! (roundBits ^ 8));
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 packReturn:
    uiZ = packToF16UI( sign, exp, sig );
 uiZ:
    uZ.ui = uiZ;
    return uZ.f;

}
#endif

