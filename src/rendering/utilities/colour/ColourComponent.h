/*
 *  Copyright (c) 2015 Stefan Johnson
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice, this list
 *     of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice, this
 *     list of conditions and the following disclaimer in the documentation and/or other
 *     materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef Blob_Game_ColourComponent_h
#define Blob_Game_ColourComponent_h

#include "ColourFormat.h"
#include <CommonC/Extensions.h>

typedef struct {
    CCColourFormat type;
    union {
        float f32;
        double f64;
        
        int8_t i8;
        int16_t i16;
        int32_t i32;
        int64_t i64;
        
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
        uint64_t u64;
    };
} CCColourComponent;


/*!
 * @brief Get the bit size of a colour component.
 * @param Component The colour component.
 * @return The bit size of the component.
 */
unsigned int CCColourComponentGetBitSize(CCColourComponent Component) CC_CONSTANT_FUNCTION;

/*!
 * @brief Do a linear precision conversion to convert the component from one type to
 *        the new type.
 *
 * @param Component The colour component.
 * @param OldType The old type of the component.
 *        @b CCColourFormatTypeUnsignedInteger
 *        @b CCColourFormatTypeSignedInteger
 *        @b CCColourFormatTypeFloat
 *
 * @param NewType The new type of the component.
 *        @b CCColourFormatTypeUnsignedInteger
 *        @b CCColourFormatTypeSignedInteger
 *        @b CCColourFormatTypeFloat
 *
 * @param NewPrecision The precision for the new type.
 * @return The converted colour component.
 */
CCColourComponent CCColourComponentLinearPrecisionConversion(CCColourComponent Component, CCColourFormat OldType, CCColourFormat NewType, int NewPrecision) CC_CONSTANT_FUNCTION;

#endif
