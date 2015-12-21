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

#ifndef Blob_Game_GFXDraw_h
#define Blob_Game_GFXDraw_h

#include "GFXBuffer.h"
#include "GFXShader.h"
#include "GFXTexture.h"
#include <CommonC/Common.h>


typedef enum {
    GFXPrimitiveTypePoint,
    GFXPrimitiveTypeLine,
    GFXPrimitiveTypeTriangle,
    GFXPrimitiveTypeTriangleStrip,
    GFXPrimitiveTypeTriangleFan
} GFXPrimitiveType;

typedef struct {
    char *name;
    GFXShaderInput shaderInput;
} GFXDrawInput;

typedef struct {
    GFXDrawInput input;
    GFXBuffer buffer;
    size_t stride;
    size_t size;
    GFXBufferFormat format;
} GFXDrawInputVertexBuffer;

typedef struct {
    GFXDrawInput input;
    GFXBuffer buffer;
} GFXDrawInputBuffer;

typedef struct {
    GFXDrawInput input;
    GFXTexture texture;
} GFXDrawInputTexture;

typedef struct {
    GFXBuffer buffer;
    GFXBufferFormat format;
} GFXDrawIndexBuffer;

typedef struct {
    void *internal;
    GFXShader shader;
    CCCollection vertexBuffers;
    CCCollection buffers;
    CCCollection textures;
    GFXDrawIndexBuffer index;
} GFXDrawInfo, *GFXDraw;

/*!
 * @brief Create a draw operation.
 * @param Allocator The allocator to be used for the allocations.
 * @return The created draw operation.
 */
GFXDraw GFXDrawCreate(CCAllocatorType Allocator);

/*!
 * @brief Destroy a draw operation.
 * @param Draw The draw operation to be destroyed.
 */
void GFXDrawDestroy(GFXDraw Draw);

/*!
 * @brief Submit the draw command.
 * @param Draw The draw operation.
 * @param Primitive The primitive type to be rendered.
 * @param Offset The offset of the first vertex.
 * @param Count The number of vertices to be drawn.
 */
void GFXDrawSubmit(GFXDraw Draw, GFXPrimitiveType Primitive, size_t Offset, size_t Count);

/*!
 * @brief Submit the draw command using an index buffer to draw.
 * @param Draw The draw operation.
 * @param Primitive The primitive type to be rendered.
 * @param Offset The offset to start drawing from.
 * @param Count The number of indices to be used.
 */
void GFXDrawSubmitIndexed(GFXDraw Draw, GFXPrimitiveType Primitive, size_t Offset, size_t Count);

/*!
 * @brief Set the shader to be used for the draw command.
 * @param Draw The draw operation.
 * @param Shader The shader to be used for the draw operation.
 */
void GFXDrawSetShader(GFXDraw Draw, GFXShader Shader);

/*!
 * @brief Set the index buffer of the draw command.
 * @param Draw The draw operation.
 * @param Indexes The index buffer of type @b GFXBufferHintDataIndex or @b GFXBufferHintData.
 * @param Format The format of the data.
 */
void GFXDrawSetIndexBuffer(GFXDraw Draw, GFXBuffer Indexes, GFXBufferFormat Format);

/*!
 * @brief Set the vertex attributes of the draw command.
 * @param Draw The draw operation.
 * @param Input The input name from in the shader.
 * @param Buffer The vertex buffer.
 * @param Format The format of the data.
 * @param Stride The stride of the data.
 * @param Size The size of the data.
 */
void GFXDrawSetVertexBuffer(GFXDraw Draw, const char *Input, GFXBuffer Buffer, GFXBufferFormat Format, size_t Stride, size_t Size);

/*!
 * @brief Set the uniform of the draw command.
 * @param Draw The draw operation.
 * @param Input The input name from in the shader.
 * @param Buffer The buffer.
 */
void GFXDrawSetBuffer(GFXDraw Draw, const char *Input, GFXBuffer Buffer);

/*!
 * @brief Set the texture of the draw command.
 * @param Draw The draw operation.
 * @param Input The input name from in the shader.
 * @param Texture The texture.
 */
void GFXDrawSetTexture(GFXDraw Draw, const char *Input, GFXTexture Texture);

#endif
