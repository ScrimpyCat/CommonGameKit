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

#define CC_QUICK_COMPILE
#import "MTLGFX_Private.h"
#import "MTLBuffer.h"
#import "MTLTexture.h"
#import "MTLFramebuffer.h"
#import "MTLShaderLibrary.h"
#import "MTLShader.h"
#import "MTLDraw.h"
#import "MTLBlit.h"
#import "MTLCommandBuffer.h"
#import <stdatomic.h>
@import ObjectiveC;

@interface GFXDrawableTexture : NSProxy
@end

static MTLInternalSupport MTLGFXSupport = {};

static MTLInternal MTLInfo = {
    .device = nil,
    .commandQueue = nil,
    .support = &MTLGFXSupport,
    .samplers = {nil}
};

static GFXMainInfo MTLGFXInfo = {
    .internal = &MTLInfo,
    .buffer = &MTLBufferInterface,
    .texture = &MTLTextureInterface,
    .framebuffer = &MTLFramebufferInterface,
    .library = &MTLShaderLibraryInterface,
    .shader = &MTLShaderInterface,
    .draw = &MTLDrawInterface,
    .blit = &MTLBlitInterface,
    .commandBuffer = &MTLCommandBufferInterface
};

GFXMainInfo * const MTLGFX = &MTLGFXInfo;

static inline CC_CONSTANT_FUNCTION MTLSamplerMinMagFilter SamplerFilter(GFXTextureHint Hint, GFXTextureHint FilterType)
{
    return (MTLSamplerMinMagFilter[2]){
        MTLSamplerMinMagFilterNearest,
        MTLSamplerMinMagFilterLinear
    }[(Hint >> FilterType) & GFXTextureHintFilterModeMask];
}

static inline CC_CONSTANT_FUNCTION MTLSamplerAddressMode SamplerAddressMode(GFXTextureHint Hint, GFXTextureHint Coordinate)
{
    return (MTLSamplerAddressMode[4]){
        MTLSamplerAddressModeClampToEdge,
        MTLSamplerAddressModeRepeat,
        MTLSamplerAddressModeMirrorRepeat,
        0
    }[(Hint >> Coordinate) & GFXTextureHintAddressModeMask];
}

_Static_assert(sizeof(MTLInfo.samplers) / sizeof(typeof(*MTLInfo.samplers)) == (((GFXTextureHintFilterModeMask << GFXTextureHintFilterMin) | (GFXTextureHintFilterModeMask << GFXTextureHintFilterMag) | (GFXTextureHintAddressModeMask << GFXTextureHintAddress_S) | (GFXTextureHintAddressModeMask << GFXTextureHintAddress_T) | (GFXTextureHintAddressModeMask << GFXTextureHintAddress_R)) >> 2), "Sampler count needs to be updated");
_Static_assert(GFXTextureHintFilterMin == 2, "Smallest mask index was changed");

id <MTLSamplerState>MTLGFXGetSampler(GFXTextureHint Hint)
{
    const size_t Index = Hint >> 2;
    
    CCAssertLog(Index <= sizeof(MTLInfo.samplers) / sizeof(typeof(*MTLInfo.samplers)), "Unsupported hint");
    
    id <MTLSamplerState>Sampler = MTLInfo.samplers[Index];
    
    CCAssertLog(Sampler, "No sampler for hint");
    
    return Sampler;
}

static void MTLGFXGetFeatures(void)
{
#define MTL_GFX_FEATURE(classname, propname) { .cls = [classname class], .property = #propname, .enabled = (void*)&MTLGFXSupport + offsetof(MTLInternalSupport, classname) + offsetof(MTLInternalFeature ## classname, propname) }
    
    const struct {
        Class cls;
        const char *property;
        _Bool *enabled;
    } Features[] = {
        MTL_GFX_FEATURE(MTLTextureDescriptor, allowGPUOptimizedContents),
        MTL_GFX_FEATURE(MTLRenderPipelineDescriptor, inputPrimitiveTopology),
        MTL_GFX_FEATURE(MTLRenderPipelineDescriptor, vertexBuffers),
        MTL_GFX_FEATURE(MTLRenderPipelineDescriptor, fragmentBuffers)
    };
    
    for (size_t Loop = 0; Loop < sizeof(Features) / sizeof(typeof(*Features)); Loop++)
    {
        *Features[Loop].enabled = (_Bool)class_getProperty([Features[Loop].cls class], Features[Loop].property);
    }
}

typedef enum {
    MTLGFXFamilyApple1,
    MTLGFXFamilyApple2,
    MTLGFXFamilyApple3,
    MTLGFXFamilyApple4,
    MTLGFXFamilyApple5,
    MTLGFXFamilyApple6,
    MTLGFXFamilyMac1,
    MTLGFXFamilyMac2
} MTLGFXFamily;

#if CC_PLATFORM_APPLE_VERSION_MIN_REQUIRED(CC_PLATFORM_MAC_10_11, CC_PLATFORM_IOS_8_0)
typedef struct {
    MTLFeatureSet set;
    MTLGFXFamily family;
} MTLGFXFamilyMapper;
#endif

#define MTL_GFX_PIXEL_FORMAT(...) MTL_GFX_PIXEL_FORMAT_(__VA_ARGS__, Unavailable, Unavailable, Unavailable, Unavailable, Unavailable, Unavailable)
#define MTL_GFX_PIXEL_FORMAT_(_0, _1, _2, _3, _4, _5, ...) MTLGFXPixelFormatCapability##_0 | MTLGFXPixelFormatCapability##_1 | MTLGFXPixelFormatCapability##_2 | MTLGFXPixelFormatCapability##_3 | MTLGFXPixelFormatCapability##_4 | MTLGFXPixelFormatCapability##_5

_Static_assert(MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, Blend, MSAA, Resolve) == MTLGFXPixelFormatCapabilityAll, "Need to update arguments");

