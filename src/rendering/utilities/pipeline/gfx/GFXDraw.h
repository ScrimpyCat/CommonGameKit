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

#ifndef CommonGameKit_GFXDraw_h
#define CommonGameKit_GFXDraw_h

#include <CommonGameKit/Base.h>
#include <CommonGameKit/GFXBuffer.h>
#include <CommonGameKit/GFXShader.h>
#include <CommonGameKit/GFXTexture.h>
#include <CommonGameKit/GFXFramebuffer.h>
#include <CommonGameKit/GFXBlend.h>
#include <CommonGameKit/GFXViewport.h>
#include <CommonGameKit/GFXDepth.h>
#include <CommonGameKit/GFXStencil.h>


typedef enum {
    GFXPrimitiveTypePoint,
    GFXPrimitiveTypeLine,
    GFXPrimitiveTypeLineStrip,
    GFXPrimitiveTypeTriangle,
    GFXPrimitiveTypeTriangleStrip
} GFXPrimitiveType;

typedef enum {
    GFXRasterModeFill = 0,
    GFXRasterModeLines = 1
} GFXRasterMode;

typedef enum {
    GFXCullModeNone = 0,
    GFXCullModeFront = 1,
    GFXCullModeBack = 2
} GFXCullMode;

typedef enum {
    GFXWindingClockwise = 0,
    GFXWindingCounterClockwise = 1
} GFXWinding;

typedef enum {
    GFXRenderBehaviourRasterMode = 0,
    GFXRenderBehaviourCullMode = 1,
    GFXRenderBehaviourFrontFaceWinding = 3,
    
    GFXRenderBehaviourRasterModeMask = 1,
    GFXRenderBehaviourRasterModeFill = GFXRasterModeFill,
    GFXRenderBehaviourRasterModeLines = GFXRasterModeLines,
    
    GFXRenderBehaviourCullModeMask = 3,
    GFXRenderBehaviourCullModeNone = GFXCullModeNone,
    GFXRenderBehaviourCullModeFront = GFXCullModeFront,
    GFXRenderBehaviourCullModeBack = GFXCullModeBack,
    
    GFXRenderBehaviourFrontFaceWindingMask = 1,
    GFXRenderBehaviourFrontFaceWindingClockwise = GFXWindingClockwise,
    GFXRenderBehaviourFrontFaceWindingCounterClockwise = GFXWindingCounterClockwise,
    
    GFXRenderBehaviourDefault = (GFXRenderBehaviourRasterModeFill << GFXRenderBehaviourRasterMode) | (GFXCullModeNone << GFXRenderBehaviourCullMode) | (GFXRenderBehaviourFrontFaceWindingClockwise << GFXRenderBehaviourFrontFaceWinding)
} GFXRenderBehaviour;

typedef enum {
    GFXDepthClipModeClip = 0,
    GFXDepthClipModeClamp = 1
} GFXDepthClipMode;

typedef struct {
    CCString name;
    GFXShaderInput shaderInput;
} GFXDrawInput;

typedef struct {
    GFXDrawInput input;
    GFXBuffer buffer;
    size_t stride;
    size_t offset;
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
    size_t index;
    GFXBlend blending;
} GFXDrawDestination;

typedef struct {
    size_t index;
    GFXDepth config;
    GFXDepthClipMode clipMode;
    struct {
        _Bool enable;
        float bias;
        float slope;
        float clamp;
    } adjustment;
} GFXDrawDepth;

typedef struct {
    size_t index;
    GFXStencil config;
    struct {
        struct {
            uint32_t read, write;
        } mask;
        uint32_t reference;
    } front, back;
} GFXDrawStencil;

typedef struct {
    size_t count;
    struct {
        size_t x, y, width, height;
    } rects[];
} GFXDrawScissor;

typedef struct {
    void *internal;
    GFXShader shader;
    CCCollection(GFXDrawInputVertexBuffer) vertexBuffers;
    CCCollection(GFXDrawInputBuffer) buffers;
    CCCollection(GFXDrawInputTexture) textures;
    GFXRenderBehaviour renderBehaviour;
    GFXDrawDepth depth;
    GFXDrawStencil stencil;
    GFXDrawScissor *scissor;
    GFXDrawIndexBuffer index;
    GFXFramebuffer framebuffer;
    struct {
        GFXViewport bounds;
        _Bool normalized;
    } viewport;
    GFXDrawDestination destination[8];
} GFXDrawInfo;

/*!
 * @brief The graphics draw operation.
 * @description Allows @b CCRetain.
 */
typedef GFXDrawInfo *GFXDraw;

/*!
 * @brief Create a draw operation.
 * @description To commit a draw it must be submitted either using @b GFXDrawSubmit
 *              or @b GFXDrawSubmitIndexed. One draw command can be reused and
 *              submitted as many times as needed. State of a draw command can also
 *              be changed before each submit.
 *
 * @param Allocator The allocator to be used for the allocations.
 * @return The created draw operation.
 */
