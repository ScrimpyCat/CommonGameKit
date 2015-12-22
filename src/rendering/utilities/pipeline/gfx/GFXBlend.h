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

#ifndef Blob_Game_GFXBlend_h
#define Blob_Game_GFXBlend_h

typedef enum {
    GFXBlendOperationMask = (7 << 16),
    GFXBlendOperationAdd = (0 << 16),
    GFXBlendOperationSubtract = (1 << 16),
    GFXBlendOperationReverseSubtract = (2 << 16),
    GFXBlendOperationMin = (3 << 16),
    GFXBlendOperationMax = (4 << 16),
    
    GFXBlendFactorMask = 0xf,
    GFXBlendFactorZero = 0,
    GFXBlendFactorOne,
    GFXBlendFactorSourceColour,
    GFXBlendFactorOneMinusSourceColor,
    GFXBlendFactorSourceAlpha,
    GFXBlendFactorOneMinusSourceAlpha,
    GFXBlendFactorDestinationColor,
    GFXBlendFactorOneMinusDestinationColor,
    GFXBlendFactorDestinationAlpha,
    GFXBlendFactorOneMinusDestinationAlpha,
    GFXBlendFactorSourceAlphaSaturated,
    GFXBlendFactorBlendColor,
    GFXBlendFactorOneMinusBlendColor,
    GFXBlendFactorBlendAlpha,
    GFXBlendFactorOneMinusBlendAlpha,
    
    GFXBlendComponentRGB = 4,
    GFXBlendComponentAlpha = 0,
    GFXBlendSource = 8,
    GFXBlendDestination = 0,
    
    GFXBlendOpaque = (GFXBlendOperationAdd << GFXBlendComponentRGB)
                   | (GFXBlendOperationAdd << GFXBlendComponentAlpha)
                   | (((GFXBlendFactorOne << GFXBlendComponentRGB) | (GFXBlendFactorOne << GFXBlendComponentAlpha)) << GFXBlendSource)
                   | (((GFXBlendFactorZero << GFXBlendComponentRGB) | (GFXBlendFactorZero << GFXBlendComponentAlpha)) << GFXBlendDestination),
    
    GFXBlendTransparent = (GFXBlendOperationAdd << GFXBlendComponentRGB)
                        | (GFXBlendOperationAdd << GFXBlendComponentAlpha)
                        | (((GFXBlendFactorSourceAlpha << GFXBlendComponentRGB) | (GFXBlendFactorSourceAlpha << GFXBlendComponentAlpha)) << GFXBlendSource)
                        | (((GFXBlendFactorOneMinusSourceAlpha << GFXBlendComponentRGB) | (GFXBlendFactorOneMinusSourceAlpha << GFXBlendComponentAlpha)) << GFXBlendDestination)
} GFXBlend;

/*!
 * @brief Get the operation mask for either the source or the destination.
 * @param BlendMask The blend mask.
 * @param Component The component to get, either @b GFXBlendComponentRGB or @b GFXBlendComponentAlpha.
 * @return The blend operation.
 *         @b GFXBlendOperationAdd
 *         @b GFXBlendOperationSubtract
 *         @b GFXBlendOperationReverseSubtract
 *         @b GFXBlendOperationMin
 *         @b GFXBlendOperationMax
 *
 */
static inline GFXBlend GFXBlendGetOperation(GFXBlend BlendMask, GFXBlend Component);

/*!
 * @brief Get the operation mask for either the source or the destination.
 * @param BlendMask The blend mask.
 * @param Component The component to get, either @b GFXBlendComponentRGB or @b GFXBlendComponentAlpha.
 * @param SrcOrDst The type to get, either @b GFXBlendSource or @b GFXBlendDestination.
 * @return The blend factor.
 *         @b GFXBlendFactorZero
 *         @b GFXBlendFactorOne
 *         @b GFXBlendFactorSourceColour
 *         @b GFXBlendFactorOneMinusSourceColor
 *         @b GFXBlendFactorSourceAlpha
 *         @b GFXBlendFactorOneMinusSourceAlpha
 *         @b GFXBlendFactorDestinationColor
 *         @b GFXBlendFactorOneMinusDestinationColor
 *         @b GFXBlendFactorDestinationAlpha
 *         @b GFXBlendFactorOneMinusDestinationAlpha
 *         @b GFXBlendFactorSourceAlphaSaturated
 *         @b GFXBlendFactorBlendColor
 *         @b GFXBlendFactorOneMinusBlendColor
 *         @b GFXBlendFactorBlendAlpha
 *         @b GFXBlendFactorOneMinusBlendAlpha
 *
 */
static inline GFXBlend GFXBlendGetFactor(GFXBlend BlendMask, GFXBlend Component, GFXBlend SrcOrDst);


static inline GFXBlend GFXBlendGetOperation(GFXBlend BlendMask, GFXBlend Component)
{
    return (BlendMask >> Component) & GFXBlendOperationMask;
}

static inline GFXBlend GFXBlendGetFactor(GFXBlend BlendMask, GFXBlend Component, GFXBlend SrcOrDst)
{
    return ((BlendMask >> Component) >> SrcOrDst) & GFXBlendFactorMask;
}

#endif