//Data from: https://developer.apple.com/metal/Metal-Feature-Set-Tables.pdf
static const MTLGFXCapability Caps[] = {
    {
        //MTLGFXFamilyApple1
        .functionArguments = {
            .maxVertexAttributes = 31,
            .maxBufferArguments = 31,
            .maxTextureArguments = 31,
            .maxSamplerArguments = 16,
            .maxThreadgroupMemoryArguments = 31,
            .maxInlineConstantBuffers = 31,
            .maxInlineConstantBufferLength = 4000, //4 KB
            .maxThreadsPerThreadgroup = 512,
            .maxAllocatedThreadgroupMemory = 16352, //16352 B
            .maxAllocatedTileMemory = 0, //Not accessible
            .threadgroupMemoryAlignment = 16, //16 B
            .maxAllocatedConstantBufferMemory = SIZE_MAX, //No limit
            .maxFragmentFunctionStageInInputs = 60,
            .maxFragmentFunctionStageInInputComponents = 60,
            .maxFunctionConstants = 65536,
            .maxTessellationFactor = 0, // Not available
            .maxVertexFunctionViewportAndScissorRectangles = 1,
            .maxFragmentFunctionRasterOrderGroups = 0, // Not available
            .maxArgumentBufferArguments = 0,
        },
        .resources = {
            .minBufferOffsetAlignment = 4, // 4 B
            .maxTextureDimensions1D = 8192, // 8192 px
            .maxTextureDimensions2D = 8192, // 8192 px
            .maxTextureDimensionsCubeMap = 8192, // 8192 px
            .maxTextureDimensions3D = 2048, // 2048 px
            .maxTextureLayers = 2048,
            .bufferAlignmentForTextureCopy = 64, // 64 B
        },
        .renderTargets = {
            .maxColourRenderTargets = 4,
            .maxPointPrimitiveSize = 511,
            .maxRenderTargetSize = 128, // 128 bits
            .maxVisibilityQueryOffset = 65528, // 65528 B
        },
        .synchronization = {
            .maxFences = 32768,
            .maxVertexAmplificationVertexCount = 0, // Not available
        },
        .pixelFormats = {
            .A8Unorm = MTL_GFX_PIXEL_FORMAT(Filter),
            .R8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .R8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            .R8Snorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .R8Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .R8Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            
            .R16Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .R16Snorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .R16Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .R16Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .R16Float = MTL_GFX_PIXEL_FORMAT(All),
            .RG8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .RG8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            .RG8Snorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .RG8Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG8Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            
            .B5G6R5Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            .A1BGR5Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            .ABGR4Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            .BGR5A1Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            
            .R32Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .R32Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .R32Float = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA, Blend),
            .RG16Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .RG16Snorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .RG16Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG16Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG16Float = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            .RGBA8Snorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .RGBA8Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RGBA8Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .BGRA8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .BGRA8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            
            .RGB10A2Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            .RGB10A2Uint = MTL_GFX_PIXEL_FORMAT(Colour, MSAA),
            .RG11B10Float = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            .RGB9E5Float = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            
            .RG32Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .RG32Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .RG32Float = MTL_GFX_PIXEL_FORMAT(Write, Colour, Blend),
            .RGBA16Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .RGBA16Snorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .RGBA16Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RGBA16Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RGBA16Float = MTL_GFX_PIXEL_FORMAT(All),
            
            .RGBA32Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .RGBA32Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .RGBA32Float = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            
            .PVRTC = MTL_GFX_PIXEL_FORMAT(Filter),
            .EAC = MTL_GFX_PIXEL_FORMAT(Filter),
            .ETC = MTL_GFX_PIXEL_FORMAT(Filter),
            .ASTC = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .HDR_ASTC = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .BC = MTL_GFX_PIXEL_FORMAT(Unavailable),
            
            .GBGR422 = MTL_GFX_PIXEL_FORMAT(Filter),
            .BGRG422 = MTL_GFX_PIXEL_FORMAT(Filter),
            
            .Depth16Unorm = MTL_GFX_PIXEL_FORMAT(Filter, MSAA),
            .Depth32Float = MTL_GFX_PIXEL_FORMAT(MSAA),
            .Stencil8 = MTL_GFX_PIXEL_FORMAT(MSAA),
            .Depth24Unorm_Stencil8 = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .Depth32Float_Stencil8 = MTL_GFX_PIXEL_FORMAT(MSAA),
            .X24_Stencil8 = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .X32_Stencil8 = MTL_GFX_PIXEL_FORMAT(MSAA),
            
            .BGRA10_XR = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .BGRA10_XR_sRGB = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .BGR10_XR = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .BGR10_XR_sRGB = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .BGR10A2Unorm = MTL_GFX_PIXEL_FORMAT(All),
        }
    },
    {
        //MTLGFXFamilyApple2
        .functionArguments = {
            .maxVertexAttributes = 31,
            .maxBufferArguments = 31,
            .maxTextureArguments = 31,
            .maxSamplerArguments = 16,
            .maxThreadgroupMemoryArguments = 31,
            .maxInlineConstantBuffers = 31,
            .maxInlineConstantBufferLength = 4000, //4 KB
            .maxThreadsPerThreadgroup = 512,
            .maxAllocatedThreadgroupMemory = 16352, //16352 B
            .maxAllocatedTileMemory = 0, //Not accessible
            .threadgroupMemoryAlignment = 16, //16 B
            .maxAllocatedConstantBufferMemory = SIZE_MAX, //No limit
            .maxFragmentFunctionStageInInputs = 60,
            .maxFragmentFunctionStageInInputComponents = 60,
            .maxFunctionConstants = 65536,
            .maxTessellationFactor = 0, // Not available
            .maxVertexFunctionViewportAndScissorRectangles = 1,
            .maxFragmentFunctionRasterOrderGroups = 0, // Not available
            .maxArgumentBufferArguments = 0,
        },
        .resources = {
            .minBufferOffsetAlignment = 4, // 4 B
            .maxTextureDimensions1D = 8192, // 8192 px
            .maxTextureDimensions2D = 8192, // 8192 px
            .maxTextureDimensionsCubeMap = 8192, // 8192 px
            .maxTextureDimensions3D = 2048, // 2048 px
            .maxTextureLayers = 2048,
            .bufferAlignmentForTextureCopy = 64, // 64 B
        },
        .renderTargets = {
            .maxColourRenderTargets = 8,
            .maxPointPrimitiveSize = 511,
            .maxRenderTargetSize = 256, // 256 bits
            .maxVisibilityQueryOffset = 65528, // 65528 B
        },
        .synchronization = {
            .maxFences = 32768,
            .maxVertexAmplificationVertexCount = 0, // Not available
        },
        .pixelFormats = {
            .A8Unorm = MTL_GFX_PIXEL_FORMAT(Filter),
            .R8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .R8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(All),
            .R8Snorm = MTL_GFX_PIXEL_FORMAT(All),
            .R8Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .R8Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            
            .R16Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .R16Snorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .R16Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .R16Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .R16Float = MTL_GFX_PIXEL_FORMAT(All),
            .RG8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .RG8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(All),
            .RG8Snorm = MTL_GFX_PIXEL_FORMAT(All),
            .RG8Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG8Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            
            .B5G6R5Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            .A1BGR5Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            .ABGR4Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            .BGR5A1Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            
            .R32Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .R32Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .R32Float = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA, Blend),
            .RG16Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .RG16Snorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .RG16Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG16Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG16Float = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA8Snorm = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA8Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RGBA8Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .BGRA8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .BGRA8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(All),
            
            .RGB10A2Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            .RGB10A2Uint = MTL_GFX_PIXEL_FORMAT(Colour, MSAA),
            .RG11B10Float = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            .RGB9E5Float = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            
            .RG32Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .RG32Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .RG32Float = MTL_GFX_PIXEL_FORMAT(Write, Colour, Blend),
            .RGBA16Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .RGBA16Snorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .RGBA16Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RGBA16Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RGBA16Float = MTL_GFX_PIXEL_FORMAT(All),
            
            .RGBA32Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .RGBA32Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .RGBA32Float = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            
            .PVRTC = MTL_GFX_PIXEL_FORMAT(Filter),
            .EAC = MTL_GFX_PIXEL_FORMAT(Filter),
            .ETC = MTL_GFX_PIXEL_FORMAT(Filter),
            .ASTC = MTL_GFX_PIXEL_FORMAT(Filter),
            .HDR_ASTC = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .BC = MTL_GFX_PIXEL_FORMAT(Unavailable),
            
            .GBGR422 = MTL_GFX_PIXEL_FORMAT(Filter),
            .BGRG422 = MTL_GFX_PIXEL_FORMAT(Filter),
            
            .Depth16Unorm = MTL_GFX_PIXEL_FORMAT(Filter, MSAA),
            .Depth32Float = MTL_GFX_PIXEL_FORMAT(MSAA),
            .Stencil8 = MTL_GFX_PIXEL_FORMAT(MSAA),
            .Depth24Unorm_Stencil8 = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .Depth32Float_Stencil8 = MTL_GFX_PIXEL_FORMAT(MSAA),
            .X24_Stencil8 = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .X32_Stencil8 = MTL_GFX_PIXEL_FORMAT(MSAA),
            
            .BGRA10_XR = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .BGRA10_XR_sRGB = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .BGR10_XR = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .BGR10_XR_sRGB = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .BGR10A2Unorm = MTL_GFX_PIXEL_FORMAT(All),
        }
    },
    {
        //MTLGFXFamilyApple3
        .functionArguments = {
            .maxVertexAttributes = 31,
            .maxBufferArguments = 31,
            .maxTextureArguments = 31,
            .maxSamplerArguments = 16,
            .maxThreadgroupMemoryArguments = 31,
            .maxInlineConstantBuffers = 31,
            .maxInlineConstantBufferLength = 4000, //4 KB
            .maxThreadsPerThreadgroup = 512,
            .maxAllocatedThreadgroupMemory = 16000, //16 KB
            .maxAllocatedTileMemory = 0, //Not accessible
            .threadgroupMemoryAlignment = 16, //16 B
            .maxAllocatedConstantBufferMemory = SIZE_MAX, //No limit
            .maxFragmentFunctionStageInInputs = 60,
            .maxFragmentFunctionStageInInputComponents = 60,
            .maxFunctionConstants = 65536,
            .maxTessellationFactor = 16,
            .maxVertexFunctionViewportAndScissorRectangles = 1,
            .maxFragmentFunctionRasterOrderGroups = 0, // Not available
            .maxArgumentBufferArguments = 0,
        },
        .resources = {
            .minBufferOffsetAlignment = 4, // 4 B
            .maxTextureDimensions1D = 16384, // 16384 px
            .maxTextureDimensions2D = 16384, // 16384 px
            .maxTextureDimensionsCubeMap = 16384, // 16384 px
            .maxTextureDimensions3D = 2048, // 2048 px
            .maxTextureLayers = 2048,
            .bufferAlignmentForTextureCopy = 16, // 16 B
        },
        .renderTargets = {
            .maxColourRenderTargets = 8,
            .maxPointPrimitiveSize = 511,
            .maxRenderTargetSize = 256, // 256 bits
            .maxVisibilityQueryOffset = 65528, // 65528 B
        },
        .synchronization = {
            .maxFences = 32768,
            .maxVertexAmplificationVertexCount = 0, // Not available
        },
        .pixelFormats = {
            .A8Unorm = MTL_GFX_PIXEL_FORMAT(Filter),
            .R8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .R8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(All),
            .R8Snorm = MTL_GFX_PIXEL_FORMAT(All),
            .R8Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .R8Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            
            .R16Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .R16Snorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .R16Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .R16Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .R16Float = MTL_GFX_PIXEL_FORMAT(All),
            .RG8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .RG8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(All),
            .RG8Snorm = MTL_GFX_PIXEL_FORMAT(All),
            .RG8Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG8Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            
            .B5G6R5Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            .A1BGR5Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            .ABGR4Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            .BGR5A1Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            
            .R32Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .R32Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .R32Float = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA, Blend),
            .RG16Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .RG16Snorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .RG16Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG16Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG16Float = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA8Snorm = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA8Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RGBA8Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .BGRA8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .BGRA8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(All),
            
            .RGB10A2Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .RGB10A2Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG11B10Float = MTL_GFX_PIXEL_FORMAT(All),
            .RGB9E5Float = MTL_GFX_PIXEL_FORMAT(All),
            
            .RG32Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .RG32Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .RG32Float = MTL_GFX_PIXEL_FORMAT(Write, Colour, Blend),
            .RGBA16Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .RGBA16Snorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .RGBA16Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RGBA16Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RGBA16Float = MTL_GFX_PIXEL_FORMAT(All),
            
            .RGBA32Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .RGBA32Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .RGBA32Float = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            
            .PVRTC = MTL_GFX_PIXEL_FORMAT(Filter),
            .EAC = MTL_GFX_PIXEL_FORMAT(Filter),
            .ETC = MTL_GFX_PIXEL_FORMAT(Filter),
            .ASTC = MTL_GFX_PIXEL_FORMAT(Filter),
            .HDR_ASTC = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .BC = MTL_GFX_PIXEL_FORMAT(Unavailable),
            
            .GBGR422 = MTL_GFX_PIXEL_FORMAT(Filter),
            .BGRG422 = MTL_GFX_PIXEL_FORMAT(Filter),
            
            .Depth16Unorm = MTL_GFX_PIXEL_FORMAT(Filter, MSAA, Resolve),
            .Depth32Float = MTL_GFX_PIXEL_FORMAT(MSAA, Resolve),
            .Stencil8 = MTL_GFX_PIXEL_FORMAT(MSAA),
            .Depth24Unorm_Stencil8 = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .Depth32Float_Stencil8 = MTL_GFX_PIXEL_FORMAT(MSAA, Resolve),
            .X24_Stencil8 = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .X32_Stencil8 = MTL_GFX_PIXEL_FORMAT(MSAA),
            
            .BGRA10_XR = MTL_GFX_PIXEL_FORMAT(All),
            .BGRA10_XR_sRGB = MTL_GFX_PIXEL_FORMAT(All),
            .BGR10_XR = MTL_GFX_PIXEL_FORMAT(All),
            .BGR10_XR_sRGB = MTL_GFX_PIXEL_FORMAT(All),
            .BGR10A2Unorm = MTL_GFX_PIXEL_FORMAT(All),
        }
    },
    {
        //MTLGFXFamilyApple4
        .functionArguments = {
            .maxVertexAttributes = 31,
            .maxBufferArguments = 31,
            .maxTextureArguments = 96,
            .maxSamplerArguments = 16,
            .maxThreadgroupMemoryArguments = 31,
            .maxInlineConstantBuffers = 31,
            .maxInlineConstantBufferLength = 4000, //4 KB
            .maxThreadsPerThreadgroup = 1024,
            .maxAllocatedThreadgroupMemory = 32000, //32 KB
            .maxAllocatedTileMemory = 32000, //32 KB
            .threadgroupMemoryAlignment = 16, //16 B
            .maxAllocatedConstantBufferMemory = SIZE_MAX, //No limit
            .maxFragmentFunctionStageInInputs = 124,
            .maxFragmentFunctionStageInInputComponents = 124,
            .maxFunctionConstants = 65536,
            .maxTessellationFactor = 16,
            .maxVertexFunctionViewportAndScissorRectangles = 1,
            .maxFragmentFunctionRasterOrderGroups = 8,
            .maxArgumentBufferArguments = 0,
        },
        .resources = {
            .minBufferOffsetAlignment = 4, // 4 B
            .maxTextureDimensions1D = 16384, // 16384 px
            .maxTextureDimensions2D = 16384, // 16384 px
            .maxTextureDimensionsCubeMap = 16384, // 16384 px
            .maxTextureDimensions3D = 2048, // 2048 px
            .maxTextureLayers = 2048,
            .bufferAlignmentForTextureCopy = 16, // 16 B
        },
        .renderTargets = {
            .maxColourRenderTargets = 8,
            .maxPointPrimitiveSize = 511,
            .maxRenderTargetSize = 512, // 512 bits
            .maxVisibilityQueryOffset = 65528, // 65528 B
        },
        .synchronization = {
            .maxFences = 32768,
            .maxVertexAmplificationVertexCount = 0, // Not available
        },
        .pixelFormats = {
            .A8Unorm = MTL_GFX_PIXEL_FORMAT(Filter),
            .R8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .R8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(All),
            .R8Snorm = MTL_GFX_PIXEL_FORMAT(All),
            .R8Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .R8Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            
            .R16Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .R16Snorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .R16Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .R16Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .R16Float = MTL_GFX_PIXEL_FORMAT(All),
            .RG8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .RG8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(All),
            .RG8Snorm = MTL_GFX_PIXEL_FORMAT(All),
            .RG8Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG8Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            
            .B5G6R5Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            .A1BGR5Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            .ABGR4Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            .BGR5A1Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            
            .R32Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .R32Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .R32Float = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA, Blend),
            .RG16Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .RG16Snorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .RG16Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG16Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG16Float = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA8Snorm = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA8Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RGBA8Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .BGRA8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .BGRA8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(All),
            
            .RGB10A2Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .RGB10A2Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG11B10Float = MTL_GFX_PIXEL_FORMAT(All),
            .RGB9E5Float = MTL_GFX_PIXEL_FORMAT(All),
            
            .RG32Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .RG32Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .RG32Float = MTL_GFX_PIXEL_FORMAT(Write, Colour, Blend),
            .RGBA16Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .RGBA16Snorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .RGBA16Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RGBA16Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RGBA16Float = MTL_GFX_PIXEL_FORMAT(All),
            
            .RGBA32Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .RGBA32Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .RGBA32Float = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            
            .PVRTC = MTL_GFX_PIXEL_FORMAT(Filter),
            .EAC = MTL_GFX_PIXEL_FORMAT(Filter),
            .ETC = MTL_GFX_PIXEL_FORMAT(Filter),
            .ASTC = MTL_GFX_PIXEL_FORMAT(Filter),
            .HDR_ASTC = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .BC = MTL_GFX_PIXEL_FORMAT(Unavailable),
            
            .GBGR422 = MTL_GFX_PIXEL_FORMAT(Filter),
            .BGRG422 = MTL_GFX_PIXEL_FORMAT(Filter),
            
            .Depth16Unorm = MTL_GFX_PIXEL_FORMAT(Filter, MSAA, Resolve),
            .Depth32Float = MTL_GFX_PIXEL_FORMAT(MSAA, Resolve),
            .Stencil8 = MTL_GFX_PIXEL_FORMAT(MSAA),
            .Depth24Unorm_Stencil8 = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .Depth32Float_Stencil8 = MTL_GFX_PIXEL_FORMAT(MSAA, Resolve),
            .X24_Stencil8 = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .X32_Stencil8 = MTL_GFX_PIXEL_FORMAT(MSAA),
            
            .BGRA10_XR = MTL_GFX_PIXEL_FORMAT(All),
            .BGRA10_XR_sRGB = MTL_GFX_PIXEL_FORMAT(All),
            .BGR10_XR = MTL_GFX_PIXEL_FORMAT(All),
            .BGR10_XR_sRGB = MTL_GFX_PIXEL_FORMAT(All),
            .BGR10A2Unorm = MTL_GFX_PIXEL_FORMAT(All),
        }
    },
    {
        //MTLGFXFamilyApple5
        .functionArguments = {
            .maxVertexAttributes = 31,
            .maxBufferArguments = 31,
            .maxTextureArguments = 96,
            .maxSamplerArguments = 16,
            .maxThreadgroupMemoryArguments = 31,
            .maxInlineConstantBuffers = 31,
            .maxInlineConstantBufferLength = 4000, //4 KB
            .maxThreadsPerThreadgroup = 1024,
            .maxAllocatedThreadgroupMemory = 32000, //32 KB
            .maxAllocatedTileMemory = 32000, //32 KB
            .threadgroupMemoryAlignment = 16, //16 B
            .maxAllocatedConstantBufferMemory = SIZE_MAX, //No limit
            .maxFragmentFunctionStageInInputs = 124,
            .maxFragmentFunctionStageInInputComponents = 124,
            .maxFunctionConstants = 65536,
            .maxTessellationFactor = 64,
            .maxVertexFunctionViewportAndScissorRectangles = 16,
            .maxFragmentFunctionRasterOrderGroups = 8,
            .maxArgumentBufferArguments = 0,
        },
        .resources = {
            .minBufferOffsetAlignment = 4, // 4 B
            .maxTextureDimensions1D = 16384, // 16384 px
            .maxTextureDimensions2D = 16384, // 16384 px
            .maxTextureDimensionsCubeMap = 16384, // 16384 px
            .maxTextureDimensions3D = 2048, // 2048 px
            .maxTextureLayers = 2048,
            .bufferAlignmentForTextureCopy = 16, // 16 B
        },
        .renderTargets = {
            .maxColourRenderTargets = 8,
            .maxPointPrimitiveSize = 511,
            .maxRenderTargetSize = 512, // 512 bits
            .maxVisibilityQueryOffset = 65528, // 65528 B
        },
        .synchronization = {
            .maxFences = 32768,
            .maxVertexAmplificationVertexCount = 0, // Not available
        },
        .pixelFormats = {
            .A8Unorm = MTL_GFX_PIXEL_FORMAT(Filter),
            .R8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .R8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(All),
            .R8Snorm = MTL_GFX_PIXEL_FORMAT(All),
            .R8Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .R8Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            
            .R16Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .R16Snorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .R16Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .R16Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .R16Float = MTL_GFX_PIXEL_FORMAT(All),
            .RG8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .RG8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(All),
            .RG8Snorm = MTL_GFX_PIXEL_FORMAT(All),
            .RG8Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG8Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            
            .B5G6R5Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            .A1BGR5Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            .ABGR4Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            .BGR5A1Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            
            .R32Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .R32Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .R32Float = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA, Blend),
            .RG16Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .RG16Snorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .RG16Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG16Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG16Float = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA8Snorm = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA8Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RGBA8Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .BGRA8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .BGRA8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(All),
            
            .RGB10A2Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .RGB10A2Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG11B10Float = MTL_GFX_PIXEL_FORMAT(All),
            .RGB9E5Float = MTL_GFX_PIXEL_FORMAT(All),
            
            .RG32Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .RG32Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .RG32Float = MTL_GFX_PIXEL_FORMAT(Write, Colour, Blend),
            .RGBA16Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .RGBA16Snorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .RGBA16Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RGBA16Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RGBA16Float = MTL_GFX_PIXEL_FORMAT(All),
            
            .RGBA32Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .RGBA32Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .RGBA32Float = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            
            .PVRTC = MTL_GFX_PIXEL_FORMAT(Filter),
            .EAC = MTL_GFX_PIXEL_FORMAT(Filter),
            .ETC = MTL_GFX_PIXEL_FORMAT(Filter),
            .ASTC = MTL_GFX_PIXEL_FORMAT(Filter),
            .HDR_ASTC = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .BC = MTL_GFX_PIXEL_FORMAT(Unavailable),
            
            .GBGR422 = MTL_GFX_PIXEL_FORMAT(Filter),
            .BGRG422 = MTL_GFX_PIXEL_FORMAT(Filter),
            
            .Depth16Unorm = MTL_GFX_PIXEL_FORMAT(Filter, MSAA, Resolve),
            .Depth32Float = MTL_GFX_PIXEL_FORMAT(MSAA, Resolve),
            .Stencil8 = MTL_GFX_PIXEL_FORMAT(MSAA),
            .Depth24Unorm_Stencil8 = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .Depth32Float_Stencil8 = MTL_GFX_PIXEL_FORMAT(MSAA, Resolve),
            .X24_Stencil8 = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .X32_Stencil8 = MTL_GFX_PIXEL_FORMAT(MSAA),
            
            .BGRA10_XR = MTL_GFX_PIXEL_FORMAT(All),
            .BGRA10_XR_sRGB = MTL_GFX_PIXEL_FORMAT(All),
            .BGR10_XR = MTL_GFX_PIXEL_FORMAT(All),
            .BGR10_XR_sRGB = MTL_GFX_PIXEL_FORMAT(All),
            .BGR10A2Unorm = MTL_GFX_PIXEL_FORMAT(All),
        }
    },
    {
        //MTLGFXFamilyApple6
        .functionArguments = {
            .maxVertexAttributes = 31,
            .maxBufferArguments = 31,
            .maxTextureArguments = 128,
            .maxSamplerArguments = 16,
            .maxThreadgroupMemoryArguments = 31,
            .maxInlineConstantBuffers = 31,
            .maxInlineConstantBufferLength = 4000, //4 KB
            .maxThreadsPerThreadgroup = 1024,
            .maxAllocatedThreadgroupMemory = 32000, //32 KB
            .maxAllocatedTileMemory = 32000, //32 KB
            .threadgroupMemoryAlignment = 16, //16 B
            .maxAllocatedConstantBufferMemory = SIZE_MAX, //No limit
            .maxFragmentFunctionStageInInputs = 124,
            .maxFragmentFunctionStageInInputComponents = 124,
            .maxFunctionConstants = 65536,
            .maxTessellationFactor = 64,
            .maxVertexFunctionViewportAndScissorRectangles = 16,
            .maxFragmentFunctionRasterOrderGroups = 8,
            .maxArgumentBufferArguments = 0,
        },
        .resources = {
            .minBufferOffsetAlignment = 4, // 4 B
            .maxTextureDimensions1D = 16384, // 16384 px
            .maxTextureDimensions2D = 16384, // 16384 px
            .maxTextureDimensionsCubeMap = 16384, // 16384 px
            .maxTextureDimensions3D = 2048, // 2048 px
            .maxTextureLayers = 2048,
            .bufferAlignmentForTextureCopy = 16, // 16 B
        },
        .renderTargets = {
            .maxColourRenderTargets = 8,
            .maxPointPrimitiveSize = 511,
            .maxRenderTargetSize = 512, // 512 bits
            .maxVisibilityQueryOffset = 65528, // 65528 B
        },
        .synchronization = {
            .maxFences = 32768,
            .maxVertexAmplificationVertexCount = 2,
        },
        .pixelFormats = {
            .A8Unorm = MTL_GFX_PIXEL_FORMAT(Filter),
            .R8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .R8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(All),
            .R8Snorm = MTL_GFX_PIXEL_FORMAT(All),
            .R8Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .R8Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            
            .R16Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .R16Snorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .R16Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .R16Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .R16Float = MTL_GFX_PIXEL_FORMAT(All),
            .RG8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .RG8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(All),
            .RG8Snorm = MTL_GFX_PIXEL_FORMAT(All),
            .RG8Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG8Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            
            .B5G6R5Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            .A1BGR5Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            .ABGR4Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            .BGR5A1Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            
            .R32Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .R32Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .R32Float = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA, Blend),
            .RG16Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .RG16Snorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .RG16Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG16Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG16Float = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA8Snorm = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA8Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RGBA8Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .BGRA8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .BGRA8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(All),
            
            .RGB10A2Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .RGB10A2Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG11B10Float = MTL_GFX_PIXEL_FORMAT(All),
            .RGB9E5Float = MTL_GFX_PIXEL_FORMAT(All),
            
            .RG32Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .RG32Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .RG32Float = MTL_GFX_PIXEL_FORMAT(Write, Colour, Blend),
            .RGBA16Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .RGBA16Snorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Blend),
            .RGBA16Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RGBA16Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RGBA16Float = MTL_GFX_PIXEL_FORMAT(All),
            
            .RGBA32Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .RGBA32Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            .RGBA32Float = MTL_GFX_PIXEL_FORMAT(Write, Colour),
            
            .PVRTC = MTL_GFX_PIXEL_FORMAT(Filter),
            .EAC = MTL_GFX_PIXEL_FORMAT(Filter),
            .ETC = MTL_GFX_PIXEL_FORMAT(Filter),
            .ASTC = MTL_GFX_PIXEL_FORMAT(Filter),
            .HDR_ASTC = MTL_GFX_PIXEL_FORMAT(Filter),
            .BC = MTL_GFX_PIXEL_FORMAT(Unavailable),
            
            .GBGR422 = MTL_GFX_PIXEL_FORMAT(Filter),
            .BGRG422 = MTL_GFX_PIXEL_FORMAT(Filter),
            
            .Depth16Unorm = MTL_GFX_PIXEL_FORMAT(Filter, MSAA, Resolve),
            .Depth32Float = MTL_GFX_PIXEL_FORMAT(MSAA, Resolve),
            .Stencil8 = MTL_GFX_PIXEL_FORMAT(MSAA),
            .Depth24Unorm_Stencil8 = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .Depth32Float_Stencil8 = MTL_GFX_PIXEL_FORMAT(MSAA, Resolve),
            .X24_Stencil8 = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .X32_Stencil8 = MTL_GFX_PIXEL_FORMAT(MSAA),
            
            .BGRA10_XR = MTL_GFX_PIXEL_FORMAT(All),
            .BGRA10_XR_sRGB = MTL_GFX_PIXEL_FORMAT(All),
            .BGR10_XR = MTL_GFX_PIXEL_FORMAT(All),
            .BGR10_XR_sRGB = MTL_GFX_PIXEL_FORMAT(All),
            .BGR10A2Unorm = MTL_GFX_PIXEL_FORMAT(All),
        }
    },
    {
        //MTLGFXFamilyMac1
        .functionArguments = {
            .maxVertexAttributes = 31,
            .maxBufferArguments = 31,
            .maxTextureArguments = 128,
            .maxSamplerArguments = 16,
            .maxThreadgroupMemoryArguments = 31,
            .maxInlineConstantBuffers = 14,
            .maxInlineConstantBufferLength = 4000, //4 KB
            .maxThreadsPerThreadgroup = 1024,
            .maxAllocatedThreadgroupMemory = 32000, //32 KB
            .maxAllocatedTileMemory = 0, //Not accessible,
            .threadgroupMemoryAlignment = 16, //16 B
            .maxAllocatedConstantBufferMemory = 64000, //64 KB
            .maxFragmentFunctionStageInInputs = 32,
            .maxFragmentFunctionStageInInputComponents = 124,
            .maxFunctionConstants = 65536,
            .maxTessellationFactor = 64,
            .maxVertexFunctionViewportAndScissorRectangles = 16,
            .maxFragmentFunctionRasterOrderGroups = 8,
            .maxArgumentBufferArguments = 0,
        },
        .resources = {
            .minBufferOffsetAlignment = 256, // 256 B
            .maxTextureDimensions1D = 16384, // 16384 px
            .maxTextureDimensions2D = 16384, // 16384 px
            .maxTextureDimensionsCubeMap = 16384, // 16384 px
            .maxTextureDimensions3D = 2048, // 2048 px
            .maxTextureLayers = 2048,
            .bufferAlignmentForTextureCopy = 256, // 256 B
        },
        .renderTargets = {
            .maxColourRenderTargets = 8,
            .maxPointPrimitiveSize = 511,
            .maxRenderTargetSize = SIZE_MAX, // No limit
            .maxVisibilityQueryOffset = 256000, // 256 KB
        },
        .synchronization = {
            .maxFences = 32768,
            .maxVertexAmplificationVertexCount = 0, // Not available
        },
        .pixelFormats = {
            .A8Unorm = MTL_GFX_PIXEL_FORMAT(Filter),
            .R8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .R8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .R8Snorm = MTL_GFX_PIXEL_FORMAT(All),
            .R8Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .R8Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            
            .R16Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .R16Snorm = MTL_GFX_PIXEL_FORMAT(All),
            .R16Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .R16Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .R16Float = MTL_GFX_PIXEL_FORMAT(All),
            .RG8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .RG8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .RG8Snorm = MTL_GFX_PIXEL_FORMAT(All),
            .RG8Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG8Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            
            .B5G6R5Unorm = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .A1BGR5Unorm = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .ABGR4Unorm = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .BGR5A1Unorm = MTL_GFX_PIXEL_FORMAT(Unavailable),
            
            .R32Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .R32Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .R32Float = MTL_GFX_PIXEL_FORMAT(All),
            .RG16Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .RG16Snorm = MTL_GFX_PIXEL_FORMAT(All),
            .RG16Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG16Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG16Float = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            .RGBA8Snorm = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA8Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RGBA8Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .BGRA8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .BGRA8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            
            .RGB10A2Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .RGB10A2Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG11B10Float = MTL_GFX_PIXEL_FORMAT(All),
            .RGB9E5Float = MTL_GFX_PIXEL_FORMAT(Filter),
            
            .RG32Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG32Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG32Float = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA16Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA16Snorm = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA16Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RGBA16Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RGBA16Float = MTL_GFX_PIXEL_FORMAT(All),
            
            .RGBA32Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RGBA32Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RGBA32Float = MTL_GFX_PIXEL_FORMAT(All),
            
            .PVRTC = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .EAC = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .ETC = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .ASTC = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .HDR_ASTC = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .BC = MTL_GFX_PIXEL_FORMAT(Filter),
            
            .GBGR422 = MTL_GFX_PIXEL_FORMAT(Filter),
            .BGRG422 = MTL_GFX_PIXEL_FORMAT(Filter),
            
            .Depth16Unorm = MTL_GFX_PIXEL_FORMAT(Filter, MSAA, Resolve),
            .Depth32Float = MTL_GFX_PIXEL_FORMAT(Filter, MSAA, Resolve),
            .Stencil8 = MTL_GFX_PIXEL_FORMAT(MSAA),
            .Depth24Unorm_Stencil8 = MTL_GFX_PIXEL_FORMAT(Filter, MSAA, Resolve),
            .Depth32Float_Stencil8 = MTL_GFX_PIXEL_FORMAT(Filter, MSAA, Resolve),
            .X24_Stencil8 = MTL_GFX_PIXEL_FORMAT(MSAA),
            .X32_Stencil8 = MTL_GFX_PIXEL_FORMAT(MSAA),
            
            .BGRA10_XR = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .BGRA10_XR_sRGB = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .BGR10_XR = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .BGR10_XR_sRGB = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .BGR10A2Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
        }
    },
    {
        //MTLGFXFamilyMac2
        .functionArguments = {
            .maxVertexAttributes = 31,
            .maxBufferArguments = 31,
            .maxTextureArguments = 128,
            .maxSamplerArguments = 16,
            .maxThreadgroupMemoryArguments = 31,
            .maxInlineConstantBuffers = 14,
            .maxInlineConstantBufferLength = 4000, //4 KB
            .maxThreadsPerThreadgroup = 1024,
            .maxAllocatedThreadgroupMemory = 32000, //32 KB
            .maxAllocatedTileMemory = 0, //Not accessible
            .threadgroupMemoryAlignment = 16, //16 B
            .maxAllocatedConstantBufferMemory = 64000, //64 KB
            .maxFragmentFunctionStageInInputs = 32,
            .maxFragmentFunctionStageInInputComponents = 124,
            .maxFunctionConstants = 65536,
            .maxTessellationFactor = 64,
            .maxVertexFunctionViewportAndScissorRectangles = 16,
            .maxFragmentFunctionRasterOrderGroups = 8,
            .maxArgumentBufferArguments = 0,
        },
        .resources = {
            .minBufferOffsetAlignment = 256, // 256 B
            .maxTextureDimensions1D = 16384, // 16384 px
            .maxTextureDimensions2D = 16384, // 16384 px
            .maxTextureDimensionsCubeMap = 16384, // 16384 px
            .maxTextureDimensions3D = 2048, // 2048 px
            .maxTextureLayers = 2048,
            .bufferAlignmentForTextureCopy = 256, // 256 B
        },
        .renderTargets = {
            .maxColourRenderTargets = 8,
            .maxPointPrimitiveSize = 511,
            .maxRenderTargetSize = SIZE_MAX, // No limit
            .maxVisibilityQueryOffset = 256000, // 256 KB
        },
        .synchronization = {
            .maxFences = 32768,
            .maxVertexAmplificationVertexCount = 0, // Not available
        },
        .pixelFormats = {
            .A8Unorm = MTL_GFX_PIXEL_FORMAT(Filter),
            .R8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .R8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .R8Snorm = MTL_GFX_PIXEL_FORMAT(All),
            .R8Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .R8Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            
            .R16Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .R16Snorm = MTL_GFX_PIXEL_FORMAT(All),
            .R16Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .R16Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .R16Float = MTL_GFX_PIXEL_FORMAT(All),
            .RG8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .RG8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .RG8Snorm = MTL_GFX_PIXEL_FORMAT(All),
            .RG8Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG8Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            
            .B5G6R5Unorm = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .A1BGR5Unorm = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .ABGR4Unorm = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .BGR5A1Unorm = MTL_GFX_PIXEL_FORMAT(Unavailable),
            
            .R32Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .R32Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .R32Float = MTL_GFX_PIXEL_FORMAT(All),
            .RG16Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .RG16Snorm = MTL_GFX_PIXEL_FORMAT(All),
            .RG16Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG16Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG16Float = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            .RGBA8Snorm = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA8Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RGBA8Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .BGRA8Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .BGRA8Unorm_sRGB = MTL_GFX_PIXEL_FORMAT(Filter, Colour, MSAA, Resolve, Blend),
            
            .RGB10A2Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .RGB10A2Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG11B10Float = MTL_GFX_PIXEL_FORMAT(All),
            .RGB9E5Float = MTL_GFX_PIXEL_FORMAT(Filter),
            
            .RG32Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG32Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RG32Float = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA16Unorm = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA16Snorm = MTL_GFX_PIXEL_FORMAT(All),
            .RGBA16Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RGBA16Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RGBA16Float = MTL_GFX_PIXEL_FORMAT(All),
            
            .RGBA32Uint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RGBA32Sint = MTL_GFX_PIXEL_FORMAT(Write, Colour, MSAA),
            .RGBA32Float = MTL_GFX_PIXEL_FORMAT(All),
            
            .PVRTC = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .EAC = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .ETC = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .ASTC = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .HDR_ASTC = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .BC = MTL_GFX_PIXEL_FORMAT(Filter),
            
            .GBGR422 = MTL_GFX_PIXEL_FORMAT(Filter),
            .BGRG422 = MTL_GFX_PIXEL_FORMAT(Filter),
            
            .Depth16Unorm = MTL_GFX_PIXEL_FORMAT(Filter, MSAA, Resolve),
            .Depth32Float = MTL_GFX_PIXEL_FORMAT(Filter, MSAA, Resolve),
            .Stencil8 = MTL_GFX_PIXEL_FORMAT(MSAA),
            .Depth24Unorm_Stencil8 = MTL_GFX_PIXEL_FORMAT(Filter, MSAA, Resolve),
            .Depth32Float_Stencil8 = MTL_GFX_PIXEL_FORMAT(Filter, MSAA, Resolve),
            .X24_Stencil8 = MTL_GFX_PIXEL_FORMAT(MSAA),
            .X32_Stencil8 = MTL_GFX_PIXEL_FORMAT(MSAA),
            
            .BGRA10_XR = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .BGRA10_XR_sRGB = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .BGR10_XR = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .BGR10_XR_sRGB = MTL_GFX_PIXEL_FORMAT(Unavailable),
            .BGR10A2Unorm = MTL_GFX_PIXEL_FORMAT(Filter, Write, Colour, MSAA, Resolve, Blend),
        }
    },
};

