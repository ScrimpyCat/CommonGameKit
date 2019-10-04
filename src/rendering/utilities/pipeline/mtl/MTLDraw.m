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
#import "MTLDraw.h"
#import "MTLTexture.h"
#import "MTLFramebuffer.h"
#import "MTLShader.h"
#import "MTLBuffer.h"
#import "MTLBlit.h"
#import "MTLCommandBuffer.h"

static void DrawSubmit(GFXDraw Draw, GFXPrimitiveType Primitive, size_t Offset, size_t Count);
static void DrawSubmitIndexed(GFXDraw Draw, GFXPrimitiveType Primitive, size_t Offset, size_t Count);


const GFXDrawInterface MTLDrawInterface = {
    .submit = DrawSubmit,
    .indexedSubmit = DrawSubmitIndexed,
    .optional = {
        .create = NULL,
        .destroy = NULL,
        .setShader = NULL,
        .setFramebuffer = NULL,
        .setIndexBuffer = NULL,
        .setVertexBuffer = NULL,
        .setBuffer = NULL,
        .setTexture = NULL,
        .setBlend = NULL,
        .setViewport = NULL
    }
};


static CC_CONSTANT_FUNCTION MTLBlendFactor DrawBlendFactor(GFXBlend Factor)
{
    switch (Factor)
    {
        case GFXBlendFactorZero:
            return MTLBlendFactorZero;
            
        case GFXBlendFactorOne:
            return MTLBlendFactorOne;
            
        case GFXBlendFactorSourceColour:
            return MTLBlendFactorSourceColor;
            
        case GFXBlendFactorOneMinusSourceColor:
            return MTLBlendFactorOneMinusSourceColor;
            
        case GFXBlendFactorSourceAlpha:
            return MTLBlendFactorSourceAlpha;
            
        case GFXBlendFactorOneMinusSourceAlpha:
            return MTLBlendFactorOneMinusSourceAlpha;
            
        case GFXBlendFactorDestinationColor:
            return MTLBlendFactorDestinationColor;
            
        case GFXBlendFactorOneMinusDestinationColor:
            return MTLBlendFactorOneMinusDestinationColor;
            
        case GFXBlendFactorDestinationAlpha:
            return MTLBlendFactorDestinationAlpha;
            
        case GFXBlendFactorOneMinusDestinationAlpha:
            return MTLBlendFactorOneMinusDestinationAlpha;
            
        case GFXBlendFactorSourceAlphaSaturated:
            return MTLBlendFactorSourceAlphaSaturated;
            
        case GFXBlendFactorBlendColor:
            return MTLBlendFactorBlendColor;
            
        case GFXBlendFactorOneMinusBlendColor:
            return MTLBlendFactorOneMinusBlendColor;
            
        case GFXBlendFactorBlendAlpha:
            return MTLBlendFactorBlendAlpha;
            
        case GFXBlendFactorOneMinusBlendAlpha:
            return MTLBlendFactorOneMinusBlendAlpha;
            
        default:
            break;
    }
    
    CCAssertLog(0, "Unsupported factor");
}

static CC_CONSTANT_FUNCTION MTLBlendOperation DrawBlendOperation(GFXBlend Operation)
{
    switch (Operation)
    {
        case GFXBlendOperationAdd:
            return MTLBlendOperationAdd;
            
        case GFXBlendOperationSubtract:
            return MTLBlendOperationSubtract;
            
        case GFXBlendOperationReverseSubtract:
            return MTLBlendOperationReverseSubtract;
            
        case GFXBlendOperationMin:
            return MTLBlendOperationMin;
            
        case GFXBlendOperationMax:
            return MTLBlendOperationMax;
            
        default:
            break;
    }
    
    CCAssertLog(0, "Unsupported operation");
}