CC_NEW GFXDraw GFXDrawCreate(CCAllocatorType Allocator);

/*!
 * @brief Destroy a draw operation.
 * @param Draw The draw operation to be destroyed.
 */
void GFXDrawDestroy(GFXDraw CC_DESTROY(Draw));

/*!
 * @brief Submit the draw command.
 * @param Draw The draw operation.
 * @param Primitive The primitive type to be rendered.
 * @param Offset The offset of the first vertex.
 * @param Count The number of vertices to be drawn.
 */
void GFXDrawSubmit(GFXDraw Draw, GFXPrimitiveType Primitive, size_t Offset, size_t Count);

/*!
 * @brief Submit the draw command.
 * @param Draw The draw operation.
 * @param Primitive The primitive type to be rendered.
 * @param Offset The offset of the first vertex.
 * @param Count The number of vertices to be drawn.
 * @param Instances The number of instances to be drawn.
 */
void GFXDrawSubmitInstances(GFXDraw Draw, GFXPrimitiveType Primitive, size_t Offset, size_t Count, size_t Instances);

/*!
 * @brief Submit the draw command using an index buffer to draw.
 * @param Draw The draw operation.
 * @param Primitive The primitive type to be rendered.
 * @param Offset The offset to start drawing from.
 * @param Count The number of indices to be used.
 */
void GFXDrawSubmitIndexed(GFXDraw Draw, GFXPrimitiveType Primitive, size_t Offset, size_t Count);

/*!
 * @brief Submit the draw command using an index buffer to draw.
 * @param Draw The draw operation.
 * @param Primitive The primitive type to be rendered.
 * @param Offset The offset to start drawing from.
 * @param Count The number of indices to be used.
 * @param Instances The number of instances to be drawn.
 */
void GFXDrawSubmitIndexedInstances(GFXDraw Draw, GFXPrimitiveType Primitive, size_t Offset, size_t Count, size_t Instances);

/*!
 * @brief Set the shader to be used for the draw command.
 * @param Draw The draw operation.
 * @param Shader The shader to be used for the draw operation. Retains a reference to
 *        the shader.
 */
void GFXDrawSetShader(GFXDraw Draw, GFXShader CC_RETAIN(Shader));

/*!
 * @brief Set the framebuffer to be used for the draw command.
 * @param Draw The draw operation.
 * @param Framebuffer The framebuffer to be used as the destination. Retains a reference
 *        to the framebuffer.
 *
 * @param Index The attachment of that framebuffer to be used as the destination.
 */
void GFXDrawSetFramebuffer(GFXDraw Draw, GFXFramebuffer CC_RETAIN(Framebuffer), size_t Index);

/*!
 * @brief Set the framebuffers to be used for the draw command.
 * @param Draw The draw operation.
 * @param Framebuffer The framebuffer to be used as the destination. Retains a reference
 *        to the framebuffer.
 *
 * @param Indexes The attachments of that framebuffer to be used as the destinations.
 * @param Count The attachments of that framebuffer to be used as the destinations.
 */
void GFXDrawSetFramebuffers(GFXDraw Draw, GFXFramebuffer CC_RETAIN(Framebuffer), size_t *Indexes, size_t Count);

/*!
 * @brief Set the index buffer of the draw command.
 * @param Draw The draw operation.
 * @param Indexes The index buffer of type @b GFXBufferHintDataIndex. Retains a reference
 *        to the index buffer.
 *
 * @param Format The format of the data.
 */
void GFXDrawSetIndexBuffer(GFXDraw Draw, GFXBuffer CC_RETAIN(Indexes), GFXBufferFormat Format);

/*!
 * @brief Set the vertex attributes of the draw command.
 * @param Draw The draw operation.
 * @param Input The input name from in the shader.
 * @param Buffer The vertex buffer of type @b GFXBufferHintDataVertex. Retains a reference
 *        to the vertex buffer.
 *
 * @param Format The format of the data.
 * @param Stride The stride of the data.
 * @param Offset The offset of the data.
 */
void GFXDrawSetVertexBuffer(GFXDraw Draw, CCString CC_COPY(Input), GFXBuffer CC_RETAIN(Buffer), GFXBufferFormat Format, size_t Stride, ptrdiff_t Offset);

/*!
 * @brief Set the uniform of the draw command.
 * @param Draw The draw operation.
 * @param Input The input name from in the shader.
 * @param Buffer The buffer. Retains a reference to the buffer.
 */
void GFXDrawSetBuffer(GFXDraw Draw, CCString CC_COPY(Input), GFXBuffer CC_RETAIN(Buffer));

/*!
 * @brief Set the texture of the draw command.
 * @param Draw The draw operation.
 * @param Input The input name from in the shader.
 * @param Texture The texture. Retains a reference to the shader.
 */
