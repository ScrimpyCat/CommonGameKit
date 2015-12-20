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

#ifndef Blob_Game_GFXBufferFormat_h
#define Blob_Game_GFXBufferFormat_h

typedef enum {
    GFXBufferFormatNormalized = 1,
    
    GFXBufferFormatElementVectorMask = (3 << 1),
    GFXBufferFormatElementMatrixMask = (3 << 3),
    
    GFXBufferFormatElementShift = 1,
    GFXBufferFormatElementMask = (0xf << GFXBufferFormatElementShift),
    GFXBufferFormatElementScalar = (0 << GFXBufferFormatElementShift),
    GFXBufferFormatElementVector2 = (1 << GFXBufferFormatElementShift),
    GFXBufferFormatElementVector3 = (2 << GFXBufferFormatElementShift),
    GFXBufferFormatElementVector4 = (3 << GFXBufferFormatElementShift),
    GFXBufferFormatElementMatrix2x2 = (GFXBufferFormatElementVector2 << 2) | GFXBufferFormatElementVector2,
    GFXBufferFormatElementMatrix2x3 = (GFXBufferFormatElementVector2 << 2) | GFXBufferFormatElementVector3,
    GFXBufferFormatElementMatrix2x4 = (GFXBufferFormatElementVector2 << 2) | GFXBufferFormatElementVector4,
    GFXBufferFormatElementMatrix3x2 = (GFXBufferFormatElementVector3 << 2) | GFXBufferFormatElementVector2,
    GFXBufferFormatElementMatrix3x3 = (GFXBufferFormatElementVector3 << 2) | GFXBufferFormatElementVector3,
    GFXBufferFormatElementMatrix3x4 = (GFXBufferFormatElementVector3 << 2) | GFXBufferFormatElementVector4,
    GFXBufferFormatElementMatrix4x2 = (GFXBufferFormatElementVector4 << 2) | GFXBufferFormatElementVector2,
    GFXBufferFormatElementMatrix4x3 = (GFXBufferFormatElementVector4 << 2) | GFXBufferFormatElementVector3,
    GFXBufferFormatElementMatrix4x4 = (GFXBufferFormatElementVector4 << 2) | GFXBufferFormatElementVector4,
    
    GFXBufferFormatTypeMask = (3 << 5),
    GFXBufferFormatTypeInt = (0 << 5),
    GFXBufferFormatTypeUInt = (1 << 5),
    GFXBufferFormatTypeFloat = (2 << 5),
    
    GFXBufferFormatBitShift = 7,
    GFXBufferFormatBitMask = (3 << GFXBufferFormatBitShift),
    GFXBufferFormatBit8 = (0 << GFXBufferFormatBitShift),
    GFXBufferFormatBit16 = (1 << GFXBufferFormatBitShift),
    GFXBufferFormatBit32 = (2 << GFXBufferFormatBitShift),
    GFXBufferFormatBit64 = (3 << GFXBufferFormatBitShift),
    
    //Int8
    GFXBufferFormatInt8 = GFXBufferFormatTypeInt | GFXBufferFormatElementScalar | GFXBufferFormatBit8,
    GFXBufferFormatInt8x2 = GFXBufferFormatTypeInt | GFXBufferFormatElementVector2 | GFXBufferFormatBit8,
    GFXBufferFormatInt8x3 = GFXBufferFormatTypeInt | GFXBufferFormatElementVector3 | GFXBufferFormatBit8,
    GFXBufferFormatInt8x4 = GFXBufferFormatTypeInt | GFXBufferFormatElementVector4 | GFXBufferFormatBit8,
    GFXBufferFormatInt8Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeInt | GFXBufferFormatElementScalar | GFXBufferFormatBit8,
    GFXBufferFormatInt8x2Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeInt | GFXBufferFormatElementVector2 | GFXBufferFormatBit8,
    GFXBufferFormatInt8x3Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeInt | GFXBufferFormatElementVector3 | GFXBufferFormatBit8,
    GFXBufferFormatInt8x4Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeInt | GFXBufferFormatElementVector4 | GFXBufferFormatBit8,
    
    //UInt8
    GFXBufferFormatUInt8 = GFXBufferFormatTypeUInt | GFXBufferFormatElementScalar | GFXBufferFormatBit8,
    GFXBufferFormatUInt8x2 = GFXBufferFormatTypeUInt | GFXBufferFormatElementVector2 | GFXBufferFormatBit8,
    GFXBufferFormatUInt8x3 = GFXBufferFormatTypeUInt | GFXBufferFormatElementVector3 | GFXBufferFormatBit8,
    GFXBufferFormatUInt8x4 = GFXBufferFormatTypeUInt | GFXBufferFormatElementVector4 | GFXBufferFormatBit8,
    GFXBufferFormatUInt8Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeUInt | GFXBufferFormatElementScalar | GFXBufferFormatBit8,
    GFXBufferFormatUInt8x2Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeUInt | GFXBufferFormatElementVector2 | GFXBufferFormatBit8,
    GFXBufferFormatUInt8x3Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeUInt | GFXBufferFormatElementVector3 | GFXBufferFormatBit8,
    GFXBufferFormatUInt8x4Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeUInt | GFXBufferFormatElementVector4 | GFXBufferFormatBit8,
    
    //Int16
    GFXBufferFormatInt16 = GFXBufferFormatTypeInt | GFXBufferFormatElementScalar | GFXBufferFormatBit16,
    GFXBufferFormatInt16x2 = GFXBufferFormatTypeInt | GFXBufferFormatElementVector2 | GFXBufferFormatBit16,
    GFXBufferFormatInt16x3 = GFXBufferFormatTypeInt | GFXBufferFormatElementVector3 | GFXBufferFormatBit16,
    GFXBufferFormatInt16x4 = GFXBufferFormatTypeInt | GFXBufferFormatElementVector4 | GFXBufferFormatBit16,
    GFXBufferFormatInt16Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeInt | GFXBufferFormatElementScalar | GFXBufferFormatBit16,
    GFXBufferFormatInt16x2Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeInt | GFXBufferFormatElementVector2 | GFXBufferFormatBit16,
    GFXBufferFormatInt16x3Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeInt | GFXBufferFormatElementVector3 | GFXBufferFormatBit16,
    GFXBufferFormatInt16x4Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeInt | GFXBufferFormatElementVector4 | GFXBufferFormatBit16,
    
    //UInt16
    GFXBufferFormatUInt16 = GFXBufferFormatTypeUInt | GFXBufferFormatElementScalar | GFXBufferFormatBit16,
    GFXBufferFormatUInt16x2 = GFXBufferFormatTypeUInt | GFXBufferFormatElementVector2 | GFXBufferFormatBit16,
    GFXBufferFormatUInt16x3 = GFXBufferFormatTypeUInt | GFXBufferFormatElementVector3 | GFXBufferFormatBit16,
    GFXBufferFormatUInt16x4 = GFXBufferFormatTypeUInt | GFXBufferFormatElementVector4 | GFXBufferFormatBit16,
    GFXBufferFormatUInt16Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeUInt | GFXBufferFormatElementScalar | GFXBufferFormatBit16,
    GFXBufferFormatUInt16x2Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeUInt | GFXBufferFormatElementVector2 | GFXBufferFormatBit16,
    GFXBufferFormatUInt16x3Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeUInt | GFXBufferFormatElementVector3 | GFXBufferFormatBit16,
    GFXBufferFormatUInt16x4Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeUInt | GFXBufferFormatElementVector4 | GFXBufferFormatBit16,
    
    //Int32
    GFXBufferFormatInt32 = GFXBufferFormatTypeInt | GFXBufferFormatElementScalar | GFXBufferFormatBit32,
    GFXBufferFormatInt32x2 = GFXBufferFormatTypeInt | GFXBufferFormatElementVector2 | GFXBufferFormatBit32,
    GFXBufferFormatInt32x3 = GFXBufferFormatTypeInt | GFXBufferFormatElementVector3 | GFXBufferFormatBit32,
    GFXBufferFormatInt32x4 = GFXBufferFormatTypeInt | GFXBufferFormatElementVector4 | GFXBufferFormatBit32,
    GFXBufferFormatInt32Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeInt | GFXBufferFormatElementScalar | GFXBufferFormatBit32,
    GFXBufferFormatInt32x2Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeInt | GFXBufferFormatElementVector2 | GFXBufferFormatBit32,
    GFXBufferFormatInt32x3Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeInt | GFXBufferFormatElementVector3 | GFXBufferFormatBit32,
    GFXBufferFormatInt32x4Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeInt | GFXBufferFormatElementVector4 | GFXBufferFormatBit32,
    
    //UInt32
    GFXBufferFormatUInt32 = GFXBufferFormatTypeUInt | GFXBufferFormatElementScalar | GFXBufferFormatBit32,
    GFXBufferFormatUInt32x2 = GFXBufferFormatTypeUInt | GFXBufferFormatElementVector2 | GFXBufferFormatBit32,
    GFXBufferFormatUInt32x3 = GFXBufferFormatTypeUInt | GFXBufferFormatElementVector3 | GFXBufferFormatBit32,
    GFXBufferFormatUInt32x4 = GFXBufferFormatTypeUInt | GFXBufferFormatElementVector4 | GFXBufferFormatBit32,
    GFXBufferFormatUInt32Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeUInt | GFXBufferFormatElementScalar | GFXBufferFormatBit32,
    GFXBufferFormatUInt32x2Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeUInt | GFXBufferFormatElementVector2 | GFXBufferFormatBit32,
    GFXBufferFormatUInt32x3Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeUInt | GFXBufferFormatElementVector3 | GFXBufferFormatBit32,
    GFXBufferFormatUInt32x4Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeUInt | GFXBufferFormatElementVector4 | GFXBufferFormatBit32,
    
    //Int64
    GFXBufferFormatInt64 = GFXBufferFormatTypeInt | GFXBufferFormatElementScalar | GFXBufferFormatBit64,
    GFXBufferFormatInt64x2 = GFXBufferFormatTypeInt | GFXBufferFormatElementVector2 | GFXBufferFormatBit64,
    GFXBufferFormatInt64x3 = GFXBufferFormatTypeInt | GFXBufferFormatElementVector3 | GFXBufferFormatBit64,
    GFXBufferFormatInt64x4 = GFXBufferFormatTypeInt | GFXBufferFormatElementVector4 | GFXBufferFormatBit64,
    GFXBufferFormatInt64Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeInt | GFXBufferFormatElementScalar | GFXBufferFormatBit64,
    GFXBufferFormatInt64x2Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeInt | GFXBufferFormatElementVector2 | GFXBufferFormatBit64,
    GFXBufferFormatInt64x3Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeInt | GFXBufferFormatElementVector3 | GFXBufferFormatBit64,
    GFXBufferFormatInt64x4Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeInt | GFXBufferFormatElementVector4 | GFXBufferFormatBit64,
    
    //UInt64
    GFXBufferFormatUInt64 = GFXBufferFormatTypeUInt | GFXBufferFormatElementScalar | GFXBufferFormatBit64,
    GFXBufferFormatUInt64x2 = GFXBufferFormatTypeUInt | GFXBufferFormatElementVector2 | GFXBufferFormatBit64,
    GFXBufferFormatUInt64x3 = GFXBufferFormatTypeUInt | GFXBufferFormatElementVector3 | GFXBufferFormatBit64,
    GFXBufferFormatUInt64x4 = GFXBufferFormatTypeUInt | GFXBufferFormatElementVector4 | GFXBufferFormatBit64,
    GFXBufferFormatUInt64Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeUInt | GFXBufferFormatElementScalar | GFXBufferFormatBit64,
    GFXBufferFormatUInt64x2Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeUInt | GFXBufferFormatElementVector2 | GFXBufferFormatBit64,
    GFXBufferFormatUInt64x3Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeUInt | GFXBufferFormatElementVector3 | GFXBufferFormatBit64,
    GFXBufferFormatUInt64x4Normalized = GFXBufferFormatNormalized | GFXBufferFormatTypeUInt | GFXBufferFormatElementVector4 | GFXBufferFormatBit64,
    
    //Float16
    GFXBufferFormatFloat16 = GFXBufferFormatTypeFloat | GFXBufferFormatElementScalar | GFXBufferFormatBit16,
    GFXBufferFormatFloat16x2 = GFXBufferFormatTypeFloat | GFXBufferFormatElementVector2 | GFXBufferFormatBit16,
    GFXBufferFormatFloat16x3 = GFXBufferFormatTypeFloat | GFXBufferFormatElementVector3 | GFXBufferFormatBit16,
    GFXBufferFormatFloat16x4 = GFXBufferFormatTypeFloat | GFXBufferFormatElementVector4 | GFXBufferFormatBit16,
    GFXBufferFormatFloat16x2x2 = GFXBufferFormatTypeFloat | GFXBufferFormatElementMatrix2x2 | GFXBufferFormatBit16,
    GFXBufferFormatFloat16x3x2 = GFXBufferFormatTypeFloat | GFXBufferFormatElementMatrix3x2 | GFXBufferFormatBit16,
    GFXBufferFormatFloat16x4x2 = GFXBufferFormatTypeFloat | GFXBufferFormatElementMatrix4x2 | GFXBufferFormatBit16,
    GFXBufferFormatFloat16x2x3 = GFXBufferFormatTypeFloat | GFXBufferFormatElementMatrix2x3 | GFXBufferFormatBit16,
    GFXBufferFormatFloat16x3x3 = GFXBufferFormatTypeFloat | GFXBufferFormatElementMatrix3x3 | GFXBufferFormatBit16,
    GFXBufferFormatFloat16x4x3 = GFXBufferFormatTypeFloat | GFXBufferFormatElementMatrix4x3 | GFXBufferFormatBit16,
    GFXBufferFormatFloat16x2x4 = GFXBufferFormatTypeFloat | GFXBufferFormatElementMatrix2x4 | GFXBufferFormatBit16,
    GFXBufferFormatFloat16x3x4 = GFXBufferFormatTypeFloat | GFXBufferFormatElementMatrix3x4 | GFXBufferFormatBit16,
    GFXBufferFormatFloat16x4x4 = GFXBufferFormatTypeFloat | GFXBufferFormatElementMatrix4x4 | GFXBufferFormatBit16,
    
    //Float32
    GFXBufferFormatFloat32 = GFXBufferFormatTypeFloat | GFXBufferFormatElementScalar | GFXBufferFormatBit32,
    GFXBufferFormatFloat32x2 = GFXBufferFormatTypeFloat | GFXBufferFormatElementVector2 | GFXBufferFormatBit32,
    GFXBufferFormatFloat32x3 = GFXBufferFormatTypeFloat | GFXBufferFormatElementVector3 | GFXBufferFormatBit32,
    GFXBufferFormatFloat32x4 = GFXBufferFormatTypeFloat | GFXBufferFormatElementVector4 | GFXBufferFormatBit32,
    GFXBufferFormatFloat32x2x2 = GFXBufferFormatTypeFloat | GFXBufferFormatElementMatrix2x2 | GFXBufferFormatBit32,
    GFXBufferFormatFloat32x3x2 = GFXBufferFormatTypeFloat | GFXBufferFormatElementMatrix3x2 | GFXBufferFormatBit32,
    GFXBufferFormatFloat32x4x2 = GFXBufferFormatTypeFloat | GFXBufferFormatElementMatrix4x2 | GFXBufferFormatBit32,
    GFXBufferFormatFloat32x2x3 = GFXBufferFormatTypeFloat | GFXBufferFormatElementMatrix2x3 | GFXBufferFormatBit32,
    GFXBufferFormatFloat32x3x3 = GFXBufferFormatTypeFloat | GFXBufferFormatElementMatrix3x3 | GFXBufferFormatBit32,
    GFXBufferFormatFloat32x4x3 = GFXBufferFormatTypeFloat | GFXBufferFormatElementMatrix4x3 | GFXBufferFormatBit32,
    GFXBufferFormatFloat32x2x4 = GFXBufferFormatTypeFloat | GFXBufferFormatElementMatrix2x4 | GFXBufferFormatBit32,
    GFXBufferFormatFloat32x3x4 = GFXBufferFormatTypeFloat | GFXBufferFormatElementMatrix3x4 | GFXBufferFormatBit32,
    GFXBufferFormatFloat32x4x4 = GFXBufferFormatTypeFloat | GFXBufferFormatElementMatrix4x4 | GFXBufferFormatBit32,
    
    //Float64
    GFXBufferFormatFloat64 = GFXBufferFormatTypeFloat | GFXBufferFormatElementScalar | GFXBufferFormatBit64,
    GFXBufferFormatFloat64x2 = GFXBufferFormatTypeFloat | GFXBufferFormatElementVector2 | GFXBufferFormatBit64,
    GFXBufferFormatFloat64x3 = GFXBufferFormatTypeFloat | GFXBufferFormatElementVector3 | GFXBufferFormatBit64,
    GFXBufferFormatFloat64x4 = GFXBufferFormatTypeFloat | GFXBufferFormatElementVector4 | GFXBufferFormatBit64,
    GFXBufferFormatFloat64x2x2 = GFXBufferFormatTypeFloat | GFXBufferFormatElementMatrix2x2 | GFXBufferFormatBit64,
    GFXBufferFormatFloat64x3x2 = GFXBufferFormatTypeFloat | GFXBufferFormatElementMatrix3x2 | GFXBufferFormatBit64,
    GFXBufferFormatFloat64x4x2 = GFXBufferFormatTypeFloat | GFXBufferFormatElementMatrix4x2 | GFXBufferFormatBit64,
    GFXBufferFormatFloat64x2x3 = GFXBufferFormatTypeFloat | GFXBufferFormatElementMatrix2x3 | GFXBufferFormatBit64,
    GFXBufferFormatFloat64x3x3 = GFXBufferFormatTypeFloat | GFXBufferFormatElementMatrix3x3 | GFXBufferFormatBit64,
    GFXBufferFormatFloat64x4x3 = GFXBufferFormatTypeFloat | GFXBufferFormatElementMatrix4x3 | GFXBufferFormatBit64,
    GFXBufferFormatFloat64x2x4 = GFXBufferFormatTypeFloat | GFXBufferFormatElementMatrix2x4 | GFXBufferFormatBit64,
    GFXBufferFormatFloat64x3x4 = GFXBufferFormatTypeFloat | GFXBufferFormatElementMatrix3x4 | GFXBufferFormatBit64,
    GFXBufferFormatFloat64x4x4 = GFXBufferFormatTypeFloat | GFXBufferFormatElementMatrix4x4 | GFXBufferFormatBit64,
} GFXBufferFormat;