static void MTLGFXGetCapabilities(void)
{
#if CC_PLATFORM_APPLE_VERSION_MIN_REQUIRED(CC_PLATFORM_MAC_10_11, CC_PLATFORM_IOS_8_0)
    MTLGFXFamilyMapper AvailableSets[32];
    size_t SetCount = 0;
    _Bool QueryReadWriteTextureSupport = FALSE, QueryArgumentBuffersSupport = FALSE;
    
#if CC_PLATFORM_OS_X
    if (CC_AVAILABLE(macOS 10.14, *))
    {
        AvailableSets[SetCount++] = (MTLGFXFamilyMapper){ MTLFeatureSet_macOS_GPUFamily2_v1, MTLGFXFamilyMac2 };
        AvailableSets[SetCount++] = (MTLGFXFamilyMapper){ MTLFeatureSet_macOS_GPUFamily1_v4, MTLGFXFamilyMac1 };
    }
    
    if (CC_AVAILABLE(macOS 10.13, *))
    {
        AvailableSets[SetCount++] = (MTLGFXFamilyMapper){ MTLFeatureSet_macOS_GPUFamily1_v3, MTLGFXFamilyMac1 };
    }
    
    if (CC_AVAILABLE(macOS 10.12, *))
    {
        QueryArgumentBuffersSupport = [MTLInfo.device respondsToSelector: @selector(argumentBuffersSupport)];
        
        if (!(QueryReadWriteTextureSupport = [MTLInfo.device respondsToSelector: @selector(readWriteTextureSupport)]))
        {
            AvailableSets[SetCount++] = (MTLGFXFamilyMapper){ MTLFeatureSet_macOS_ReadWriteTextureTier2, MTLGFXFamilyMac1 };
        }
        
        AvailableSets[SetCount++] = (MTLGFXFamilyMapper){ MTLFeatureSet_macOS_GPUFamily1_v2, MTLGFXFamilyMac1 };
    }
    
    if (CC_AVAILABLE(macOS 10.11, *))
    {
        AvailableSets[SetCount++] = (MTLGFXFamilyMapper){ MTLFeatureSet_macOS_GPUFamily1_v1, MTLGFXFamilyMac1 };
    }
#endif
    
#if CC_PLATFORM_IOS
    if (CC_AVAILABLE(iOS 12, *))
    {
        AvailableSets[SetCount++] = (MTLGFXFamilyMapper){ MTLFeatureSet_iOS_GPUFamily4_v2, MTLGFXFamilyApple4 };
        AvailableSets[SetCount++] = (MTLGFXFamilyMapper){ MTLFeatureSet_iOS_GPUFamily3_v4, MTLGFXFamilyApple3 };
        AvailableSets[SetCount++] = (MTLGFXFamilyMapper){ MTLFeatureSet_iOS_GPUFamily2_v5, MTLGFXFamilyApple2 };
        AvailableSets[SetCount++] = (MTLGFXFamilyMapper){ MTLFeatureSet_iOS_GPUFamily1_v5, MTLGFXFamilyApple1 };
    }
    
    if (CC_AVAILABLE(iOS 11, *))
    {
        QueryArgumentBuffersSupport = [MTLInfo.device respondsToSelector: @selector(argumentBuffersSupport)];
        QueryReadWriteTextureSupport = [MTLInfo.device respondsToSelector: @selector(readWriteTextureSupport)];
        
        AvailableSets[SetCount++] = (MTLGFXFamilyMapper){ MTLFeatureSet_iOS_GPUFamily4_v1, MTLGFXFamilyApple4 };
        AvailableSets[SetCount++] = (MTLGFXFamilyMapper){ MTLFeatureSet_iOS_GPUFamily3_v3, MTLGFXFamilyApple3 };
        AvailableSets[SetCount++] = (MTLGFXFamilyMapper){ MTLFeatureSet_iOS_GPUFamily2_v4, MTLGFXFamilyApple2 };
        AvailableSets[SetCount++] = (MTLGFXFamilyMapper){ MTLFeatureSet_iOS_GPUFamily1_v4, MTLGFXFamilyApple1 };
    }
    
    if (CC_AVAILABLE(iOS 10, *))
    {
        AvailableSets[SetCount++] = (MTLGFXFamilyMapper){ MTLFeatureSet_iOS_GPUFamily3_v2, MTLGFXFamilyApple3 };
        AvailableSets[SetCount++] = (MTLGFXFamilyMapper){ MTLFeatureSet_iOS_GPUFamily2_v3, MTLGFXFamilyApple2 };
        AvailableSets[SetCount++] = (MTLGFXFamilyMapper){ MTLFeatureSet_iOS_GPUFamily1_v3, MTLGFXFamilyApple1 };
    }
    
    if (CC_AVAILABLE(iOS 9, *))
    {
        AvailableSets[SetCount++] = (MTLGFXFamilyMapper){ MTLFeatureSet_iOS_GPUFamily3_v1, MTLGFXFamilyApple3 };
        AvailableSets[SetCount++] = (MTLGFXFamilyMapper){ MTLFeatureSet_iOS_GPUFamily2_v2, MTLGFXFamilyApple2 };
        AvailableSets[SetCount++] = (MTLGFXFamilyMapper){ MTLFeatureSet_iOS_GPUFamily1_v2, MTLGFXFamilyApple1 };
    }
    
    if (CC_AVAILABLE(iOS 8, *))
    {
        AvailableSets[SetCount++] = (MTLGFXFamilyMapper){ MTLFeatureSet_iOS_GPUFamily2_v1, MTLGFXFamilyApple2 };
        AvailableSets[SetCount++] = (MTLGFXFamilyMapper){ MTLFeatureSet_iOS_GPUFamily1_v1, MTLGFXFamilyApple1 };
    }
#endif
    
    for (size_t Loop = 0; Loop < SetCount; Loop++)
    {
        if ([MTLInfo.device supportsFeatureSet: AvailableSets[Loop].set])
        {
            MTLGFXSupport.capability = Caps[AvailableSets[Loop].family];
            
            int ReadWriteTextureTier = AvailableSets[Loop].set == MTLFeatureSet_macOS_ReadWriteTextureTier2 ? 2 : 0;
#if CC_PLATFORM_APPLE_VERSION_MIN_REQUIRED(CC_PLATFORM_MAC_10_13, CC_PLATFORM_IOS_11_0)
            if (CC_AVAILABLE(macOS 10.13, iOS 11, *))
            {
                if (QueryReadWriteTextureSupport)
                {
                    switch (MTLInfo.device.readWriteTextureSupport)
                    {
                        case MTLReadWriteTextureTierNone:
                            ReadWriteTextureTier = 0;
                            break;
                            
                        case MTLReadWriteTextureTier1:
                            ReadWriteTextureTier = 1;
                            break;
                            
                        case MTLReadWriteTextureTier2:
                            ReadWriteTextureTier = 2;
                            break;
                    }
                }
            }
#endif
            
            switch (ReadWriteTextureTier)
            {
                case 2:
                    MTLGFXSupport.capability.pixelFormats.RGBA32Float |= MTLGFXPixelFormatCapabilityReadWrite;
                    MTLGFXSupport.capability.pixelFormats.RGBA32Uint |= MTLGFXPixelFormatCapabilityReadWrite;
                    MTLGFXSupport.capability.pixelFormats.RGBA32Sint |= MTLGFXPixelFormatCapabilityReadWrite;
                    MTLGFXSupport.capability.pixelFormats.RGBA16Float |= MTLGFXPixelFormatCapabilityReadWrite;
                    MTLGFXSupport.capability.pixelFormats.RGBA16Uint |= MTLGFXPixelFormatCapabilityReadWrite;
                    MTLGFXSupport.capability.pixelFormats.RGBA16Sint |= MTLGFXPixelFormatCapabilityReadWrite;
                    MTLGFXSupport.capability.pixelFormats.RGBA8Unorm |= MTLGFXPixelFormatCapabilityReadWrite;
                    MTLGFXSupport.capability.pixelFormats.RGBA8Uint |= MTLGFXPixelFormatCapabilityReadWrite;
                    MTLGFXSupport.capability.pixelFormats.RGBA8Sint |= MTLGFXPixelFormatCapabilityReadWrite;
                    MTLGFXSupport.capability.pixelFormats.R16Float |= MTLGFXPixelFormatCapabilityReadWrite;
                    MTLGFXSupport.capability.pixelFormats.R16Uint |= MTLGFXPixelFormatCapabilityReadWrite;
                    MTLGFXSupport.capability.pixelFormats.R16Sint |= MTLGFXPixelFormatCapabilityReadWrite;
                    MTLGFXSupport.capability.pixelFormats.R8Unorm |= MTLGFXPixelFormatCapabilityReadWrite;
                    MTLGFXSupport.capability.pixelFormats.R8Uint |= MTLGFXPixelFormatCapabilityReadWrite;
                    MTLGFXSupport.capability.pixelFormats.R8Sint |= MTLGFXPixelFormatCapabilityReadWrite;
                case 1:
                    MTLGFXSupport.capability.pixelFormats.R32Float |= MTLGFXPixelFormatCapabilityReadWrite;
                    MTLGFXSupport.capability.pixelFormats.R32Uint |= MTLGFXPixelFormatCapabilityReadWrite;
                    MTLGFXSupport.capability.pixelFormats.R32Sint |= MTLGFXPixelFormatCapabilityReadWrite;
                default:
                    break;
            }
            
#if CC_PLATFORM_APPLE_VERSION_MIN_REQUIRED(CC_PLATFORM_MAC_10_13, CC_PLATFORM_IOS_11_0)
            if (CC_AVAILABLE(macOS 10.13, iOS 11, *))
            {
                if (QueryArgumentBuffersSupport)
                {
                    MTLGFXSupport.capability.functionArguments.maxArgumentBufferArguments = 8;
                    
                    switch (MTLInfo.device.argumentBuffersSupport)
                    {
                        case MTLArgumentBuffersTier1:
                            /*
                             https://developer.apple.com/documentation/metal/buffers/about_argument_buffers
                             https://developer.apple.com/metal/Metal-Feature-Set-Tables.pdf
                             
                             Values recorded in the above links differ for the following families. Fairly sure the spreadsheet is a wrong, but
                             to avoid possible bugs going to play it safe and treat it as though these values only apply if argument buffers are
                             available.
                             */
                            if (AvailableSets[Loop].family >= MTLGFXFamilyMac1)
                            {
                                if (MTLGFXSupport.capability.functionArguments.maxBufferArguments < 64) MTLGFXSupport.capability.functionArguments.maxBufferArguments = 64;
                            }
                            
                            else if (AvailableSets[Loop].family >= MTLGFXFamilyApple4)
                            {
                                if (MTLGFXSupport.capability.functionArguments.maxBufferArguments < 96) MTLGFXSupport.capability.functionArguments.maxBufferArguments = 96;
                            }
                            break;
                            
                        case MTLArgumentBuffersTier2:
                            MTLGFXSupport.capability.functionArguments.maxBufferArguments = 500000;
                            MTLGFXSupport.capability.functionArguments.maxTextureArguments = 500000;
                            MTLGFXSupport.capability.functionArguments.maxSamplerArguments = 2048;
                            break;
                    }
                    
                    if (CC_AVAILABLE(macOS 10.14, iOS 12, *)) MTLGFXSupport.capability.argumentBuffers.maxUniqueSamplers = MTLInfo.device.maxArgumentBufferSamplerCount;
                    else if (AvailableSets[Loop].family >= MTLGFXFamilyMac1) MTLGFXSupport.capability.argumentBuffers.maxUniqueSamplers = 1024;
                    else MTLGFXSupport.capability.argumentBuffers.maxUniqueSamplers = 96;
                }
            }
#endif
            
            break;
        }
    }
#elif CC_PLATFORM_OS_X
    MTLGFXSupport.capability = Caps[MTLGFXFamilyMac1];
#elif CC_PLATFORM_IOS
    MTLGFXSupport.capability = Caps[MTLGFXFamilyApple1];
#endif
    
    _Bool Depth24Stencil8PixelFormatSupported = FALSE;
#if CC_PLATFORM_APPLE_VERSION_MIN_REQUIRED(CC_PLATFORM_MAC_10_11, CC_PLATFORM_IOS_NA)
    if (CC_AVAILABLE(macOS 10.11, *))
    {
        Depth24Stencil8PixelFormatSupported = MTLInfo.device.depth24Stencil8PixelFormatSupported;
    }
#endif
    
    if (!Depth24Stencil8PixelFormatSupported)
    {
        MTLGFXSupport.capability.pixelFormats.Depth24Unorm_Stencil8 = MTLGFXPixelFormatCapabilityUnavailable;
    }
}