static CC_CONSTANT_FUNCTION MTLVertexFormat DrawVertexFormat(GFXBufferFormat Format)
{
    switch (Format)
    {
        case GFXBufferFormatUInt8x2:
            return MTLVertexFormatUChar2;
            
        case GFXBufferFormatUInt8x3:
            return MTLVertexFormatUChar3;
            
        case GFXBufferFormatUInt8x4:
            return MTLVertexFormatUChar4;
            
        case GFXBufferFormatInt8x2:
            return MTLVertexFormatChar2;
            
        case GFXBufferFormatInt8x3:
            return MTLVertexFormatChar3;
            
        case GFXBufferFormatInt8x4:
            return MTLVertexFormatChar4;
            
        case GFXBufferFormatUInt8x2Normalized:
            return MTLVertexFormatUChar2Normalized;
            
        case GFXBufferFormatUInt8x3Normalized:
            return MTLVertexFormatUChar3Normalized;
            
        case GFXBufferFormatUInt8x4Normalized:
            return MTLVertexFormatUChar4Normalized;
            
        case GFXBufferFormatInt8x2Normalized:
            return MTLVertexFormatChar2Normalized;
            
        case GFXBufferFormatInt8x3Normalized:
            return MTLVertexFormatChar3Normalized;
            
        case GFXBufferFormatInt8x4Normalized:
            return MTLVertexFormatChar4Normalized;
            
        case GFXBufferFormatUInt16x2:
            return MTLVertexFormatUShort2;
            
        case GFXBufferFormatUInt16x3:
            return MTLVertexFormatUShort3;
            
        case GFXBufferFormatUInt16x4:
            return MTLVertexFormatUShort4;
            
        case GFXBufferFormatInt16x2:
            return MTLVertexFormatShort2;
            
        case GFXBufferFormatInt16x3:
            return MTLVertexFormatShort3;
            
        case GFXBufferFormatInt16x4:
            return MTLVertexFormatShort4;
            
        case GFXBufferFormatUInt16x2Normalized:
            return MTLVertexFormatUShort2Normalized;
            
        case GFXBufferFormatUInt16x3Normalized:
            return MTLVertexFormatUShort3Normalized;
            
        case GFXBufferFormatUInt16x4Normalized:
            return MTLVertexFormatUShort4Normalized;
            
        case GFXBufferFormatInt16x2Normalized:
            return MTLVertexFormatShort2Normalized;
            
        case GFXBufferFormatInt16x3Normalized:
            return MTLVertexFormatShort3Normalized;
            
        case GFXBufferFormatInt16x4Normalized:
            return MTLVertexFormatShort4Normalized;
            
        case GFXBufferFormatFloat16x2:
            return MTLVertexFormatHalf2;
            
        case GFXBufferFormatFloat16x3:
            return MTLVertexFormatHalf3;
            
        case GFXBufferFormatFloat16x4:
            return MTLVertexFormatHalf4;
            
        case GFXBufferFormatFloat32:
            return MTLVertexFormatFloat;
            
        case GFXBufferFormatFloat32x2:
            return MTLVertexFormatFloat2;
            
        case GFXBufferFormatFloat32x3:
            return MTLVertexFormatFloat3;
            
        case GFXBufferFormatFloat32x4:
            return MTLVertexFormatFloat4;
            
        case GFXBufferFormatInt32:
            return MTLVertexFormatInt;
            
        case GFXBufferFormatInt32x2:
            return MTLVertexFormatInt2;
            
        case GFXBufferFormatInt32x3:
            return MTLVertexFormatInt3;
            
        case GFXBufferFormatInt32x4:
            return MTLVertexFormatInt4;
            
        case GFXBufferFormatUInt32:
            return MTLVertexFormatUInt;
            
        case GFXBufferFormatUInt32x2:
            return MTLVertexFormatUInt2;
            
        case GFXBufferFormatUInt32x3:
            return MTLVertexFormatUInt3;
            
        case GFXBufferFormatUInt32x4:
            return MTLVertexFormatUInt4;
            
#if CC_PLATFORM_APPLE_VERSION_MIN_REQUIRED(CC_PLATFORM_MAC_10_13, CC_PLATFORM_IOS_11_0)
        case GFXBufferFormatUInt8:
            if (CC_AVAILABLE(macOS 10.13, iOS 11, *)) return MTLVertexFormatUChar;
            
        case GFXBufferFormatInt8:
            if (CC_AVAILABLE(macOS 10.13, iOS 11, *)) return MTLVertexFormatChar;
            
        case GFXBufferFormatUInt8Normalized:
            if (CC_AVAILABLE(macOS 10.13, iOS 11, *)) return MTLVertexFormatUCharNormalized;
            
        case GFXBufferFormatInt8Normalized:
            if (CC_AVAILABLE(macOS 10.13, iOS 11, *)) return MTLVertexFormatCharNormalized;
            
        case GFXBufferFormatUInt16:
            if (CC_AVAILABLE(macOS 10.13, iOS 11, *)) return MTLVertexFormatUShort;
            
        case GFXBufferFormatInt16:
            if (CC_AVAILABLE(macOS 10.13, iOS 11, *)) return MTLVertexFormatShort;
            
        case GFXBufferFormatUInt16Normalized:
            if (CC_AVAILABLE(macOS 10.13, iOS 11, *)) return MTLVertexFormatUShortNormalized;
            
        case GFXBufferFormatInt16Normalized:
            if (CC_AVAILABLE(macOS 10.13, iOS 11, *)) return MTLVertexFormatShortNormalized;
            
        case GFXBufferFormatFloat16:
            if (CC_AVAILABLE(macOS 10.13, iOS 11, *)) return MTLVertexFormatHalf;
#endif
            
        default:
            break;
    }
    
    CCAssertLog(0, "Unsupported format");
}

