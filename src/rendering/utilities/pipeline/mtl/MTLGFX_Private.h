/*
 *  Copyright (c) 2019, Stefan Johnson
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

#ifndef CommonGameKit_MTLGFX_Private_h
#define CommonGameKit_MTLGFX_Private_h

#import "MTLGFX.h"
#import <CommonObjc/Common.h>
@import Metal;

typedef struct {
    _Bool allowGPUOptimizedContents;
} MTLInternalFeatureMTLTextureDescriptor;

typedef struct {
    _Bool inputPrimitiveTopology;
    _Bool vertexBuffers;
    _Bool fragmentBuffers;
} MTLInternalFeatureMTLRenderPipelineDescriptor;

typedef enum {
    /// The texture format is not available
    MTLGFXPixelFormatCapabilityUnavailable = 0,
    /// The texture can be filtered during sampling
    MTLGFXPixelFormatCapabilityFilter = (1 << 0),
    /// The texture can be written to by a function
    MTLGFXPixelFormatCapabilityWrite = (1 << 1),
    /// The texture can be used as a color render target
    MTLGFXPixelFormatCapabilityColour = (1 << 2),
    /// The texture can be blended
    MTLGFXPixelFormatCapabilityBlend = (1 << 3),
    /// The texture can be used as the destination for multisample antialias (MSAA) data
    MTLGFXPixelFormatCapabilityMSAA = (1 << 4),
    /// The texture can be used as the destination for resolved MSAA data
    MTLGFXPixelFormatCapabilityResolve = (1 << 5),
    MTLGFXPixelFormatCapabilityAll = MTLGFXPixelFormatCapabilityFilter | MTLGFXPixelFormatCapabilityWrite | MTLGFXPixelFormatCapabilityColour | MTLGFXPixelFormatCapabilityBlend | MTLGFXPixelFormatCapabilityMSAA | MTLGFXPixelFormatCapabilityResolve,
    
    /// The texture can be read from and written to by the same function
    MTLGFXPixelFormatCapabilityReadWrite = (1 << 16)
} MTLGFXPixelFormatCapability;

typedef struct {
    struct {
        /// Maximum number of vertex attributes, per vertex descriptor
        size_t maxVertexAttributes;
        /// Maximum number of entries in the buffer argument table, per graphics or compute function
        size_t maxBufferArguments;
        /// Maximum number of entries in the texture argument table, per graphics or compute function
        size_t maxTextureArguments;
        /// Maximum number of entries in the sampler state argument table, per graphics or compute function
        size_t maxSamplerArguments;
        /// Maximum number of entries in the threadgroup memory argument table, per compute function
        size_t maxThreadgroupMemoryArguments;
        /// Maximum number of inlined constant data buffers, per graphics or compute function
        size_t maxInlineConstantBuffers;
        /// Maximum length of an inlined constant data buffer, per graphics or compute function
        size_t maxInlineConstantBufferLength;
        /// Maximum threads per threadgroup
        size_t maxThreadsPerThreadgroup;
        /// Maximum total threadgroup memory allocation
        size_t maxAllocatedThreadgroupMemory;
        /// Maximum total tile memory allocation
        size_t maxAllocatedTileMemory;
        /// Threadgroup memory length alignment
        size_t threadgroupMemoryAlignment;
        /// Maximum function memory allocation for a buffer in the constant address space
        size_t maxAllocatedConstantBufferMemory;
        /// Maximum number of inputs (scalars or vectors) to a fragment function, declared with the stage_in qualifier
        size_t maxFragmentFunctionStageInInputs;
        /// Maximum number of input components to a fragment function, declared with the stage_in qualifier
        size_t maxFragmentFunctionStageInInputComponents;
        /// Maximum number of function constants
        size_t maxFunctionConstants;
        /// Maximum tessellation factor
        size_t maxTessellationFactor;
        /// Maximum number of viewports and scissor rectangles, per vertex function
        size_t maxVertexFunctionViewportAndScissorRectangles;
        /// Maximum number of raster order groups, per fragment function
        size_t maxFragmentFunctionRasterOrderGroups;
        /// Maximum number of argument buffers in the argument table, per graphics or compute function
        size_t maxArgumentBufferArguments;
    } functionArguments;
    struct {
        /// Minimum buffer offset alignment
        size_t minBufferOffsetAlignment;
        /// Maximum 1D texture width
        size_t maxTextureDimensions1D;
        /// Maximum 2D texture width and height
        size_t maxTextureDimensions2D;
        /// Maximum cube map texture width and height
        size_t maxTextureDimensionsCubeMap;
        /// Maximum 3D texture width, height, and depth
        size_t maxTextureDimensions3D;
        /// Maximum number of layers per 1D texture array, 2D texture array, or 3D texture
        size_t maxTextureLayers;
        /// Buffer alignment for copying an existing texture to a buffer
        size_t bufferAlignmentForTextureCopy;
    } resources;
    struct {
        /// Maximum number of color render targets per render pass descriptor
        size_t maxColourRenderTargets;
        /// Maximum size of a point primitive
        size_t maxPointPrimitiveSize;
        /// Maximum total render target size, per pixel, when using multiple color render targets
        size_t maxRenderTargetSize;
        /// Maximum visibility query offset
        size_t maxVisibilityQueryOffset;
    } renderTargets;
    struct {
        /// Maximum number of fences
        size_t maxFences;
        /// Maximum vertex count for vertex amplification
        size_t maxVertexAmplificationVertexCount;
    } synchronization;
    struct {
        size_t maxUniqueSamplers;
    } argumentBuffers;
    struct {
        //Ordinary 8-bit pixel formats
        MTLGFXPixelFormatCapability A8Unorm;
        MTLGFXPixelFormatCapability R8Unorm;
        MTLGFXPixelFormatCapability R8Unorm_sRGB;
        MTLGFXPixelFormatCapability R8Snorm;
        MTLGFXPixelFormatCapability R8Uint;
        MTLGFXPixelFormatCapability R8Sint;
        
        //Ordinary 16-bit pixel formats
        MTLGFXPixelFormatCapability R16Unorm;
        MTLGFXPixelFormatCapability R16Snorm;
        MTLGFXPixelFormatCapability R16Uint;
        MTLGFXPixelFormatCapability R16Sint;
        MTLGFXPixelFormatCapability R16Float;
        MTLGFXPixelFormatCapability RG8Unorm;
        MTLGFXPixelFormatCapability RG8Unorm_sRGB;
        MTLGFXPixelFormatCapability RG8Snorm;
        MTLGFXPixelFormatCapability RG8Uint;
        MTLGFXPixelFormatCapability RG8Sint;
        
        //Packed 16-bit pixel formats
        MTLGFXPixelFormatCapability B5G6R5Unorm;
        MTLGFXPixelFormatCapability A1BGR5Unorm;
        MTLGFXPixelFormatCapability ABGR4Unorm;
        MTLGFXPixelFormatCapability BGR5A1Unorm;
        
        //Ordinary 32-bit pixel formats
        MTLGFXPixelFormatCapability R32Uint;
        MTLGFXPixelFormatCapability R32Sint;
        MTLGFXPixelFormatCapability R32Float;
        MTLGFXPixelFormatCapability RG16Unorm;
        MTLGFXPixelFormatCapability RG16Snorm;
        MTLGFXPixelFormatCapability RG16Uint;
        MTLGFXPixelFormatCapability RG16Sint;
        MTLGFXPixelFormatCapability RG16Float;
        MTLGFXPixelFormatCapability RGBA8Unorm;
        MTLGFXPixelFormatCapability RGBA8Unorm_sRGB;
        MTLGFXPixelFormatCapability RGBA8Snorm;
        MTLGFXPixelFormatCapability RGBA8Uint;
        MTLGFXPixelFormatCapability RGBA8Sint;
        MTLGFXPixelFormatCapability BGRA8Unorm;
        MTLGFXPixelFormatCapability BGRA8Unorm_sRGB;
        
        //Packed 32-bit pixel formats
        MTLGFXPixelFormatCapability RGB10A2Unorm;
        MTLGFXPixelFormatCapability RGB10A2Uint;
        MTLGFXPixelFormatCapability RG11B10Float;
        MTLGFXPixelFormatCapability RGB9E5Float;
        
        //Ordinary 64-bit pixel formats
        MTLGFXPixelFormatCapability RG32Uint;
        MTLGFXPixelFormatCapability RG32Sint;
        MTLGFXPixelFormatCapability RG32Float;
        MTLGFXPixelFormatCapability RGBA16Unorm;
        MTLGFXPixelFormatCapability RGBA16Snorm;
        MTLGFXPixelFormatCapability RGBA16Uint;
        MTLGFXPixelFormatCapability RGBA16Sint;
        MTLGFXPixelFormatCapability RGBA16Float;
        
        //Ordinary 128-bit pixel formats
        MTLGFXPixelFormatCapability RGBA32Uint;
        MTLGFXPixelFormatCapability RGBA32Sint;
        MTLGFXPixelFormatCapability RGBA32Float;
        
        //Compressed pixel formats
        MTLGFXPixelFormatCapability PVRTC;
        MTLGFXPixelFormatCapability EAC;
        MTLGFXPixelFormatCapability ETC;
        MTLGFXPixelFormatCapability ASTC;
        MTLGFXPixelFormatCapability HDR_ASTC;
        MTLGFXPixelFormatCapability BC;
        
        //YUV pixel formats
        MTLGFXPixelFormatCapability GBGR422;
        MTLGFXPixelFormatCapability BGRG422;
        
        //Depth and stencil pixel formats
        MTLGFXPixelFormatCapability Depth16Unorm;
        MTLGFXPixelFormatCapability Depth32Float;
        MTLGFXPixelFormatCapability Stencil8;
        MTLGFXPixelFormatCapability Depth24Unorm_Stencil8;
        MTLGFXPixelFormatCapability Depth32Float_Stencil8;
        MTLGFXPixelFormatCapability X24_Stencil8;
        MTLGFXPixelFormatCapability X32_Stencil8;
        
        //Extended range and wide color pixel formats
        MTLGFXPixelFormatCapability BGRA10_XR;
        MTLGFXPixelFormatCapability BGRA10_XR_sRGB;
        MTLGFXPixelFormatCapability BGR10_XR;
        MTLGFXPixelFormatCapability BGR10_XR_sRGB;
        MTLGFXPixelFormatCapability BGR10A2Unorm;
    } pixelFormats;
} MTLGFXCapability;

typedef struct {
    MTLInternalFeatureMTLTextureDescriptor MTLTextureDescriptor;
    MTLInternalFeatureMTLRenderPipelineDescriptor MTLRenderPipelineDescriptor;
    MTLGFXCapability capability;
} MTLInternalSupport;

typedef struct {
    __unsafe_unretained id <MTLDevice>device;
    __unsafe_unretained id <MTLCommandQueue>commandQueue;
    GFXTexture drawable;
    MTLInternalSupport *support;
    __unsafe_unretained id <MTLSamplerState>samplers[255];
} MTLInternal;

id <MTLSamplerState>MTLGFXGetSampler(GFXTextureHint Hint);

#endif

