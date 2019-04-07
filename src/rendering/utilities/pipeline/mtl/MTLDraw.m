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