void GFXDrawSetTexture(GFXDraw Draw, CCString CC_COPY(Input), GFXTexture CC_RETAIN(Texture));

/*!
 * @brief Set the blending for the draw command.
 * @description The default is opaque.
 * @param Draw The draw operation.
 * @param BlendMask The blend mask.
 */
void GFXDrawSetBlending(GFXDraw Draw, GFXBlend BlendMask);

/*!
 * @brief Set the blending for a specific draw destination in the draw command.
 * @description The default is opaque.
 * @param Draw The draw operation.
 * @param BlendMask The blend mask.
 * @param Index The index of the draw destination to have its blend mode changed.
 */
void GFXDrawSetDestinationBlending(GFXDraw Draw, GFXBlend BlendMask, size_t Index);

/*!
 * @brief Set the clipping viewport for the draw command.
 * @description The default is equal to the bounds of the @b GFXDrawDestination and 0.0 -
 *              1.0 for the depth range.
 *
 * @param Draw The draw operation.
 * @param Viewport The viewport.
 */
void GFXDrawSetViewport(GFXDraw Draw, GFXViewport Viewport);

/*!
 * @brief Set the normalized clipping viewport for the draw command.
 * @description The default is equal to the bounds of the @b GFXDrawDestination and 0.0 -
 *              1.0 for the depth range.
 *
 * @param Draw The draw operation.
 * @param Viewport The normalized viewport.
 */
void GFXDrawSetNormalizedViewport(GFXDraw Draw, GFXViewport Viewport);

/*!
 * @brief Retrieve the clipping viewport for the draw command.
 * @description If the viewport is normalized, it will return the real viewport based on the
 *              current @b GFXDrawDestination.
 *
 * @param Draw The draw operation.
 * @return The viewport.
 */
GFXViewport GFXDrawGetViewport(GFXDraw Draw);

/*!
 * @brief Set the render behaviour for the draw command.
 * @description The default is @b GFXRenderBehaviourDefault does a fill, no culling, and front face triangle order is clock wise.
 * @param Draw The draw operation.
 * @param RasterMode The rasterisation mode.
 * @param CullMode The cull mode.
 * @param FrontFaceWinding The front face winding order.
 */
void GFXDrawSetRenderBehaviour(GFXDraw Draw, GFXRasterMode RasterMode, GFXCullMode CullMode, GFXWinding FrontFaceWinding);

/*!
 * @brief Set the depth test configuration for the draw command.
 * @description The default is a depth test that always fails and does not update.
 * @param Draw The draw operation.
 * @param Config The depth test configuration.
 * @param ClipMode The clip mode of the near and far planes.
 * @param Index The index of the depth buffer.
 */
void GFXDrawSetDepth(GFXDraw Draw, GFXDepth Config, GFXDepthClipMode ClipMode, size_t Index);

/*!
 * @brief Disable custom depth bias adjustments for the draw command.
 * @param Draw The draw operation.
 */
void GFXDrawDisableDepthBias(GFXDraw Draw);

/*!
 * @brief Enable custom depth bias adjustments for the draw command.
 * @description By default the custom depth bias is disabled.
 * @param Draw The draw operation.
 * @param Bias A constant bias to apply.
 * @param Slope A scaling slope to apply.
 * @param Clamp The minimum or maximum value to clamp the bias adjustment to. Use 0.0 to disable clamping.
 */
void GFXDrawEnableDepthBias(GFXDraw Draw, float Bias, float Slope, float Clamp);

/*!
 * @brief Set the stencil test configuration for the draw command.
 * @description The default is a stencil test that always fails and does not update.
 * @param Draw The draw operation.
 * @param Config The stencil test configuration
 * @param FrontReadMask The front face mask to use for reading the stencil buffer and reference value.
 * @param FrontWriteMask The front face  mask to use for writing the stencil buffer.
 * @param FrontReferenceValue The front face reference value to be used.
 * @param BackReadMask The back face mask to use for reading the stencil buffer and reference value.
 * @param BackWriteMask The back face mask to use for writing the stencil buffer.
 * @param BackReferenceValue The back face reference value to be used.
 * @param Index The index of the stencil buffer.
 */
void GFXDrawSetStencil(GFXDraw Draw, GFXStencil Config, uint32_t FrontReadMask, uint32_t FrontWriteMask, uint32_t FrontReferenceValue, uint32_t BackReadMask, uint32_t BackWriteMask, uint32_t BackReferenceValue, size_t Index);

/*!
 * @brief Set the scissor test for the draw command.
 * @description The default performs no scissor test.
 * @param Draw The draw operation.
 * @param Scissor The scissor test configuration object. Or NULL to disable.
 */
void GFXDrawSetScissor(GFXDraw Draw, GFXDrawScissor *CC_RETAIN(Scissor));

#endif