static inline _Bool GFXBufferFormatIsScalar(GFXBufferFormat Format);
static inline _Bool GFXBufferFormatIsVector(GFXBufferFormat Format);
static inline _Bool GFXBufferFormatIsMatrix(GFXBufferFormat Format);
static inline _Bool GFXBufferFormatIsInteger(GFXBufferFormat Format);
static inline _Bool GFXBufferFormatIsFloat(GFXBufferFormat Format);
static inline _Bool GFXBufferFormatIsSigned(GFXBufferFormat Format);
static inline _Bool GFXBufferFormatIsUnsigned(GFXBufferFormat Format);
static inline size_t GFXBufferFormatGetBitSize(GFXBufferFormat Format);
static inline size_t GFXBufferFormatGetElementCount(GFXBufferFormat Format);
static inline size_t GFXBufferFormatGetSize(GFXBufferFormat Format);
static inline size_t GFXBufferFormatGetMatrixM(GFXBufferFormat Format); //GFXBufferFormatElementMatrixMxN
static inline size_t GFXBufferFormatGetMatrixN(GFXBufferFormat Format); //GFXBufferFormatElementMatrixMxN


static inline _Bool GFXBufferFormatIsScalar(GFXBufferFormat Format)
{
    return !(Format & GFXBufferFormatElementMask);
}

