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

#ifndef Blob_Game_GFXShader_h
#define Blob_Game_GFXShader_h

#include "GFXShaderLibrary.h"
#include <CommonC/Common.h>


typedef struct GFXShaderInput *GFXShaderInput;

typedef struct GFXShader *GFXShader;

/*!
 * @brief Create a shader.
 * @param Allocator The allocator to be used for the allocations.
 * @param Vertex The vertex source.
 * @param Fragment The fragment source.
 * @return The created shader.
 */
GFXShader GFXShaderCreate(CCAllocatorType Allocator, GFXShaderSource Vertex, GFXShaderSource Fragment);

/*!
 * @brief Destroy a shader.
 * @param Shader The shader to be destroyed.
 */
void GFXShaderDestroy(GFXShader Shader);

/*!
 * @brief Get the input handle for name.
 * @param Shader The shader to get the input to.
 * @param Name The name of the input.
 * @return The shader input.
 */
GFXShaderInput GFXShaderGetInput(GFXShader Shader, const char *Name);

#endif
