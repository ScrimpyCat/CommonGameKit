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

#import "MTLDraw.h"
#import "MTLTexture.h"
#import "MTLShader.h"


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
            return MTLVertexFormatUChar;
            
        case GFXBufferFormatInt8:
            return MTLVertexFormatChar;
            
        case GFXBufferFormatUInt8Normalized:
            return MTLVertexFormatUCharNormalized;
            
        case GFXBufferFormatInt8Normalized:
            return MTLVertexFormatCharNormalized;
            
        case GFXBufferFormatUInt16:
            return MTLVertexFormatUShort;
            
        case GFXBufferFormatInt16:
            return MTLVertexFormatShort;
            
        case GFXBufferFormatUInt16Normalized:
            return MTLVertexFormatUShortNormalized;
            
        case GFXBufferFormatInt16Normalized:
            return MTLVertexFormatShortNormalized;
            
        case GFXBufferFormatFloat16:
            return MTLVertexFormatHalf;
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