void MTLGFXSetup(void)
{
    GFXMain = MTLGFX;
    
    @autoreleasepool {
        MTLInfo.device = (__bridge id<MTLDevice>)((__bridge_retained CFTypeRef)MTLCreateSystemDefaultDevice()); // TODO: Setup notifications to manage devices
        MTLInfo.commandQueue = (__bridge id<MTLCommandQueue>)((__bridge_retained CFTypeRef)[MTLInfo.device newCommandQueue]);
    //    MTLInfo.drawable = MTLGFXTextureCreate(CC_STD_ALLOCATOR, (id<MTLTexture>)[GFXDrawableTexture alloc]);
        
        MTLGFXGetFeatures();
        MTLGFXGetCapabilities();
        
        //TODO: Add config to specify which samplers to cache
        MTLSamplerDescriptor *SamplerDescriptor = [MTLSamplerDescriptor new];
        for (size_t Loop = 0; Loop < sizeof(MTLInfo.samplers) / sizeof(typeof(*MTLInfo.samplers)); Loop++)
        {
            GFXTextureHint Hint = (GFXTextureHint)(Loop << 2);
            SamplerDescriptor.sAddressMode = SamplerAddressMode(Hint, GFXTextureHintAddress_S);
            SamplerDescriptor.tAddressMode = SamplerAddressMode(Hint, GFXTextureHintAddress_T);
            SamplerDescriptor.rAddressMode = SamplerAddressMode(Hint, GFXTextureHintAddress_R);
            SamplerDescriptor.minFilter = SamplerFilter(Hint, GFXTextureHintFilterMin);
            SamplerDescriptor.magFilter = SamplerFilter(Hint, GFXTextureHintFilterMag);
            
            MTLInfo.samplers[Loop] = (__bridge id<MTLSamplerState>)((__bridge_retained CFTypeRef)[MTLInfo.device newSamplerStateWithDescriptor: SamplerDescriptor]);
        }
    }
}

