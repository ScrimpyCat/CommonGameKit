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

#include "GFXTexture.h"
#include "GFXMain.h"

GFXTexture GFXTextureCreate(CCAllocatorType Allocator, GFXTextureHint Hint, CCColourFormat Format, size_t Width, size_t Height, size_t Depth, CCPixelData Data)
{
    return GFXMain->texture->create(Allocator, Hint, Format, Width, Height, Depth, Data);
}

GFXTexture GFXTextureCreateSubTexture(CCAllocatorType Allocator, GFXTexture Texture, size_t X, size_t Y, size_t Z, size_t Width, size_t Height, size_t Depth, CCPixelData Data)
{
    CCAssertLog(Texture, "Texture must not be null");
    
    size_t W, H, D;
    GFXTextureGetSize(Texture, &W, &H, &D);
    
    CCAssertLog(((X + Width) <= W) && ((Y + Height) <= H) && ((Z + Depth) <= D), "Sub texture must not exceed bounds of parent texture");
    CCAssertLog(((X + Width) > X) && ((Y + Height) > Y) && ((Z + Depth) > Z), "Sub texture bounds must be greater than 1 or must not overflow");
    
    return GFXMain->texture->createSub(Allocator, CCRetain(Texture), X, Y, Z, Width, Height, Depth, Data);
}

void GFXTextureDestroy(GFXTexture Texture)
{
    CCAssertLog(Texture, "Texture must not be null");
    
    GFXMain->texture->destroy(Texture);
}

GFXTexture GFXTextureGetParent(GFXTexture Texture)
{
    CCAssertLog(Texture, "Texture must not be null");
    
    return GFXMain->texture->parent(Texture);
}

GFXTextureHint GFXTextureGetHints(GFXTexture Texture)
{
    CCAssertLog(Texture, "Texture must not be null");
    
    return GFXMain->texture->hints(Texture);
}

CCPixelData GFXTextureGetData(GFXTexture Texture)
{
    CCAssertLog(Texture, "Texture must not be null");
    
    return GFXMain->texture->data(Texture);
}

void GFXTextureGetOffset(GFXTexture Texture, size_t *X, size_t *Y, size_t *Z)
{
    CCAssertLog(Texture, "Texture must not be null");
    
    GFXMain->texture->offset(Texture, X, Y, Z);
}

void GFXTextureGetInternalOffset(GFXTexture Texture, size_t *X, size_t *Y, size_t *Z)
{
    CCAssertLog(Texture, "Texture must not be null");
    
    if (GFXMain->texture->optional.internalOffset) GFXMain->texture->optional.internalOffset(Texture, X, Y, Z);
    else
    {
        size_t RealX = 0, RealY = 0, RealZ = 0;
        while (Texture)
        {
            size_t CurX, CurY, CurZ;
            GFXTextureGetOffset(Texture, &CurX, &CurY, &CurZ);
            
            RealX += CurX;
            RealY += CurY;
            RealZ += CurZ;
            
            Texture = GFXTextureGetParent(Texture);
        }
        
        *X = RealX;
        *Y = RealY;
        *Z = RealZ;
    }
}

void GFXTextureGetSize(GFXTexture Texture, size_t *Width, size_t *Height, size_t *Depth)
{
    CCAssertLog(Texture, "Texture must not be null");
    
    GFXMain->texture->size(Texture, Width, Height, Depth);
}

void GFXTextureGetBounds(GFXTexture Texture, CCVector3D *Bottom, CCVector3D *Top)
{
    CCAssertLog(Texture, "Texture must not be null");
    
    size_t Width, Height, Depth;
    GFXTextureGetSize(Texture, &Width, &Height, &Depth);
    
    size_t RealX = 0, RealY = 0, RealZ = 0;
    size_t RealW = 0, RealH = 0, RealD = 0;
    while (Texture)
    {
        size_t X, Y, Z;
        GFXTextureGetOffset(Texture, &X, &Y, &Z);
        
        RealX += X;
        RealY += Y;
        RealZ += Z;
        
        GFXTextureGetSize(Texture, &RealW, &RealH, &RealD);
        
        Texture = GFXTextureGetParent(Texture);
    }
    
    if (Bottom) *Bottom = CCVector3DMake((float)RealX / (float)RealW,
                                         (float)RealY / (float)RealH,
                                         (float)RealZ / (float)RealD);
    
    if (Top) *Top = CCVector3DMake((float)(RealX + Width) / (float)RealW,
                                   (float)(RealY + Height) / (float)RealH,
                                   (float)(RealZ + Depth) / (float)RealD);
}

CCVector3D GFXTextureGetMultiplier(GFXTexture Texture)
{
    CCAssertLog(Texture, "Texture must not be null");
    
    size_t RealW = 0, RealH = 0, RealD = 0;
    while (Texture)
    {
        GFXTextureGetSize(Texture, &RealW, &RealH, &RealD);
        
        Texture = GFXTextureGetParent(Texture);
    }
    
    return CCVector3DMake(1.0f / (float)RealW,
                          1.0f / (float)RealH,
                          1.0f / (float)RealD);
}

void GFXTextureSetFilterMode(GFXTexture Texture, GFXTextureHint FilterType, GFXTextureHint FilterMode)
{
    CCAssertLog(Texture, "Texture must not be null");
    
    GFXMain->texture->setFilterMode(Texture, FilterType, FilterMode);
}

void GFXTextureSetAddressMode(GFXTexture Texture, GFXTextureHint Coordinate, GFXTextureHint AddressMode)
{
    CCAssertLog(Texture, "Texture must not be null");
    
    GFXMain->texture->setAddressMode(Texture, Coordinate, AddressMode);
}

void GFXTextureInvalidate(GFXTexture Texture)
{
    CCAssertLog(Texture, "Texture must not be null");
    
    if (GFXMain->texture->optional.invalidate) GFXMain->texture->optional.invalidate(Texture);
}