static CC_CONSTANT_FUNCTION MTLPrimitiveType DrawPrimitiveType(GFXPrimitiveType Primitive)
{
    switch (Primitive)
    {
        case GFXPrimitiveTypePoint:
            return MTLPrimitiveTypePoint;
            
        case GFXPrimitiveTypeLine:
            return MTLPrimitiveTypeLine;
            
        case GFXPrimitiveTypeLineStrip:
            return MTLPrimitiveTypeLineStrip;
            
        case GFXPrimitiveTypeTriangle:
            return MTLPrimitiveTypeTriangle;
            
        case GFXPrimitiveTypeTriangleStrip:
            return MTLPrimitiveTypeTriangleStrip;
            
        default:
            break;
    }
    
    CCAssertLog(0, "Unsupported primitive");
}

static CC_CONSTANT_FUNCTION MTLLoadAction DrawLoadAction(GFXFramebufferAttachmentAction Action)
{
    if (Action & GFXFramebufferAttachmentActionFlagClearOnce) return MTLLoadActionClear;
    
    switch (Action)
    {
        case GFXFramebufferAttachmentActionDontCare:
            return MTLLoadActionDontCare;
            
        case GFXFramebufferAttachmentActionClear:
            return MTLLoadActionClear;
            
        case GFXFramebufferAttachmentActionLoad:
            return MTLLoadActionLoad;
            
        default:
            break;
    }
    
    CCAssertLog(0, "Unsupported load action");
}

static CC_CONSTANT_FUNCTION MTLStoreAction DrawStoreAction(GFXFramebufferAttachmentAction Action)
{
    if (Action & GFXFramebufferAttachmentActionFlagClearOnce) return MTLStoreActionDontCare;
    
    switch (Action)
    {
        case GFXFramebufferAttachmentActionDontCare:
            return MTLStoreActionDontCare;
            
        case GFXFramebufferAttachmentActionClear:
            return MTLStoreActionDontCare;
            
        case GFXFramebufferAttachmentActionStore:
            return MTLStoreActionStore;
            
        default:
            break;
    }
    
    CCAssertLog(0, "Unsupported store action");
}

static MTLIndexType IndexTypeTypeFromBufferFormat(GFXBufferFormat Format)
{
    if ((GFXBufferFormatIsInteger(Format)) && (GFXBufferFormatIsUnsigned(Format)))
    {
        switch (GFXBufferFormatGetBitSize(Format))
        {
            case 16:
                return MTLIndexTypeUInt16;
                
            case 32:
                return MTLIndexTypeUInt32;
        }
    }
    
    CCAssertLog(0, "Unsupported index format %d", Format);
}

