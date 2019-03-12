/*
 *  Copyright (c) 2015, Stefan Johnson
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

#ifndef CommonGameKit_GFXShaderLibrary_h
#define CommonGameKit_GFXShaderLibrary_h

#include <CommonC/Common.h>

typedef enum {
    GFXShaderSourceTypeHeader,
    GFXShaderSourceTypeVertex,
    GFXShaderSourceTypeFragment
} GFXShaderSourceType;

/*!
 * @brief The graphics shader library.
 * @description Allows @b CCRetain.
 */
typedef struct GFXShaderLibrary *GFXShaderLibrary;

/*!
 * @brief The graphics shader source.
 */
typedef struct GFXShaderSource *GFXShaderSource;

/*!
 * @brief Set this to a function that will retrieve the shader library with a certain name.
 * @description This is used by some implementations to allow for referencing of other shaders.
 * @param Name The name of the library to retrieve.
 * @result The shader library, or NULL if there is none. Ownership should be passed to the caller.
 */
extern CC_NEW GFXShaderLibrary (*GFXShaderLibraryForName)(CCString Name);


/*!
 * @brief Create a shader library.
 * @description A library to lookup shader sources from to use to create shaders.
 * @param Allocator The allocator to be used for the allocations.
 * @return The created shader library.
 */
CC_NEW GFXShaderLibrary GFXShaderLibraryCreate(CCAllocatorType Allocator);

/*!
 * @brief Create a shader library from a precompiled library.
 * @description A library to lookup shader sources from to use to create shaders.
 * @param Allocator The allocator to be used for the allocations.
 * @param Data The precompiled library.
 * @return The created shader library.
 */
CC_NEW GFXShaderLibrary GFXShaderLibraryCreateFromPrecompiled(CCAllocatorType Allocator, CCData Data);

/*!
 * @brief Destroy a shader library.
 * @param Library The shader library to be destroyed.
 */
void GFXShaderLibraryDestroy(GFXShaderLibrary CC_DESTROY(Library));

/*!
 * @brief Compile a shader.
 * @param Library The shader library to compile the shader.
 * @param Type The shader source type.
 * @param Name The name to reference the compiled shader source.
 * @param Source The sourcecode for the shader.
 * @return The compiled shader source.
 */
const GFXShaderSource GFXShaderLibraryCompile(GFXShaderLibrary Library, GFXShaderSourceType Type, CCString CC_COPY(Name), const char *Source);

/*!
 * @brief Lookup a compiled shader.
 * @param Library The shader library to lookup the shader in.
 * @param Name The name of the compiled shader source.
 * @return The compiled shader source.
 */
const GFXShaderSource GFXShaderLibraryGetSource(GFXShaderLibrary Library, CCString Name);

#endif