typedef struct {
    __unsafe_unretained id <MTLDrawable>drawable;
    __unsafe_unretained id <MTLTexture>texture;
} MTLGFXDrawable;

static _Atomic(size_t) TargetReadLock = ATOMIC_VAR_INIT(0);
static _Atomic(MTLGFXDrawable) Target = ATOMIC_VAR_INIT(((MTLGFXDrawable){ nil, nil }));
void MTLGFXSetDrawable(id <MTLDrawable>Drawable, id <MTLTexture>Texture)
{
    MTLGFXDrawable NewTarget = {
        .drawable = (__bridge id<MTLDrawable>)((__bridge_retained CFTypeRef)Drawable),
        .texture = (__bridge id<MTLTexture>)((__bridge_retained CFTypeRef)Texture),
    };
    
    MTLGFXDrawable PrevTarget = atomic_exchange_explicit(&Target, NewTarget, memory_order_release);
    
    while (atomic_load_explicit(&TargetReadLock, memory_order_relaxed)) CC_SPIN_WAIT();
    
    atomic_thread_fence(memory_order_acq_rel);
    
    if (PrevTarget.drawable) CFRelease((__bridge CFTypeRef)PrevTarget.drawable);
    if (PrevTarget.texture) CFRelease((__bridge CFTypeRef)PrevTarget.texture);
}