static inline _Bool GFXBufferFormatIsVector(GFXBufferFormat Format)
{
    return (Format & GFXBufferFormatElementMask) && !(Format & GFXBufferFormatElementMatrixMask);
}

static inline _Bool GFXBufferFormatIsMatrix(GFXBufferFormat Format)
{
    return (Format & GFXBufferFormatElementMask) && (Format & GFXBufferFormatElementMatrixMask);
}

static inline _Bool GFXBufferFormatIsInteger(GFXBufferFormat Format)
{
    return (Format & GFXBufferFormatTypeMask) == GFXBufferFormatTypeInt || (Format & GFXBufferFormatTypeMask) == GFXBufferFormatTypeUInt;
}

static inline _Bool GFXBufferFormatIsFloat(GFXBufferFormat Format)
{
    return (Format & GFXBufferFormatTypeMask) == GFXBufferFormatTypeFloat;
}

static inline _Bool GFXBufferFormatIsSigned(GFXBufferFormat Format)
{
    return GFXBufferFormatIsFloat(Format) || (Format & GFXBufferFormatTypeMask) == GFXBufferFormatTypeInt;
}

static inline _Bool GFXBufferFormatIsUnsigned(GFXBufferFormat Format)
{
    return (Format & GFXBufferFormatTypeMask) == GFXBufferFormatTypeUInt;
}

static inline size_t GFXBufferFormatGetBitSize(GFXBufferFormat Format)
{
    return 8 << ((Format & GFXBufferFormatBitMask) >> GFXBufferFormatBitShift);
}

static inline size_t GFXBufferFormatGetElementCount(GFXBufferFormat Format)
{
    return (((Format & GFXBufferFormatElementVectorMask) >> GFXBufferFormatElementShift) + 1) * (((Format & GFXBufferFormatElementMatrixMask) >> (GFXBufferFormatElementShift + 2)) + 1);
}

static inline size_t GFXBufferFormatGetSize(GFXBufferFormat Format)
{
    return (GFXBufferFormatGetElementCount(Format) * GFXBufferFormatGetBitSize(Format)) / 8;
}

static inline size_t GFXBufferFormatGetMatrixM(GFXBufferFormat Format)
{
    return ((Format & GFXBufferFormatElementMatrixMask) >> (GFXBufferFormatElementShift + 2)) + 1;
}

static inline size_t GFXBufferFormatGetMatrixN(GFXBufferFormat Format)
{
    return ((Format & GFXBufferFormatElementVectorMask) >> GFXBufferFormatElementShift) + 1;
}

#endif
