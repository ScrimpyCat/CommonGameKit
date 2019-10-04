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

#define CC_QUICK_COMPILE
#include "PixelDataGenerator.h"

typedef struct {
    CCPixelDataGeneratorFunction generator;
} CCPixelDataGeneratorInternal;

static void *CCPixelDataGeneratorConstructor(CCAllocatorType Allocator);
static void CCPixelDataGeneratorDestructor(CCPixelDataGeneratorInternal *Internal);
static CCColour CCPixelDataGeneratorGetColour(CCPixelData Pixels, size_t x, size_t y, size_t z);

const CCPixelDataInterface CCPixelDataGeneratorInterface = {
    .create = CCPixelDataGeneratorConstructor,
    .destroy = (CCPixelDataDestructorCallback)CCPixelDataGeneratorDestructor,
    .colour = CCPixelDataGeneratorGetColour,
    .optional = {
        .size = NULL,
        .packedData = NULL
    }
};

const CCPixelDataInterface * const CCPixelDataGenerator = &CCPixelDataGeneratorInterface;


static void *CCPixelDataGeneratorConstructor(CCAllocatorType Allocator)
{
    CCPixelDataGeneratorInternal *Internal = CCMalloc(Allocator, sizeof(CCPixelDataGeneratorInternal), NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (Internal)
    {
        *Internal = (CCPixelDataGeneratorInternal){
            .generator = NULL
        };
    }
    
    else
    {
        CC_LOG_ERROR("Failed to create generator pixel data: Failed to allocate memory of size (%zu)", sizeof(CCPixelDataGeneratorInternal));
    }
    
    return Internal;
}

static void CCPixelDataGeneratorDestructor(CCPixelDataGeneratorInternal *Internal)
{
    CCFree(Internal);
}

static CCColour CCPixelDataGeneratorGetColour(CCPixelData Pixels, size_t x, size_t y, size_t z)
{
    const CCPixelDataGeneratorFunction Generator = ((CCPixelDataGeneratorInternal*)Pixels->internal)->generator;
    
    return CCColourConversion(Generator ? Generator(x, y, z) : (CCColour){ .type = 0 }, Pixels->format);
}

CCPixelData CCPixelDataGeneratorCreate(CCAllocatorType Allocator, CCPixelDataGeneratorFunction Func, CCColourFormat Format)
{
    CCPixelData Pixels = CCPixelDataCreate(Allocator, Format, CCPixelDataGenerator);
    
    ((CCPixelDataGeneratorInternal*)Pixels->internal)->generator = Func;
    
    return Pixels;
}