id <MTLDrawable>MTLGFXGetDrawable(void)
{
    atomic_fetch_add_explicit(&TargetReadLock, 1, memory_order_acquire);
    MTLGFXDrawable CurrentTarget = atomic_load_explicit(&Target, memory_order_relaxed);
    id <MTLDrawable>Drawable = CurrentTarget.drawable;
    atomic_fetch_sub_explicit(&TargetReadLock, 1, memory_order_release);
    
    return Drawable;
}

id <MTLTexture>MTLGFXGetDrawableTexture(void)
{
    atomic_fetch_add_explicit(&TargetReadLock, 1, memory_order_acquire);
    MTLGFXDrawable CurrentTarget = atomic_load_explicit(&Target, memory_order_relaxed);
    id <MTLTexture>Texture = CurrentTarget.texture;
    atomic_fetch_sub_explicit(&TargetReadLock, 1, memory_order_release);
    
    return Texture;
}

@implementation GFXDrawableTexture

-(NSMethodSignature*) methodSignatureForSelector: (SEL)sel
{
    id <MTLTexture>DrawableTexture = MTLGFXGetDrawableTexture();
    return [NSMethodSignature signatureWithObjCTypes: DrawableTexture ? method_getTypeEncoding(class_getInstanceMethod([DrawableTexture class], sel)) : protocol_getMethodDescription(@protocol(MTLTexture), sel, YES, YES).types];
}

-(void) forwardInvocation: (NSInvocation*)invocation
{
    [invocation invokeWithTarget: MTLGFXGetDrawableTexture()];
}

@end