static CCComparisonResult GFXDrawFindInput(const GFXDrawInput *left, const GFXDrawInput *right)
{
    return CCStringEqual(left->name, right->name) ? CCComparisonResultEqual : CCComparisonResultInvalid;
}

static void MTLGFXDraw(GFXDraw Draw, GFXPrimitiveType Primitive, size_t Offset, size_t Count, _Bool Indexed)
{
    @autoreleasepool {
        GFXFramebufferAttachment *Attachment = GFXFramebufferGetAttachment(Draw->destination.framebuffer, Draw->destination.index);
        
        MTLRenderPipelineDescriptor *RenderPipelineDescriptor = [MTLRenderPipelineDescriptor new];
        RenderPipelineDescriptor.colorAttachments[0].pixelFormat = MTLGFXFramebufferAttachmentGetTexture(Attachment).pixelFormat;
        RenderPipelineDescriptor.colorAttachments[0].blendingEnabled = Draw->blending != GFXBlendOpaque;
        RenderPipelineDescriptor.colorAttachments[0].sourceRGBBlendFactor = DrawBlendFactor(GFXBlendGetFactor(Draw->blending, GFXBlendComponentRGB, GFXBlendSource));
        RenderPipelineDescriptor.colorAttachments[0].sourceAlphaBlendFactor = DrawBlendFactor(GFXBlendGetFactor(Draw->blending, GFXBlendComponentAlpha, GFXBlendSource));
        RenderPipelineDescriptor.colorAttachments[0].destinationRGBBlendFactor = DrawBlendFactor(GFXBlendGetFactor(Draw->blending, GFXBlendComponentRGB, GFXBlendDestination));
        RenderPipelineDescriptor.colorAttachments[0].destinationAlphaBlendFactor = DrawBlendFactor(GFXBlendGetFactor(Draw->blending, GFXBlendComponentAlpha, GFXBlendDestination));
        RenderPipelineDescriptor.colorAttachments[0].rgbBlendOperation = DrawBlendOperation(GFXBlendGetOperation(Draw->blending, GFXBlendComponentRGB));
        RenderPipelineDescriptor.colorAttachments[0].alphaBlendOperation = DrawBlendOperation(GFXBlendGetOperation(Draw->blending, GFXBlendComponentAlpha));
        
        id <MTLFunction>Vert = ((MTLGFXShader)Draw->shader)->vert;
        RenderPipelineDescriptor.vertexFunction = Vert;
        
        MTLVertexDescriptor *VertexDescriptor = [MTLVertexDescriptor vertexDescriptor];
        NSUInteger Index = 0, Stride = 0;
        GFXBuffer SharedVertexBuffer = NULL;
        for (MTLVertexAttribute *Attribute in Vert.vertexAttributes)
        {
            CCString Input = CCStringCreate(CC_STD_ALLOCATOR, (CCStringHint)CCStringEncodingUTF8, [Attribute.name UTF8String]);
            GFXDrawInputVertexBuffer *VertexBuffer = CCCollectionGetElement(Draw->vertexBuffers, CCCollectionFindElement(Draw->vertexBuffers, &(GFXDrawInput){ .name = Input }, (CCComparator)GFXDrawFindInput));
            
            if (VertexBuffer)
            {
                MTLVertexAttributeDescriptor *VertexAttributeDescriptor = [MTLVertexAttributeDescriptor new];
                VertexAttributeDescriptor.format = DrawVertexFormat(VertexBuffer->format);
                VertexAttributeDescriptor.offset = VertexBuffer->offset;
                VertexAttributeDescriptor.bufferIndex = 0;
                [VertexDescriptor.attributes setObject: VertexAttributeDescriptor atIndexedSubscript: Index];
                
                if (Index++)
                {
                    CCAssertLog(Stride == VertexBuffer->stride, "Strides must match");
                    CCAssertLog(SharedVertexBuffer == VertexBuffer->buffer, "Buffer must be the same");
                }
                
                else
                {
                    Stride = VertexBuffer->stride;
                    SharedVertexBuffer = VertexBuffer->buffer;
                }
            }
            
            CCStringDestroy(Input);
        }
        
        if (Index)
        {
            MTLVertexBufferLayoutDescriptor *LayoutDescriptor = [MTLVertexBufferLayoutDescriptor new];
            LayoutDescriptor.stride = Stride;
            LayoutDescriptor.stepFunction = MTLVertexStepFunctionPerVertex;
            LayoutDescriptor.stepRate = 1;
            [VertexDescriptor.layouts setObject: LayoutDescriptor atIndexedSubscript: 0];
            
            RenderPipelineDescriptor.vertexDescriptor = VertexDescriptor;
        }
        
        RenderPipelineDescriptor.fragmentFunction = ((MTLGFXShader)Draw->shader)->frag;
        
        MTLAutoreleasedRenderPipelineReflection Reflection;
        id <MTLRenderPipelineState>RenderPipelineState = [((MTLInternal*)MTLGFX->internal)->device newRenderPipelineStateWithDescriptor: RenderPipelineDescriptor options: MTLPipelineOptionArgumentInfo | MTLPipelineOptionBufferTypeInfo reflection: &Reflection error: NULL];
        
        MTLRenderPassDescriptor *RenderPassDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];
        RenderPassDescriptor.colorAttachments[0].texture = MTLGFXFramebufferAttachmentGetTexture(Attachment);
        RenderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(Attachment->colour.clear.r, Attachment->colour.clear.g, Attachment->colour.clear.b, Attachment->colour.clear.a);
        RenderPassDescriptor.colorAttachments[0].loadAction = DrawLoadAction(Attachment->load);
        RenderPassDescriptor.colorAttachments[0].storeAction = DrawStoreAction(Attachment->store);
        
        const _Bool ClearStore = (Attachment->store & GFXFramebufferAttachmentActionFlagClearOnce) || ((Attachment->store & ~GFXFramebufferAttachmentActionFlagClearOnce) == GFXFramebufferAttachmentActionClear);
        Attachment->load &= ~GFXFramebufferAttachmentActionFlagClearOnce;
        Attachment->store &= ~GFXFramebufferAttachmentActionFlagClearOnce;
        
        id <MTLRenderCommandEncoder>RenderCommand = [((MTLGFXCommandBuffer)GFXCommandBufferRecording())->commandBuffer renderCommandEncoderWithDescriptor:  RenderPassDescriptor];
        
        const GFXViewport Viewport = GFXDrawGetViewport(Draw);
        [RenderCommand setViewport: (MTLViewport){ Viewport.x, Viewport.y, Viewport.width, Viewport.height, Viewport.minDepth, Viewport.maxDepth }];
        [RenderCommand setRenderPipelineState: RenderPipelineState];
        
        for (MTLArgument *Arg in Reflection.vertexArguments)
        {
            switch (Arg.type)
            {
                case MTLArgumentTypeBuffer:
                    if ([Arg.name hasPrefix: @"vertexBuffer."])
                    {
                        CCAssertLog(SharedVertexBuffer, "Vertex buffer must not be null for stage_in");
                        [RenderCommand setVertexBuffer: MTLGFXBufferGetBuffer((MTLGFXBuffer)SharedVertexBuffer) offset: 0 atIndex: Arg.index];
                    }
                    
                    else
                    {
                        CCString Input = CCStringCreate(CC_STD_ALLOCATOR, (CCStringHint)CCStringEncodingUTF8, [Arg.name UTF8String]);
                        GFXDrawInputBuffer *Buffer = CCCollectionGetElement(Draw->buffers, CCCollectionFindElement(Draw->buffers, &(GFXDrawInput){ .name = Input }, (CCComparator)GFXDrawFindInput));
                        
                        if (Buffer)
                        {
                            
                            [RenderCommand setVertexBuffer: MTLGFXBufferGetBuffer((MTLGFXBuffer)Buffer->buffer) offset: 0 atIndex: Arg.index];
                        }
                        
                        CCStringDestroy(Input);
                    }
                    break;
                    
                case MTLArgumentTypeTexture:
                {
                    CCString Input = CCStringCreate(CC_STD_ALLOCATOR, (CCStringHint)CCStringEncodingUTF8, [Arg.name UTF8String]);
                    GFXDrawInputTexture *Texture = CCCollectionGetElement(Draw->textures, CCCollectionFindElement(Draw->textures, &(GFXDrawInput){ .name = Input }, (CCComparator)GFXDrawFindInput));
                    
                    if (Texture)
                    {
                        [RenderCommand setVertexTexture: MTLGFXTextureGetTexture((MTLGFXTexture)Texture->texture)  atIndex: Arg.index];
                        [RenderCommand setVertexSamplerState: MTLGFXGetSampler(GFXTextureGetHints(Texture->texture)) atIndex: Arg.index];
                    }
                    
                    CCStringDestroy(Input);
                    
                    break;
                }
                
                case MTLArgumentTypeSampler:
                    break;
                    
                default:
                    CCAssertLog(0, "Unsupported vertex argument type");
                    break;
            }
        }
        
        for (MTLArgument *Arg in Reflection.fragmentArguments)
        {
            switch (Arg.type)
            {
                case MTLArgumentTypeBuffer:
                {
                    CCString Input = CCStringCreate(CC_STD_ALLOCATOR, (CCStringHint)CCStringEncodingUTF8, [Arg.name UTF8String]);
                    GFXDrawInputBuffer *Buffer = CCCollectionGetElement(Draw->buffers, CCCollectionFindElement(Draw->buffers, &(GFXDrawInput){ .name = Input }, (CCComparator)GFXDrawFindInput));
                    
                    if (Buffer)
                    {
                        
                        [RenderCommand setFragmentBuffer: MTLGFXBufferGetBuffer((MTLGFXBuffer)Buffer->buffer) offset: 0 atIndex: Arg.index];
                    }
                    
                    CCStringDestroy(Input);
                    
                    break;
                }
                    
                case MTLArgumentTypeTexture:
                {
                    CCString Input = CCStringCreate(CC_STD_ALLOCATOR, (CCStringHint)CCStringEncodingUTF8, [Arg.name UTF8String]);
                    GFXDrawInputTexture *Texture = CCCollectionGetElement(Draw->textures, CCCollectionFindElement(Draw->textures, &(GFXDrawInput){ .name = Input }, (CCComparator)GFXDrawFindInput));
                    
                    if (Texture)
                    {
                        [RenderCommand setFragmentTexture: MTLGFXTextureGetTexture((MTLGFXTexture)Texture->texture)  atIndex: Arg.index];
                        [RenderCommand setFragmentSamplerState: MTLGFXGetSampler(GFXTextureGetHints(Texture->texture)) atIndex: Arg.index];
                    }
                    
                    CCStringDestroy(Input);
                    
                    break;
                }
                
                case MTLArgumentTypeSampler:
                    break;
                    
                default:
                    CCAssertLog(0, "Unsupported fragment argument type");
                    break;
            }
        }
        
        if (Indexed)
        {
            [RenderCommand drawIndexedPrimitives: DrawPrimitiveType(Primitive) indexCount: Count indexType: IndexTypeTypeFromBufferFormat(Draw->index.format) indexBuffer: MTLGFXBufferGetBuffer((MTLGFXBuffer)Draw->index.buffer) indexBufferOffset: Offset];
        }
        
        else
        {
            [RenderCommand drawPrimitives: DrawPrimitiveType(Primitive) vertexStart: Offset vertexCount: Count];
        }
        
        [RenderCommand endEncoding];
        
        
        if (ClearStore) [MTLGFXClearEncoder(Attachment) endEncoding];
    }
}

static void DrawSubmit(GFXDraw Draw, GFXPrimitiveType Primitive, size_t Offset, size_t Count)
{
    MTLGFXDraw(Draw, Primitive, Offset, Count, FALSE);
}

static void DrawSubmitIndexed(GFXDraw Draw, GFXPrimitiveType Primitive, size_t Offset, size_t Count)
{
    MTLGFXDraw(Draw, Primitive, Offset, Count, TRUE);
}
