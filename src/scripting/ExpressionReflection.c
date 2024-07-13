/*
 *  Copyright (c) 2024, Stefan Johnson
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

#include "ExpressionReflection.h"
#include "ExpressionEvaluator.h"

static CCExpression CCExpressionReflectionSerializeHex(const void *Data, size_t Size, size_t PrintStart, size_t PrintInvert)
{
    CCString String = CC_STRING("0x");
    
    static const char Hex[] = "0123456789abcdef";
    char Byte[3] = { 0, 0, 0 };
    
    for (size_t Loop = 0; Loop < Size; Loop++)
    {
        uint8_t Value = ((const uint8_t*)Data)[PrintStart + (PrintInvert * Loop)];
        
        Byte[0] = Hex[Value >> 4];
        Byte[1] = Hex[Value & 0xf];
        
        CCString ByteStr = CCStringCreate(CC_STD_ALLOCATOR, (CCStringHint)CCStringEncodingASCII, Byte);
        CCString NewStr = CCStringCreateByJoiningStrings((CCString[2]){ String, ByteStr }, 2, 0);
        CCStringDestroy(ByteStr);
        CCStringDestroy(String);
        
        String = NewStr;
    }
    
    return CCExpressionCreateString(CC_STD_ALLOCATOR, String, FALSE);
}

static void CCExpressionReflectionDeserializeHex(CCExpression Expression, void *Data, size_t Size, CCReflectEndian Endian)
{
    size_t PrintStart, PrintInvert;
#if CC_HARDWARE_ENDIAN_LITTLE
    switch (Endian)
    {
        case CCReflectEndianNative:
        case CCReflectEndianLittle:
            PrintInvert = 1;
            PrintStart = 0;
            break;
            
        case CCReflectEndianBig:
            PrintInvert = -1;
            PrintStart = Size - 1;
            break;
    }
#elif CC_HARDWARE_ENDIAN_BIG
    switch (Endian)
    {
        case CCReflectEndianNative:
        case CCReflectEndianBig:
            PrintInvert = -1;
            PrintStart = Size - 1;
            break;
            
        case CCReflectEndianLittle:
            PrintInvert = 1;
            PrintStart = 0;
            break;
    }
#else
#error Unknown native endianness
#endif
    
    CCString String = CCExpressionGetString(Expression);
    
    if (CCStringHasPrefix(String, CC_STRING("0x")))
    {
        memset(Data, 0, Size);
        
        CC_STRING_TEMP_BUFFER(Buffer, String)
        {
            const char *Hex = Buffer + 2;
            
            size_t Length = 0;
            for (char Chr; ((Chr = Hex[Length]) >= '0') && ((Chr <= '9') || ((Chr <= 'f') && (Chr >= 'a')) || ((Chr <= 'F') && (Chr >= 'A'))); Length++);
            
            for (size_t Loop = 0; Loop < Length; Loop++)
            {
                uint8_t Nibble = 0;
                
                const char Chr = Hex[Length - Loop - 1];
                
                if ((Chr >= '0') && (Chr <= '9')) Nibble = Chr - '0';
                else if ((Chr >= 'a') && (Chr <= 'f')) Nibble = Chr - 'a' + 10;
                else if ((Chr >= 'A') && (Chr <= 'F')) Nibble = Chr - 'A' + 10;
                
                const size_t Index = PrintStart + (PrintInvert * (Loop / 2));
                ((uint8_t*)Data)[Index] |= Nibble << ((Loop & 1) << 2);
            }
        }
    }
}

static CCExpression CCExpressionReflectionSerializeInteger(const CCReflectInteger *Integer, const void *Data)
{
    const size_t Size = Integer->size;
    
    size_t Invert, Start, PrintStart, PrintInvert;
#if CC_HARDWARE_ENDIAN_LITTLE
    switch (Integer->endian)
    {
        case CCReflectEndianNative:
        case CCReflectEndianLittle:
            Invert = 1;
            Start = 0;
            PrintInvert = -1;
            PrintStart = Size - 1;
            break;
            
        case CCReflectEndianBig:
            Invert = -1;
            Start = Size - 1;
            PrintInvert = 1;
            PrintStart = 0;
            break;
    }
#elif CC_HARDWARE_ENDIAN_BIG
    switch (Integer->endian)
    {
        case CCReflectEndianNative:
        case CCReflectEndianBig:
            Invert = 1;
            Start = 0;
            PrintInvert = 1;
            PrintStart = 0;
            break;
            
        case CCReflectEndianLittle:
            Invert = -1;
            Start = Size - 1;
            PrintInvert = -1;
            PrintStart = Size - 1;
            break;
    }
#else
#error Unknown native endianness
#endif
    
    if ((CCReflectValidateMinimum(Integer, Data, &CC_REFLECT_VALIDATE_MINIMUM(Integer, CC_REFLECT_VALUE(&CC_REFLECT(int32_t), &(int32_t){ INT32_MIN }), CC_REFLECT_RESULT_DEFAULT, CC_REFLECT_RESULT_DEFAULT))) &&
        (CCReflectValidateMaximum(Integer, Data, &CC_REFLECT_VALIDATE_MAXIMUM(Integer, CC_REFLECT_VALUE(&CC_REFLECT(int32_t), &(int32_t){ INT32_MAX }), CC_REFLECT_RESULT_DEFAULT, CC_REFLECT_RESULT_DEFAULT))))
    {
        const uint8_t Sign = Integer->sign ? (((const uint8_t*)Data)[Start + (Invert * (Size - 1))] >> 7) : 0;
        
        int32_t Value = Sign ? -1 : 0;
        
        for (size_t Loop = 0, Count = CCMin(Size, sizeof(int32_t)); Loop < Count; Loop++)
        {
            ((uint8_t*)&Value)[Loop] = ((const uint8_t*)Data)[Start + (Invert * Loop)];
        }
        
        return CCExpressionCreateInteger(CC_STD_ALLOCATOR, Value);
    }
    
    return CCExpressionReflectionSerializeHex(Data, Size, PrintStart, PrintInvert);
}

static void CCExpressionReflectionCastInteger(intmax_t Value, const CCReflectInteger *Integer, void *Data)
{
    const size_t Size = Integer->size;
    
    size_t Invert, Start;
#if CC_HARDWARE_ENDIAN_LITTLE
    switch (Integer->endian)
    {
        case CCReflectEndianNative:
        case CCReflectEndianLittle:
            Invert = 1;
            Start = 0;
            break;
            
        case CCReflectEndianBig:
            Invert = -1;
            Start = Size - 1;
            break;
    }
#elif CC_HARDWARE_ENDIAN_BIG
    switch (Integer->endian)
    {
        case CCReflectEndianNative:
        case CCReflectEndianBig:
            Invert = 1;
            Start = 0;
            break;
            
        case CCReflectEndianLittle:
            Invert = -1;
            Start = Size - 1;
            break;
    }
#else
#error Unknown native endianness
#endif
    
    if (Integer->sign)
    {
        if (Value < 0)
        {
            memset(Data, 0, Integer->size);
            
            ((uint8_t*)Data)[Start + (Invert * (Size - 1))] = 0x80;
            
            if (CCReflectValidateMinimum(&CC_REFLECT(intmax_t), &Value, &CC_REFLECT_VALIDATE_MINIMUM(&CC_REFLECT(intmax_t), CC_REFLECT_VALUE(Integer, Data), CC_REFLECT_RESULT_DEFAULT, CC_REFLECT_RESULT_DEFAULT)))
            {
                memset(Data, 0xff, Integer->size);
                
                for (size_t Loop = 0, Count = CCMin(Size, sizeof(intmax_t)); Loop < Count; Loop++)
                {
                    ((uint8_t*)Data)[Start + (Invert * Loop)] = ((uint8_t*)&Value)[Loop];
                }
            }
            
            return;
        }
    }
    
    memset(Data, 0xff, Integer->size);
    
    ((uint8_t*)Data)[Start + (Invert * (Size - 1))] ^= 0x80;
    
    if (CCReflectValidateMaximum(&CC_REFLECT(intmax_t), &Value, &CC_REFLECT_VALIDATE_MAXIMUM(&CC_REFLECT(intmax_t), CC_REFLECT_VALUE(Integer, Data), CC_REFLECT_RESULT_DEFAULT, CC_REFLECT_RESULT_DEFAULT)))
    {
        memset(Data, 0, Integer->size);
        
        for (size_t Loop = 0, Count = CCMin(Size, sizeof(intmax_t)); Loop < Count; Loop++)
        {
            ((uint8_t*)Data)[Start + (Invert * Loop)] = ((uint8_t*)&Value)[Loop];
        }
        
        ((uint8_t*)Data)[Start + (Invert * (Size - 1))] = ((uint8_t*)Data)[Start + (Invert * (Size - 1))] & ~0x80;
    }
}

static void CCExpressionReflectionDeserializeInteger(const CCReflectInteger *Integer, CCExpression Expression, void *Data)
{
    switch (CCExpressionGetType(Expression))
    {
        case CCExpressionValueTypeInteger:
            CCExpressionReflectionCastInteger(CCExpressionGetInteger(Expression), Integer, Data);
            break;
            
        case CCExpressionValueTypeFloat:
        {
            float Value = CCExpressionGetFloat(Expression);
            
            if (isinf(Value))
            {
                if (signbit(Value)) CCExpressionReflectionCastInteger(INTMAX_MIN, Integer, Data);
                else CCExpressionReflectionCastInteger(INTMAX_MAX, Integer, Data);
            }
            
            else if (isnan(Value)) CCExpressionReflectionCastInteger(0, Integer, Data);
            else if (Value <= INTMAX_MIN) CCExpressionReflectionCastInteger(INTMAX_MIN, Integer, Data);
            else if (Value >= INTMAX_MAX) CCExpressionReflectionCastInteger(INTMAX_MAX, Integer, Data);
            else CCExpressionReflectionCastInteger(CCExpressionGetFloat(Expression), Integer, Data);
            
            break;
        }
            
        case CCExpressionValueTypeString:
            CCExpressionReflectionDeserializeHex(Expression, Data, Integer->size, Integer->endian);
            break;
            
        default:
            CC_LOG_ERROR_CUSTOM("Deserializing malformed integer. Expects an integer, float, or hex string.");
            break;
    }
}

static CCExpression CCExpressionReflectionSerializeFloat(const CCReflectFloat *Float, const void *Data)
{
    const size_t Size = Float->size;
    
    size_t Invert, Start, PrintStart, PrintInvert;
#if CC_HARDWARE_ENDIAN_LITTLE
    switch (Float->endian)
    {
        case CCReflectEndianNative:
        case CCReflectEndianLittle:
            Invert = 1;
            Start = 0;
            PrintInvert = -1;
            PrintStart = Size - 1;
            break;
            
        case CCReflectEndianBig:
            Invert = -1;
            Start = Size - 1;
            PrintInvert = 1;
            PrintStart = 0;
            break;
    }
#elif CC_HARDWARE_ENDIAN_BIG
    switch (Float->endian)
    {
        case CCReflectEndianNative:
        case CCReflectEndianBig:
            Invert = 1;
            Start = 0;
            PrintInvert = 1;
            PrintStart = 0;
            break;
            
        case CCReflectEndianLittle:
            Invert = -1;
            Start = Size - 1;
            PrintInvert = -1;
            PrintStart = Size - 1;
            break;
    }
#else
#error Unknown native endianness
#endif
    
    switch (Size)
    {
        case sizeof(float):
        {
            float Value = 0.0f;
            
            for (size_t Loop = 0; Loop < sizeof(float); Loop++)
            {
                ((uint8_t*)&Value)[Loop] = ((const uint8_t*)Data)[Start + (Invert * Loop)];
            }
            
            return CCExpressionCreateFloat(CC_STD_ALLOCATOR, Value);
        }
            
        default:
            break;
    }
    
    return CCExpressionReflectionSerializeHex(Data, Size, PrintStart, PrintInvert);
}

static void CCExpressionReflectionCastFloat(long double Value, const CCReflectFloat *Float, void *Data)
{
    switch (Float->size)
    {
        case sizeof(float):
            *(float*)Data = Value;
            break;
            
        case sizeof(double):
            *(double*)Data = Value;
            break;
            
        case sizeof(long double):
            *(long double*)Data = Value;
            break;
            
        default:
            CCAssertLog(0, "Unable to cast to float of size (%zu)", Float->size);
            break;
    }

#if CC_HARDWARE_ENDIAN_LITTLE
    if (Float->endian == CCReflectEndianBig)
#elif CC_HARDWARE_ENDIAN_BIG
    if (Float->endian == CCReflectEndianLittle)
#endif
    {
        CCSwap(Data, Float->size);
    }
}

static void CCExpressionReflectionDeserializeFloat(const CCReflectFloat *Float, CCExpression Expression, void *Data)
{
    switch (CCExpressionGetType(Expression))
    {
        case CCExpressionValueTypeInteger:
            CCExpressionReflectionCastFloat(CCExpressionGetInteger(Expression), Float, Data);
            break;
            
        case CCExpressionValueTypeFloat:
            CCExpressionReflectionCastFloat(CCExpressionGetFloat(Expression), Float, Data);
            break;
            
        case CCExpressionValueTypeString:
            CCExpressionReflectionDeserializeHex(Expression, Data, Float->size, Float->endian);
            break;
            
        default:
            CC_LOG_ERROR_CUSTOM("Deserializing malformed float. Expects an integer, float, or hex string.");
            break;
    }
}

static CCExpression CCExpressionReflectionSerializeType(CCReflectType Type, CCMemoryZone Zone);

void CCExpressionReflectionSerializeHandler(CCReflectType Type, const void *Data, CCExpressionReflectionSerializeHandlerArgs *Args)
{
    CCExpression Expr = CCExpressionCreateList(CC_STD_ALLOCATOR);
    
    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionReflectionSerializeType(Type, Args->zone) });
    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionReflectionSerialize(Type, Data, Args->zone) });
    
    *Args->result = Expr;
}

static CCExpression CCExpressionReflectionSerializeType(CCReflectType Type, CCMemoryZone Zone)
{
    CCExpression Expr = CCExpressionCreateList(CC_STD_ALLOCATOR);
    
    switch (*(const CCReflectTypeID*)Type)
    {
        case CCReflectTypePointer:
        {
            CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":pointer"), TRUE) });
            
            CCExpression Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("type:"), TRUE) });
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionReflectionSerializeType(((const CCReflectPointer*)Type)->type, Zone) });
            CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
            
            Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("storage:"), TRUE) });
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateInteger(CC_STD_ALLOCATOR, ((const CCReflectPointer*)Type)->storage) }); // TODO: change to atom
            CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
            
            Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("ownership:"), TRUE) });
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateInteger(CC_STD_ALLOCATOR, ((const CCReflectPointer*)Type)->ownership) }); // TODO: change to atom
            CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
            
            if (((const CCReflectPointer*)Type)->storage == CCReflectStorageStatic)
            {
                CCExpression Result = NULL;
                
                CCReflectStaticPointerTypeDescriptorMap(&CC_REFLECT(CCReflectStaticPointer), Type, &(CCExpressionReflectionSerializeHandlerArgs){
                    .result = &Result,
                    .zone = Zone
                }, (CCReflectTypeHandler)CCExpressionReflectionSerializeHandler, Zone, CC_AUTORELEASE_ALLOCATOR(Zone), CCReflectMapIntentSerialize);
                
                CCAssertLog(Result, "Result should be valid expression");
                
                Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
                CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("descriptor:"), TRUE) });
                CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &Result);
                CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
            }
            
            else
            {
                CCAssertLog(((const CCReflectDynamicPointer*)Type)->allocator.allocator == CC_NULL_ALLOCATOR.allocator, "Cannot serialize dynamic pointer types with custom allocators");
                
//                Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
//                CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("allocator:"), TRUE) });
//                CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":null"), TRUE) });
//                CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
                
                if (((const CCReflectDynamicPointer*)Type)->destructor)
                {
                    Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("destructor:"), TRUE) });
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionReflectionSerialize(&CC_REFLECT(CCMemoryDestructorCallback), ((const CCReflectDynamicPointer*)Type)->destructor, Zone) });
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
                }
            }
            
            break;
        }
            
        case CCReflectTypeInteger:
        {
            CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":integer"), TRUE) });
            
            CCExpression Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("sign:"), TRUE) });
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateInteger(CC_STD_ALLOCATOR, ((const CCReflectInteger*)Type)->sign) });
            CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
            
            Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("fixed:"), TRUE) });
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateInteger(CC_STD_ALLOCATOR, ((const CCReflectInteger*)Type)->fixed) });
            CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
            
            Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("endian:"), TRUE) });
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateInteger(CC_STD_ALLOCATOR, ((const CCReflectInteger*)Type)->endian) });
            CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
            
            Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("size:"), TRUE) });
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionReflectionSerializeInteger(&CC_REFLECT(size_t), &((const CCReflectInteger*)Type)->size) });
            CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
            
            break;
        }
            
        case CCReflectTypeFloat:
        {
            CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":float"), TRUE) });
            
            CCExpression Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("endian:"), TRUE) });
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateInteger(CC_STD_ALLOCATOR, ((const CCReflectFloat*)Type)->endian) });
            CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
            
            Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("size:"), TRUE) });
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionReflectionSerializeInteger(&CC_REFLECT(size_t), &((const CCReflectFloat*)Type)->size) });
            CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
            
            break;
        }
            
        case CCReflectTypeStruct:
        {
            CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":struct"), TRUE) });
            
            CCExpression Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("size:"), TRUE) });
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionReflectionSerializeInteger(&CC_REFLECT(size_t), &((const CCReflectStruct*)Type)->size) });
            CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
            
            Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("fields:"), TRUE) });
            CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
            
            CCExpression Fields = CCExpressionCreateList(CC_STD_ALLOCATOR);
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &Fields);
            
            for (size_t Loop = 0, Count = ((const CCReflectStruct*)Type)->count; Loop < Count; Loop++)
            {
                CCExpression StructField = CCExpressionCreateList(CC_STD_ALLOCATOR);
                CCOrderedCollectionAppendElement(CCExpressionGetList(Fields), &StructField);
                
                Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
                CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("name:"), TRUE) });
                CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateString(CC_STD_ALLOCATOR, ((const CCReflectStruct*)Type)->fields[Loop].name, TRUE) });
                CCOrderedCollectionAppendElement(CCExpressionGetList(StructField), &Field);
                
                Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
                CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("offset:"), TRUE) });
                CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionReflectionSerializeInteger(&CC_REFLECT(size_t), &((const CCReflectStruct*)Type)->fields[Loop].offset) });
                CCOrderedCollectionAppendElement(CCExpressionGetList(StructField), &Field);
                
                Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
                CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("type:"), TRUE) });
                CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionReflectionSerializeType(((const CCReflectStruct*)Type)->fields[Loop].type, Zone) });
                CCOrderedCollectionAppendElement(CCExpressionGetList(StructField), &Field);
            }
            
            break;
        }
            
        case CCReflectTypeOpaque:
        {
            CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":opaque"), TRUE) });
            
            CCExpression Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("type-size:"), TRUE) });
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionReflectionSerializeInteger(&CC_REFLECT(size_t), &((const CCReflectOpaque*)Type)->typeSize) });
            CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
            
            CCExpression Result = NULL;
            
            CCReflectOpaqueTypeDescriptorMap(&CC_REFLECT(CCReflectType), &Type, &(CCExpressionReflectionSerializeHandlerArgs){
                .result = &Result,
                .zone = Zone
            }, (CCReflectTypeHandler)CCExpressionReflectionSerializeHandler, Zone, CC_AUTORELEASE_ALLOCATOR(Zone), CCReflectMapIntentSerialize);
            
            CCAssertLog(Result, "Result should be valid expression");
            
            Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("descriptor:"), TRUE) });
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &Result);
            CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
            
            break;
        }
            
        case CCReflectTypeArray:
        {
            CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":array"), TRUE) });
            
            CCExpression Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("type:"), TRUE) });
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionReflectionSerializeType(((const CCReflectArray*)Type)->type, Zone) });
            CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
            
            Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("count:"), TRUE) });
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionReflectionSerializeInteger(&CC_REFLECT(size_t), &((const CCReflectArray*)Type)->count) });
            CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
            
            break;
        }
            
        case CCReflectTypeValidator:
            CCAssertLog(0, "Cannot serialize validator type");
            break;
            
        case CCReflectTypeEnumerable:
        {
            CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":enumerable"), TRUE) });
            
            CCExpression Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("type:"), TRUE) });
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionReflectionSerializeType(((const CCReflectEnumerable*)Type)->type, Zone) });
            CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
            
            Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("count:"), TRUE) });
            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionReflectionSerializeInteger(&CC_REFLECT(size_t), &((const CCReflectEnumerable*)Type)->count) });
            CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
            
            break;
        }
    }
    
    return Expr;
}

void CCExpressionReflectionDeserializeHandler(CCReflectType Type, const void *Data, CCExpressionReflectionDeserializeHandlerArgs *Args)
{
    CCReflectCopy(Type, Args->dest, Data, Args->zone, Args->allocator, CCReflectAssignmentShare, TRUE);
}

static CCReflectType CCExpressionReflectionDeserializeType(CCExpression Expression, CCMemoryZone Zone)
{
    if (CCExpressionGetType(Expression) == CCExpressionValueTypeList)
    {
        CCEnumerator Enumerator;
        CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
        
        CCExpression *TypeExpr = CCCollectionEnumeratorGetCurrent(&Enumerator);
        if ((TypeExpr) && (CCExpressionGetType(*TypeExpr) == CCExpressionValueTypeAtom))
        {
            CCString Type = CCExpressionGetAtom(*TypeExpr);
            
            if (CCStringEqual(Type, CC_STRING(":pointer")))
            {
                CCReflectPointer *Pointer = CCMemoryZoneAllocate(Zone, sizeof(union { CCReflectStaticPointer staticPointer; CCReflectDynamicPointer dynamicPointer; }));
                
                memset(Pointer, 0, sizeof(union { CCReflectStaticPointer staticPointer; CCReflectDynamicPointer dynamicPointer; }));
                Pointer->id = CCReflectTypePointer;
                
                for (CCExpression *Element = CCCollectionEnumeratorNext(&Enumerator); Element; Element = CCCollectionEnumeratorNext(&Enumerator))
                {
                    CCExpression Arg = *Element;
                    
                    if (CCExpressionGetType(Arg) == CCExpressionValueTypeList)
                    {
                        size_t Count = CCCollectionGetCount(CCExpressionGetList(Arg));
                        if (Count == 2)
                        {
                            CCExpression NameExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 0);
                            CCExpression ArgExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 1);
                            
                            if (CCExpressionGetType(NameExpr) == CCExpressionValueTypeAtom)
                            {
                                CCString Name = CCExpressionGetString(NameExpr);
                                
                                if (CCStringEqual(Name, CC_STRING("type:")))
                                {
                                    Pointer->type = CCExpressionReflectionDeserializeType(ArgExpr, Zone);
                                }
                                
                                else if (CCStringEqual(Name, CC_STRING("storage:")))
                                {
                                    CCExpressionReflectionDeserializeInteger(&CC_REFLECT(CCReflectStorage), ArgExpr, &Pointer->storage);
                                    
                                    if (Pointer->storage == CCReflectStorageDynamic) ((CCReflectDynamicPointer*)Pointer)->allocator = CC_NULL_ALLOCATOR;
                                }
                                
                                else if (CCStringEqual(Name, CC_STRING("ownership:")))
                                {
                                    CCExpressionReflectionDeserializeInteger(&CC_REFLECT(CCReflectOwnership), ArgExpr, &Pointer->ownership);
                                }
                                
                                else if (Pointer->storage == CCReflectStorageStatic)
                                {
                                    if (CCStringEqual(Name, CC_STRING("descriptor:")))
                                    {
                                        CCExpression DescriptorTypeExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(ArgExpr), 0);
                                        CCExpression DescriptorDataExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(ArgExpr), 1);
                                        
                                        CCReflectType MappedType = CCExpressionReflectionDeserializeType(DescriptorTypeExpr, Zone);
                                        void *MappedData = CCMemoryZoneAllocate(Zone, CCReflectTypeSize(MappedType));
                                        CCExpressionReflectionDeserialize(DescriptorDataExpr, MappedType, MappedData, Zone, CC_AUTORELEASE_ALLOCATOR(Zone));
                                        
                                        CCReflectStaticPointerTypeDescriptorUnmap(&CC_REFLECT(CCReflectStaticPointer), MappedType, MappedData, &(CCExpressionReflectionDeserializeHandlerArgs){
                                            .dest = Pointer,
                                            .allocator = CC_AUTORELEASE_ALLOCATOR(Zone),
                                            .zone = Zone
                                        }, (CCReflectTypeHandler)CCExpressionReflectionDeserializeHandler, Zone, CC_AUTORELEASE_ALLOCATOR(Zone));
                                    }
                                    
                                    else CC_LOG_ERROR_CUSTOM("Deserializing unknown option (%S) for type :pointer. Expects 'size:', 'endian:', 'sign:', or 'fixed:'; and 'descriptor:' for static pointers.", Name);
                                }
                                
                                else if (Pointer->storage == CCReflectStorageDynamic)
                                {
                                    if (CCStringEqual(Name, CC_STRING("allocator:")))
                                    {
                                        
                                    }
                                    
                                    else if (CCStringEqual(Name, CC_STRING("destructor:")))
                                    {
                                        CCExpressionReflectionDeserialize(ArgExpr, &CC_REFLECT(CCMemoryDestructorCallback), &((CCReflectDynamicPointer*)Pointer)->destructor, Zone, CC_AUTORELEASE_ALLOCATOR(Zone));
                                    }
                                    
                                    else CC_LOG_ERROR_CUSTOM("Deserializing unknown option (%S) for type :pointer. Expects 'size:', 'endian:', 'sign:', or 'fixed:'; and 'allocator:' or 'destructor:' for dynamic pointers.", Name);
                                }
                                
                                else CC_LOG_ERROR_CUSTOM("Deserializing unknown option (%S) for type :pointer. Expects 'size:', 'endian:', 'sign:', or 'fixed:'.", Name);
                            }
                            
                            else CC_LOG_ERROR_CUSTOM("Deserializing unknown value for type :pointer. Expects option arguments.");
                        }
                        
                        else CC_LOG_ERROR_CUSTOM("Deserializing unknown value for type :pointer. Expects option arguments.");
                    }
                    
                    else CC_LOG_ERROR_CUSTOM("Deserializing unknown value for type :pointer. Expects option arguments.");
                }
                
                return Pointer;
            }
            
            else if (CCStringEqual(Type, CC_STRING(":integer")))
            {
                CCReflectInteger *Integer = CCMemoryZoneAllocate(Zone, sizeof(CCReflectInteger));
                
                memset(Integer, 0, sizeof(CCReflectInteger));
                Integer->id = CCReflectTypeInteger;
                
                for (CCExpression *Element = CCCollectionEnumeratorNext(&Enumerator); Element; Element = CCCollectionEnumeratorNext(&Enumerator))
                {
                    CCExpression Arg = *Element;
                    
                    if (CCExpressionGetType(Arg) == CCExpressionValueTypeList)
                    {
                        size_t Count = CCCollectionGetCount(CCExpressionGetList(Arg));
                        if (Count == 2)
                        {
                            CCExpression NameExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 0);
                            CCExpression ArgExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 1);
                            
                            if (CCExpressionGetType(NameExpr) == CCExpressionValueTypeAtom)
                            {
                                CCString Name = CCExpressionGetString(NameExpr);
                                
                                if (CCStringEqual(Name, CC_STRING("size:")))
                                {
                                    CCExpressionReflectionDeserializeInteger(&CC_REFLECT(size_t), ArgExpr, &Integer->size);
                                }
                                
                                else if (CCStringEqual(Name, CC_STRING("endian:")))
                                {
                                    CCExpressionReflectionDeserializeInteger(&CC_REFLECT(CCReflectEndian), ArgExpr, &Integer->endian);
                                }
                                
                                else  if (CCStringEqual(Name, CC_STRING("sign:")))
                                {
                                    CCExpressionReflectionDeserializeInteger(&CC_REFLECT(_Bool), ArgExpr, &Integer->sign);
                                }
                                
                                else if (CCStringEqual(Name, CC_STRING("fixed:")))
                                {
                                    CCExpressionReflectionDeserializeInteger(&CC_REFLECT(_Bool), ArgExpr, &Integer->fixed);
                                }
                                
                                else CC_LOG_ERROR_CUSTOM("Deserializing unknown option (%S) for type :integer. Expects 'size:', 'endian:', 'sign:', or 'fixed:'.", Name);
                            }
                            
                            else CC_LOG_ERROR_CUSTOM("Deserializing unknown value for type :integer. Expects option arguments.");
                        }
                        
                        else CC_LOG_ERROR_CUSTOM("Deserializing unknown value for type :integer. Expects option arguments.");
                    }
                    
                    else CC_LOG_ERROR_CUSTOM("Deserializing unknown value for type :integer. Expects option arguments.");
                }
                
                return Integer;
            }
            
            else if (CCStringEqual(Type, CC_STRING(":float")))
            {
                CCReflectFloat *Float = CCMemoryZoneAllocate(Zone, sizeof(CCReflectFloat));
                
                memset(Float, 0, sizeof(CCReflectFloat));
                Float->id = CCReflectTypeFloat;
                
                for (CCExpression *Element = CCCollectionEnumeratorNext(&Enumerator); Element; Element = CCCollectionEnumeratorNext(&Enumerator))
                {
                    CCExpression Arg = *Element;
                    
                    if (CCExpressionGetType(Arg) == CCExpressionValueTypeList)
                    {
                        size_t Count = CCCollectionGetCount(CCExpressionGetList(Arg));
                        if (Count == 2)
                        {
                            CCExpression NameExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 0);
                            CCExpression ArgExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 1);
                            
                            if (CCExpressionGetType(NameExpr) == CCExpressionValueTypeAtom)
                            {
                                CCString Name = CCExpressionGetString(NameExpr);
                                
                                if (CCStringEqual(Name, CC_STRING("size:")))
                                {
                                    CCExpressionReflectionDeserializeInteger(&CC_REFLECT(size_t), ArgExpr, &Float->size);
                                }
                                
                                else if (CCStringEqual(Name, CC_STRING("endian:")))
                                {
                                    CCExpressionReflectionDeserializeInteger(&CC_REFLECT(CCReflectEndian), ArgExpr, &Float->endian);
                                }
                                
                                else CC_LOG_ERROR_CUSTOM("Deserializing unknown option (%S) for type :float. Expects 'size:' or 'endian:'.", Name);
                            }
                            
                            else CC_LOG_ERROR_CUSTOM("Deserializing unknown value for type :float. Expects option arguments.");
                        }
                        
                        else CC_LOG_ERROR_CUSTOM("Deserializing unknown value for type :float. Expects option arguments.");
                    }
                    
                    else CC_LOG_ERROR_CUSTOM("Deserializing unknown value for type :float. Expects option arguments.");
                }
                
                return Float;
            }
            
            else if (CCStringEqual(Type, CC_STRING(":struct")))
            {
                size_t Size = 0;
                CCReflectStruct *Struct = CCMemoryZoneAllocate(Zone, sizeof(CCReflectStruct));
                
                memset(Struct, 0, sizeof(CCReflectStruct));
                
                for (CCExpression *Element = CCCollectionEnumeratorNext(&Enumerator); Element; Element = CCCollectionEnumeratorNext(&Enumerator))
                {
                    CCExpression Arg = *Element;
                    
                    if (CCExpressionGetType(Arg) == CCExpressionValueTypeList)
                    {
                        size_t Count = CCCollectionGetCount(CCExpressionGetList(Arg));
                        if (Count == 2)
                        {
                            CCExpression NameExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 0);
                            CCExpression ArgExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 1);
                            
                            if (CCExpressionGetType(NameExpr) == CCExpressionValueTypeAtom)
                            {
                                CCString Name = CCExpressionGetString(NameExpr);
                                
                                if (CCStringEqual(Name, CC_STRING("size:")))
                                {
                                    CCExpressionReflectionDeserializeInteger(&CC_REFLECT(size_t), ArgExpr, &Size);
                                    
                                    Struct->size = Size;
                                }
                                
                                else if (CCStringEqual(Name, CC_STRING("fields:")))
                                {
                                    if (CCExpressionGetType(ArgExpr) == CCExpressionValueTypeList)
                                    {
                                        CCCollection Fields = CCExpressionGetList(ArgExpr);
                                        const size_t Count = CCCollectionGetCount(Fields);
                                        
                                        Struct = CCMemoryZoneAllocate(Zone, sizeof(CCReflectStruct) + (sizeof(*Struct->fields) * Count));
                                        
                                        memset(Struct, 0, sizeof(CCReflectStruct) + (sizeof(*Struct->fields) * Count));
                                        Struct->id = CCReflectTypeStruct;
                                        Struct->size = Size;
                                        Struct->count = Count;
                                        
                                        size_t Index = 0;
                                        CC_COLLECTION_FOREACH(CCExpression, FieldExpr, Fields)
                                        {
                                            if (CCExpressionGetType(FieldExpr) == CCExpressionValueTypeList)
                                            {
                                                CC_COLLECTION_FOREACH(CCExpression, FieldArg, CCExpressionGetList(FieldExpr))
                                                {
                                                    if (CCExpressionGetType(FieldArg) == CCExpressionValueTypeList)
                                                    {
                                                        size_t Count = CCCollectionGetCount(CCExpressionGetList(FieldArg));
                                                        if (Count == 2)
                                                        {
                                                            CCExpression FieldArgNameExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(FieldArg), 0);
                                                            CCExpression FieldArgDataExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(FieldArg), 1);
                                                            
                                                            CCString FieldArgName = CCExpressionGetString(FieldArgNameExpr);
                                                            
                                                            if (CCStringEqual(FieldArgName, CC_STRING("name:")))
                                                            {
                                                                Struct->fields[Index].name = CCExpressionGetString(FieldArgDataExpr);
                                                            }
                                                            
                                                            else if (CCStringEqual(FieldArgName, CC_STRING("offset:")))
                                                            {
                                                                CCExpressionReflectionDeserializeInteger(&CC_REFLECT(size_t), FieldArgDataExpr, &Struct->fields[Index].offset);
                                                            }
                                                            
                                                            else if (CCStringEqual(FieldArgName, CC_STRING("type:")))
                                                            {
                                                                Struct->fields[Index].type = CCExpressionReflectionDeserializeType(FieldArgDataExpr, Zone);
                                                            }
                                                            
                                                            else CC_LOG_ERROR_CUSTOM("Deserializing unknown field option (%S) for type :struct. Expects 'name:', 'offset:', or 'type:'.", Name);
                                                        }
                                                        
                                                        else CC_LOG_ERROR_CUSTOM("Deserializing unknown field value for type :struct. Expects option arguments.");
                                                    }
                                                    
                                                    else CC_LOG_ERROR_CUSTOM("Deserializing unknown field value for type :struct. Expects option arguments.");
                                                }
                                            }
                                            
                                            else CC_LOG_ERROR_CUSTOM("Deserializing unknown field value for type :struct. Expects list of option arguments.");
                                            
                                            Index++;
                                        }
                                    }
                                    
                                    else CC_LOG_ERROR_CUSTOM("Deserializing unknown field value for type :struct. Expects list of option arguments.");
                                }
                                
                                else CC_LOG_ERROR_CUSTOM("Deserializing unknown option (%S) for type :struct. Expects 'size:' or 'fields:'.", Name);
                            }
                            
                            else CC_LOG_ERROR_CUSTOM("Deserializing unknown value for type :struct. Expects option arguments.");
                        }
                        
                        else CC_LOG_ERROR_CUSTOM("Deserializing unknown value for type :struct. Expects option arguments.");
                    }
                    
                    else CC_LOG_ERROR_CUSTOM("Deserializing unknown value for type :struct. Expects option arguments.");
                }
                
                return Struct;
            }
            
            else if (CCStringEqual(Type, CC_STRING(":opaque")))
            {
                size_t TypeSize = 0;
                
                CCReflectOpaque *Opaque = CCMemoryZoneAllocate(Zone, sizeof(CCReflectOpaque));
                
                memset(Opaque, 0, sizeof(CCReflectOpaque));
                Opaque->id = CCReflectTypeOpaque;
                
                for (CCExpression *Element = CCCollectionEnumeratorNext(&Enumerator); Element; Element = CCCollectionEnumeratorNext(&Enumerator))
                {
                    CCExpression Arg = *Element;
                    
                    if (CCExpressionGetType(Arg) == CCExpressionValueTypeList)
                    {
                        size_t Count = CCCollectionGetCount(CCExpressionGetList(Arg));
                        if (Count == 2)
                        {
                            CCExpression NameExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 0);
                            CCExpression ArgExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 1);
                            
                            if (CCExpressionGetType(NameExpr) == CCExpressionValueTypeAtom)
                            {
                                CCString Name = CCExpressionGetString(NameExpr);
                                
                                if (CCStringEqual(Name, CC_STRING("type-size:")))
                                {
                                    CCExpressionReflectionDeserializeInteger(&CC_REFLECT(size_t), ArgExpr, &TypeSize);
                                    
                                    Opaque = CCMemoryZoneAllocate(Zone, TypeSize);
                                    
                                    memset(Opaque, 0, TypeSize);
                                    Opaque->id = CCReflectTypeOpaque;
                                    Opaque->typeSize = TypeSize;
                                }
                                
                                else if (CCStringEqual(Name, CC_STRING("descriptor:")))
                                {
                                    if ((CCExpressionGetType(ArgExpr) == CCExpressionValueTypeList) && (CCCollectionGetCount(CCExpressionGetList(ArgExpr))))
                                    {
                                        CCExpression DescriptorTypeExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(ArgExpr), 0);
                                        CCExpression DescriptorDataExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(ArgExpr), 1);
                                        
                                        CCReflectType MappedType = CCExpressionReflectionDeserializeType(DescriptorTypeExpr, Zone);
                                        void *MappedData = CCMemoryZoneAllocate(Zone, CCReflectTypeSize(MappedType));
                                        CCExpressionReflectionDeserialize(DescriptorDataExpr, MappedType, MappedData, Zone, CC_AUTORELEASE_ALLOCATOR(Zone));
                                        
                                        CCReflectOpaqueTypeDescriptorUnmap(&CC_REFLECT(CCReflectType), MappedType, MappedData, &(CCExpressionReflectionDeserializeHandlerArgs){
                                            .dest = Opaque,
                                            .allocator = CC_AUTORELEASE_ALLOCATOR(Zone),
                                            .zone = Zone
                                        }, (CCReflectTypeHandler)CCExpressionReflectionDeserializeHandler, Zone, CC_AUTORELEASE_ALLOCATOR(Zone));
                                    }
                                    
                                    else CC_LOG_ERROR_CUSTOM("Deserializing invalid argument for option (descriptor) for type :opaque");
                                }
                                
                                else CC_LOG_ERROR_CUSTOM("Deserializing unknown option (%S) for type :opaque. Expects 'type-size:' or 'descriptor:'.", Name);
                            }
                            
                            else CC_LOG_ERROR_CUSTOM("Deserializing unknown value for type :opaque. Expects option arguments.");
                        }
                        
                        else CC_LOG_ERROR_CUSTOM("Deserializing unknown value for type :opaque. Expects option arguments.");
                    }
                    
                    else CC_LOG_ERROR_CUSTOM("Deserializing unknown value for type :opaque. Expects option arguments.");
                }
                
                return Opaque;
            }
            
            else if (CCStringEqual(Type, CC_STRING(":array")))
            {
                CCReflectArray *Array = CCMemoryZoneAllocate(Zone, sizeof(CCReflectArray));
                
                memset(Array, 0, sizeof(CCReflectArray));
                Array->id = CCReflectTypeArray;
                
                for (CCExpression *Element = CCCollectionEnumeratorNext(&Enumerator); Element; Element = CCCollectionEnumeratorNext(&Enumerator))
                {
                    CCExpression Arg = *Element;
                    
                    if (CCExpressionGetType(Arg) == CCExpressionValueTypeList)
                    {
                        size_t Count = CCCollectionGetCount(CCExpressionGetList(Arg));
                        if (Count == 2)
                        {
                            CCExpression NameExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 0);
                            CCExpression ArgExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 1);
                            
                            if (CCExpressionGetType(NameExpr) == CCExpressionValueTypeAtom)
                            {
                                CCString Name = CCExpressionGetString(NameExpr);
                                
                                if (CCStringEqual(Name, CC_STRING("type:")))
                                {
                                    Array->type = CCExpressionReflectionDeserializeType(ArgExpr, Zone);
                                }
                                
                                else if (CCStringEqual(Name, CC_STRING("count:")))
                                {
                                    CCExpressionReflectionDeserializeInteger(&CC_REFLECT(size_t), ArgExpr, &Array->count);
                                }
                                
                                else CC_LOG_ERROR_CUSTOM("Deserializing unknown option (%S) for type :array. Expects 'type:' or 'count:'.", Name);
                            }
                            
                            else CC_LOG_ERROR_CUSTOM("Deserializing unknown value for type :array. Expects option arguments.");
                        }
                        
                        else CC_LOG_ERROR_CUSTOM("Deserializing unknown value for type :array. Expects option arguments.");
                    }
                    
                    else CC_LOG_ERROR_CUSTOM("Deserializing unknown value for type :array. Expects option arguments.");
                }
                
                return Array;
            }
            
            else if (CCStringEqual(Type, CC_STRING(":validator")))
            {
                CC_LOG_ERROR_CUSTOM("Deserializing unsupported type :validator");
            }
            
            else if (CCStringEqual(Type, CC_STRING(":enumerable")))
            {
                CCReflectEnumerable *Enumerable = CCMemoryZoneAllocate(Zone, sizeof(CCReflectEnumerable));
                
                memset(Enumerable, 0, sizeof(CCReflectEnumerable));
                Enumerable->id = CCReflectTypeEnumerable;
                
                for (CCExpression *Element = CCCollectionEnumeratorNext(&Enumerator); Element; Element = CCCollectionEnumeratorNext(&Enumerator))
                {
                    CCExpression Arg = *Element;
                    
                    if (CCExpressionGetType(Arg) == CCExpressionValueTypeList)
                    {
                        size_t Count = CCCollectionGetCount(CCExpressionGetList(Arg));
                        if (Count == 2)
                        {
                            CCExpression NameExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 0);
                            CCExpression ArgExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 1);
                            
                            if (CCExpressionGetType(NameExpr) == CCExpressionValueTypeAtom)
                            {
                                CCString Name = CCExpressionGetString(NameExpr);
                                
                                if (CCStringEqual(Name, CC_STRING("type:")))
                                {
                                    Enumerable->type = CCExpressionReflectionDeserializeType(ArgExpr, Zone);
                                }
                                
                                else if (CCStringEqual(Name, CC_STRING("count:")))
                                {
                                    CCExpressionReflectionDeserializeInteger(&CC_REFLECT(size_t), ArgExpr, &Enumerable->count);
                                }
                                
                                else CC_LOG_ERROR_CUSTOM("Deserializing unknown option (%S) for type :enumerable. Expects 'type:' or 'count:'.", Name);
                            }
                            
                            else CC_LOG_ERROR_CUSTOM("Deserializing unknown value for type :enumerable. Expects option arguments.");
                        }
                        
                        else CC_LOG_ERROR_CUSTOM("Deserializing unknown value for type :enumerable. Expects option arguments.");
                    }
                    
                    else CC_LOG_ERROR_CUSTOM("Deserializing unknown value for type :enumerable. Expects option arguments.");
                }
                
                return Enumerable;
            }
            
            else CC_LOG_ERROR_CUSTOM("Deserializing unknown type (%S)", Type);
        }
        
        else CC_LOG_ERROR_CUSTOM("Deserializing malformed type");
    }
    
    else CC_LOG_ERROR_CUSTOM("Deserializing malformed type");
    
    return NULL;
}

CCExpression CCExpressionReflectionSerialize(CCReflectType Type, const void *Data, CCMemoryZone Zone)
{
    CCAssertLog(Type, "Type must not be null");
    CCAssertLog(Data, "Data must not be null");
    CCAssertLog(Zone, "Zone must not be null");
    
    switch (*(const CCReflectTypeID*)Type)
    {
        case CCReflectTypePointer:
        {
            CCExpression Expr = CCExpressionCreateList(CC_STD_ALLOCATOR);
            
            if (*(void**)Data)
            {
                if (((const CCReflectPointer*)Type)->storage == CCReflectStorageDynamic)
                {
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionReflectionSerialize(((const CCReflectPointer*)Type)->type, *(void**)Data, Zone) });
                }
                
                else
                {
                    CCExpression Result = NULL;
                    
                    ((const CCReflectStaticPointer*)Type)->map(Type, Data, &(CCExpressionReflectionSerializeHandlerArgs){
                        .result = &Result,
                        .zone = Zone
                    }, (CCReflectTypeHandler)CCExpressionReflectionSerializeHandler, Zone, CC_AUTORELEASE_ALLOCATOR(Zone), CCReflectMapIntentSerialize);
                    
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Result);
                }
            }
            
            return Expr;
        }
            
        case CCReflectTypeInteger:
            return CCExpressionReflectionSerializeInteger(Type, Data);
            
        case CCReflectTypeFloat:
            return CCExpressionReflectionSerializeFloat(Type, Data);
            
        case CCReflectTypeStruct:
        {
            CCExpression Expr = CCExpressionCreateList(CC_STD_ALLOCATOR);
            
            for (size_t Loop = 0, Count = ((const CCReflectStruct*)Type)->count; Loop < Count; Loop++)
            {
                CCExpression Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
                
                CCString Name = ((const CCReflectStruct*)Type)->fields[Loop].name;
                if (!Name) Name = CC_STRING("");
                
                Name = CCStringCreateByJoiningStrings((CCString[2]){ Name, CC_STRING(":") }, 2, 0);
                
                CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, Name, FALSE) });
                CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionReflectionSerialize(((const CCReflectStruct*)Type)->fields[Loop].type, Data + ((const CCReflectStruct*)Type)->fields[Loop].offset, Zone) });
                
                CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
            }
            
            return Expr;
        }
            
        case CCReflectTypeOpaque:
        {
            if (Type == &CC_REFLECT(CCString))
            {
                return *(CCString*)Data ? CCExpressionCreateString(CC_STD_ALLOCATOR, *(CCString*)Data, TRUE) : CCExpressionCreateInteger(CC_STD_ALLOCATOR, 0);
            }
            
            else if ((((const CCReflectOpaque*)Type)->map == CCReflectCCArrayMapper) && (((const CCReflectOpaque*)Type)->unmap == CCReflectCCArrayUnmapper))
            {
                CCArray Array = *(CCArray*)Data;
                
                if (!Array) return CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":null"), TRUE);
                
                CCExpression List = CCExpressionCreateList(CC_STD_ALLOCATOR);
                
                for (size_t Loop = 0, Count = CCArrayGetCount(Array); Loop < Count; Loop++)
                {
                    CCOrderedCollectionAppendElement(CCExpressionGetList(List), &(CCExpression){ CCExpressionReflectionSerialize(((const CCReflectCCArray*)Type)->elementType, CCArrayGetElementAtIndex(Array, Loop), Zone) });
                }
                
                return List;
            }
            
            else if ((((const CCReflectOpaque*)Type)->map == CCReflectCCLinkedListMapper) && (((const CCReflectOpaque*)Type)->unmap == CCReflectCCLinkedListUnmapper))
            {
                CCLinkedList LinkedList = *(CCLinkedList*)Data;
                
                if (!LinkedList) return CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":null"), TRUE);
                
                CCExpression List = CCExpressionCreateList(CC_STD_ALLOCATOR);
                
                for (CCLinkedListNode *Node = LinkedList; Node; Node = CCLinkedListEnumerateNext(Node))
                {
                    CCOrderedCollectionAppendElement(CCExpressionGetList(List), &(CCExpression){ CCExpressionReflectionSerialize(((const CCReflectCCLinkedList*)Type)->elementType, CCLinkedListGetNodeData(Node), Zone) });
                }
                
                return List;
            }
            
            else if ((((const CCReflectOpaque*)Type)->map == CCReflectCCListMapper) && (((const CCReflectOpaque*)Type)->unmap == CCReflectCCListUnmapper))
            {
                CCList List = *(CCList*)Data;
                
                if (!List) return CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":null"), TRUE);
                
                CCExpression ListExpr = CCExpressionCreateList(CC_STD_ALLOCATOR);
                
                for (size_t Loop = 0, Count = CCListGetCount(List); Loop < Count; Loop++)
                {
                    CCOrderedCollectionAppendElement(CCExpressionGetList(ListExpr), &(CCExpression){ CCExpressionReflectionSerialize(((const CCReflectCCList*)Type)->elementType, CCListGetElementAtIndex(List, Loop), Zone) });
                }
                
                return ListExpr;
            }
            
            else if ((((const CCReflectOpaque*)Type)->map == CCReflectCCCollectionMapper) && (((const CCReflectOpaque*)Type)->unmap == CCReflectCCCollectionUnmapper))
            {
                CCCollection Collection = *(CCCollection*)Data;
                
                if (!Collection) return CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":null"), TRUE);
                
                CCExpression List = CCExpressionCreateList(CC_STD_ALLOCATOR);
                
                CC_COLLECTION_FOREACH_PTR(void, Element, Collection)
                {
                    CCOrderedCollectionAppendElement(CCExpressionGetList(List), &(CCExpression){ CCExpressionReflectionSerialize(((const CCReflectCCCollection*)Type)->elementType, Element, Zone) });
                }
                
                return List;
            }
            
            else if ((((const CCReflectOpaque*)Type)->map == CCReflectCCDictionaryMapper) && (((const CCReflectOpaque*)Type)->unmap == CCReflectCCDictionaryUnmapper))
            {
                CCDictionary Dictionary = *(CCDictionary*)Data;
                
                if (!Dictionary) return CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":null"), TRUE);
                
                CCExpression List = CCExpressionCreateList(CC_STD_ALLOCATOR);
                
                CC_DICTIONARY_FOREACH_KEY_PTR(void, Key, Dictionary)
                {
                    CCExpression Pair = CCExpressionCreateList(CC_STD_ALLOCATOR);
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Pair), &(CCExpression){ CCExpressionReflectionSerialize(((const CCReflectCCDictionary*)Type)->keyType, Key, Zone) });
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Pair), &(CCExpression){ CCExpressionReflectionSerialize(((const CCReflectCCDictionary*)Type)->valueType, CCDictionaryGetValue(Dictionary, Key), Zone) });
                    CCOrderedCollectionAppendElement(CCExpressionGetList(List), &Pair);
                }
                
                return List;
            }
            
            CCExpression Result = NULL;
            
            ((const CCReflectOpaque*)Type)->map(Type, Data, &(CCExpressionReflectionSerializeHandlerArgs){
                .result = &Result,
                .zone = Zone
            }, (CCReflectTypeHandler)CCExpressionReflectionSerializeHandler, Zone, CC_AUTORELEASE_ALLOCATOR(Zone), CCReflectMapIntentSerialize);
            
            CCAssertLog(Result, "Result should be valid expression");
            
            return Result;
        }
            
        case CCReflectTypeArray:
        {
            CCExpression Expr = CCExpressionCreateList(CC_STD_ALLOCATOR);
            
            CCReflectType ElementType = ((const CCReflectArray*)Type)->type;
            const size_t ElementSize = CCReflectTypeSize(ElementType);
            
            for (size_t Loop = 0, Count = ((const CCReflectArray*)Type)->count; Loop < Count; Loop++)
            {
                CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionReflectionSerialize(ElementType, Data + (ElementSize * Loop), Zone) });
            }
            
            return Expr;
        }
            
        case CCReflectTypeValidator:
            return CCExpressionReflectionSerialize(((const CCReflectValidator*)Type)->type, Data, Zone);
            
        case CCReflectTypeEnumerable:
        {
            CCExpression Expr = CCExpressionCreateList(CC_STD_ALLOCATOR);
            
            CCEnumerable Enumerable = *(const CCEnumerable*)Data;
            CCReflectType ElementType = ((const CCReflectEnumerable*)Type)->type;
            
            for (const void *Element = CCEnumerableGetCurrent(&Enumerable); Element; Element = CCEnumerableNext(&Enumerable))
            {
                CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionReflectionSerialize(ElementType, Element, Zone) });
            }
            
            return Expr;
        }
    }
    
    return NULL;
}

static _Bool CCExpressionReflectionListIsFallback(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 2)
    {
        CCExpression TypeExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0);
        
        if (CCExpressionGetType(Expression) == CCExpressionValueTypeList)
        {
            if (CCCollectionGetCount(CCExpressionGetList(Expression)) >= 1)
            {
                CCExpression TypeNameExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(TypeExpr), 0);
                
                if (CCExpressionGetType(TypeNameExpr) == CCExpressionValueTypeAtom)
                {
                    CCString Name = CCExpressionGetAtom(TypeNameExpr);
                    
                    CCString Names[] = {
                        CC_STRING(":pointer"),
                        CC_STRING(":integer"),
                        CC_STRING(":float"),
                        CC_STRING(":struct"),
                        CC_STRING(":opaque"),
                        CC_STRING(":array"),
                        CC_STRING(":validator"),
                        CC_STRING(":enumerable")
                    };
                    
                    for (size_t Loop = 0; Loop < sizeof(Names) / sizeof(*Names); Loop++)
                    {
                        if (CCStringEqual(Name, Names[Loop]))
                        {
                            return TRUE;
                        }
                    }
                }
            }
        }
    }
    
    return FALSE;
}

void CCExpressionReflectionDeserialize(CCExpression Expression, CCReflectType Type, void *Data, CCMemoryZone Zone, CCAllocatorType Allocator)
{
    CCAssertLog(Type, "Type must not be null");
    CCAssertLog(Expression, "Expression must not be null");
    CCAssertLog(Data, "Data must not be null");
    CCAssertLog(Zone, "Zone must not be null");
    
    switch (*(const CCReflectTypeID*)Type)
    {
        case CCReflectTypePointer:
        {
            switch (CCExpressionGetType(Expression))
            {
                case CCExpressionValueTypeList:
                {
                    CCCollection List = CCExpressionGetList(Expression);
                    if (CCCollectionGetCount(List))
                    {
                        if (((const CCReflectPointer*)Type)->storage == CCReflectStorageDynamic)
                        {
                            CCAllocatorType CustomAllocator = ((const CCReflectDynamicPointer*)Type)->allocator;
                            if (CustomAllocator.allocator == CC_NULL_ALLOCATOR.allocator) CustomAllocator = Allocator;
                            
                            const size_t Size = CCReflectTypeSize(((const CCReflectPointer*)Type)->type);
                            
                            CCExpression Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(List, 0); // TODO: Other elements could be options to customise the allocation
                            
                            void *Ptr = CCMalloc(CustomAllocator, Size, NULL, CC_DEFAULT_ERROR_CALLBACK);
                            CCExpressionReflectionDeserialize(Expr, ((const CCReflectPointer*)Type)->type, Ptr, Zone, Allocator);
                            
                            *(void**)Data = Ptr;
                            
                            if (((const CCReflectDynamicPointer*)Type)->destructor)
                            {
                                CCMemorySetDestructor(Ptr, ((const CCReflectDynamicPointer*)Type)->destructor);
                            }
                        }
                        
                        else
                        {
                            CCExpression Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(List, 0);
                            
                            if ((CCExpressionGetType(Expr) == CCExpressionValueTypeList) && (CCCollectionGetCount(CCExpressionGetList(Expr)) == 2))
                            {
                                CCExpression TypeExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expr), 0);
                                CCExpression DataExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expr), 1);
                                
                                CCReflectType MappedType = CCExpressionReflectionDeserializeType(TypeExpr, Zone);
                                void *MappedData = CCMemoryZoneAllocate(Zone, CCReflectTypeSize(MappedType));
                                CCExpressionReflectionDeserialize(DataExpr, MappedType, MappedData, Zone, CC_AUTORELEASE_ALLOCATOR(Zone));
                                
                                ((const CCReflectStaticPointer*)Type)->unmap(Type, MappedType, MappedData, &(CCExpressionReflectionDeserializeHandlerArgs){
                                    .dest = Data,
                                    .allocator = CC_AUTORELEASE_ALLOCATOR(Zone),
                                    .zone = Zone
                                }, (CCReflectTypeHandler)CCExpressionReflectionDeserializeHandler, Zone, Allocator);
                            }
                            
                            else CC_LOG_ERROR_CUSTOM("Deserializing malformed static pointer. Expects a list of two lists: (type:list data:list).");
                        }
                    }
                    
                    else *(void**)Data = NULL;
                    
                    break;
                }
                    
                case CCExpressionValueTypeAtom:
                    if (CCStringEqual(CCExpressionGetAtom(Expression), CC_STRING(":null")))
                    {
                        *(void**)Data = NULL;
                    }
                    break;
                    
                default:
                    CC_LOG_ERROR_CUSTOM("Deserializing malformed pointer. Expects a list or :null");
                    break;
            }
            
            break;
        }
            
        case CCReflectTypeInteger:
            CCExpressionReflectionDeserializeInteger(Type, Expression, Data);
            break;
            
        case CCReflectTypeFloat:
            CCExpressionReflectionDeserializeFloat(Type, Expression, Data);
            break;
            
        case CCReflectTypeStruct:
            if (CCExpressionGetType(Expression) == CCExpressionValueTypeList)
            {
                const size_t Size = CC_ALIGN(((const CCReflectStruct*)Type)->count, (sizeof(uint8_t) * 8)) / (sizeof(uint8_t) * 8);
                uint8_t Initialised[Size];
                
                memset(Initialised, 0, Size);
                
                CC_COLLECTION_FOREACH(CCExpression, Element, CCExpressionGetList(Expression))
                {
                    if ((CCExpressionGetType(Element) == CCExpressionValueTypeList) && (CCCollectionGetCount(CCExpressionGetList(Element)) == 2))
                    {
                        CCExpression NameExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Element), 0);
                        CCExpression DataExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Element), 1);
                        
                        if (CCExpressionGetType(NameExpr) == CCExpressionValueTypeAtom)
                        {
                            CCString Name = CCExpressionGetAtom(NameExpr);
                            const size_t Length = CCStringGetLength(Name);
                            
                            for (size_t Loop = 0, Count = ((const CCReflectStruct*)Type)->count; Loop < Count; Loop++)
                            {
                                if (((const CCReflectStruct*)Type)->fields[Loop].name)
                                {
                                    if (CCStringHasPrefix(Name, ((const CCReflectStruct*)Type)->fields[Loop].name))
                                    {
                                        const size_t MatchLength = CCStringGetLength(((const CCReflectStruct*)Type)->fields[Loop].name);
                                        
                                        if ((Length == MatchLength + 1) && (CCStringGetCharacterAtIndex(Name, MatchLength) == ':'))
                                        {
                                            if (!((Initialised[Loop / 8] >> (Loop % 8)) & 1))
                                            {
                                                CCExpressionReflectionDeserialize(DataExpr, ((const CCReflectStruct*)Type)->fields[Loop].type, Data + ((const CCReflectStruct*)Type)->fields[Loop].offset, Zone, Allocator);
                                                
                                                Initialised[Loop / 8] |= 1 << (Loop % 8);
                                            }
                                            
                                            else CC_LOG_ERROR_CUSTOM("Deserializing duplicate struct field (%S).", Name);
                                            
                                            goto MatchedName;
                                        }
                                    }
                                }
                            }
                            
                            CC_LOG_ERROR_CUSTOM("Deserializing unknown struct field (%S).", Name);
                            
                        MatchedName:;
                        }
                        
                        else CC_LOG_ERROR_CUSTOM("Deserializing malformed struct field. Expects option argument.");
                    }
                    
                    else CC_LOG_ERROR_CUSTOM("Deserializing malformed struct field. Expects option argument.");
                }
            }
            
            else CC_LOG_ERROR_CUSTOM("Deserializing malformed struct. Expects a list of option arguments.");
            break;
            
        case CCReflectTypeOpaque:
            if ((((const CCReflectOpaque*)Type)->map == CC_REFLECT(CCString).map) && (((const CCReflectOpaque*)Type)->unmap == CC_REFLECT(CCString).unmap))
            {
                switch (CCExpressionGetType(Expression))
                {
                    case CCExpressionValueTypeString:
                        *(CCString*)Data = CCStringCopy(CCExpressionGetString(Expression));
                        return;
                        
                    case CCExpressionValueTypeInteger:
                        *(CCString*)Data = 0;
                        return;
                    
                    default:
                        break;
                }
            }
            
            else if ((((const CCReflectOpaque*)Type)->map == CCReflectCCArrayMapper) && (((const CCReflectOpaque*)Type)->unmap == CCReflectCCArrayUnmapper))
            {
                if (CCExpressionGetType(Expression) == CCExpressionValueTypeAtom)
                {
                    if (CCStringEqual(CCExpressionGetAtom(Expression), CC_STRING(":null")))
                    {
                        *(CCLinkedList*)Data = NULL;
                        
                        return;
                    }
                }
                
                else if ((CCExpressionGetType(Expression) == CCExpressionValueTypeList) && (!CCExpressionReflectionListIsFallback(Expression)))
                {
                    CCAllocatorType CustomAllocator = ((const CCReflectCCArray*)Type)->allocator;
                    if (CustomAllocator.allocator == CC_NULL_ALLOCATOR.allocator) CustomAllocator = Allocator;
                    
                    CCArray Array = CCArrayCreate(CustomAllocator, CCReflectTypeSize(((const CCReflectCCArray*)Type)->elementType), ((const CCReflectCCArray*)Type)->chunkSize);
                    
                    CC_COLLECTION_FOREACH(CCExpression, Element, CCExpressionGetList(Expression))
                    {
                        const size_t Index = CCArrayAppendElement(Array, NULL);
                        CCExpressionReflectionDeserialize(Element, ((const CCReflectCCArray*)Type)->elementType, CCArrayGetElementAtIndex(Array, Index), Zone, Allocator);
                    }
                    
                    *(CCArray*)Data = Array;
                    
                    return;
                }
            }
            
            else if ((((const CCReflectOpaque*)Type)->map == CCReflectCCLinkedListMapper) && (((const CCReflectOpaque*)Type)->unmap == CCReflectCCLinkedListUnmapper))
            {
                if (CCExpressionGetType(Expression) == CCExpressionValueTypeAtom)
                {
                    if (CCStringEqual(CCExpressionGetAtom(Expression), CC_STRING(":null")))
                    {
                        *(CCLinkedList*)Data = NULL;
                        
                        return;
                    }
                }
                
                else if ((CCExpressionGetType(Expression) == CCExpressionValueTypeList) && (!CCExpressionReflectionListIsFallback(Expression)))
                {
                    CCAllocatorType CustomAllocator = ((const CCReflectCCLinkedList*)Type)->allocator;
                    if (CustomAllocator.allocator == CC_NULL_ALLOCATOR.allocator) CustomAllocator = Allocator;
                    
                    const size_t ElementSize = CCReflectTypeSize(((const CCReflectCCLinkedList*)Type)->elementType);
                    
                    CCLinkedList LinkedList = NULL;
                    
                    CCLinkedListNode *Node = CCLinkedListCreateNode(CustomAllocator, ElementSize, NULL);
                    
                    CC_COLLECTION_FOREACH(CCExpression, Element, CCExpressionGetList(Expression))
                    {
                        CCLinkedListNode *Node = CCLinkedListCreateNode(CustomAllocator, ElementSize, NULL);
                        CCExpressionReflectionDeserialize(Element, ((const CCReflectCCLinkedList*)Type)->elementType, CCLinkedListGetNodeData(Node), Zone, Allocator);
                        
                        if (LinkedList) CCLinkedListAppend(LinkedList, Node);
                        else LinkedList = Node;
                    }
                    
                    *(CCLinkedList*)Data = LinkedList;
                    
                    return;
                }
            }
            
            else if ((((const CCReflectOpaque*)Type)->map == CCReflectCCListMapper) && (((const CCReflectOpaque*)Type)->unmap == CCReflectCCListUnmapper))
            {
                if (CCExpressionGetType(Expression) == CCExpressionValueTypeAtom)
                {
                    if (CCStringEqual(CCExpressionGetAtom(Expression), CC_STRING(":null")))
                    {
                        *(CCLinkedList*)Data = NULL;
                        
                        return;
                    }
                }
                
                else if ((CCExpressionGetType(Expression) == CCExpressionValueTypeList) && (!CCExpressionReflectionListIsFallback(Expression)))
                {
                    CCAllocatorType CustomAllocator = ((const CCReflectCCList*)Type)->allocator;
                    if (CustomAllocator.allocator == CC_NULL_ALLOCATOR.allocator) CustomAllocator = Allocator;
                    
                    CCList List = CCListCreate(CustomAllocator, CCReflectTypeSize(((const CCReflectCCList*)Type)->elementType), ((const CCReflectCCList*)Type)->chunkSize, ((const CCReflectCCList*)Type)->pageSize);
                    
                    CC_COLLECTION_FOREACH(CCExpression, Element, CCExpressionGetList(Expression))
                    {
                        const size_t Index = CCListAppendElement(List, NULL);
                        CCExpressionReflectionDeserialize(Element, ((const CCReflectCCList*)Type)->elementType, CCListGetElementAtIndex(List, Index), Zone, Allocator);
                    }
                    
                    *(CCList*)Data = List;
                    
                    return;
                }
            }
            
            else if ((((const CCReflectOpaque*)Type)->map == CCReflectCCCollectionMapper) && (((const CCReflectOpaque*)Type)->unmap == CCReflectCCCollectionUnmapper))
            {
                if (CCExpressionGetType(Expression) == CCExpressionValueTypeAtom)
                {
                    if (CCStringEqual(CCExpressionGetAtom(Expression), CC_STRING(":null")))
                    {
                        *(CCLinkedList*)Data = NULL;
                        
                        return;
                    }
                }
                
                else if ((CCExpressionGetType(Expression) == CCExpressionValueTypeList) && (!CCExpressionReflectionListIsFallback(Expression)))
                {
                    CCAllocatorType CustomAllocator = ((const CCReflectCCCollection*)Type)->allocator;
                    if (CustomAllocator.allocator == CC_NULL_ALLOCATOR.allocator) CustomAllocator = Allocator;
                    
                    CCCollection Collection = CCCollectionCreate(CustomAllocator, ((const CCReflectCCCollection*)Type)->hint, CCReflectTypeSize(((const CCReflectCCCollection*)Type)->elementType), ((const CCReflectCCCollection*)Type)->elementDestructor);
                    
                    if (((const CCReflectCCCollection*)Type)->hint & CCCollectionHintOrdered)
                    {
                        CC_COLLECTION_FOREACH(CCExpression, Element, CCExpressionGetList(Expression))
                        {
                            CCCollectionEntry Entry = CCOrderedCollectionAppendElement(Collection, NULL);
                            CCExpressionReflectionDeserialize(Element, ((const CCReflectCCCollection*)Type)->elementType, CCCollectionGetElement(Collection, Entry), Zone, Allocator);
                        }
                    }
                    
                    else
                    {
                        CC_COLLECTION_FOREACH(CCExpression, Element, CCExpressionGetList(Expression))
                        {
                            CCCollectionEntry Entry = CCCollectionInsertElement(Collection, NULL);
                            CCExpressionReflectionDeserialize(Element, ((const CCReflectCCCollection*)Type)->elementType, CCCollectionGetElement(Collection, Entry), Zone, Allocator);
                        }
                    }
                    
                    *(CCCollection*)Data = Collection;
                    
                    return;
                }
            }
            
            else if ((((const CCReflectOpaque*)Type)->map == CCReflectCCDictionaryMapper) && (((const CCReflectOpaque*)Type)->unmap == CCReflectCCDictionaryUnmapper))
            {
                if (CCExpressionGetType(Expression) == CCExpressionValueTypeAtom)
                {
                    if (CCStringEqual(CCExpressionGetAtom(Expression), CC_STRING(":null")))
                    {
                        *(CCLinkedList*)Data = NULL;
                        
                        return;
                    }
                }
                
                else if ((CCExpressionGetType(Expression) == CCExpressionValueTypeList) && (!CCExpressionReflectionListIsFallback(Expression)))
                {
                    CCAllocatorType CustomAllocator = ((const CCReflectCCDictionary*)Type)->allocator;
                    if (CustomAllocator.allocator == CC_NULL_ALLOCATOR.allocator) CustomAllocator = Allocator;
                    
                    const size_t KeySize = CCReflectTypeSize(((const CCReflectCCDictionary*)Type)->keyType);
                    
                    void *Key = CCMemoryZoneAllocate(Zone, KeySize);
                    
                    CCDictionary Dictionary = CCDictionaryCreate(CustomAllocator, ((const CCReflectCCDictionary*)Type)->hint, KeySize, CCReflectTypeSize(((const CCReflectCCDictionary*)Type)->valueType), ((const CCReflectCCDictionary*)Type)->callbacks);
                    
                    CC_COLLECTION_FOREACH(CCExpression, KVExpr, CCExpressionGetList(Expression))
                    {
                        if (CCExpressionGetType(KVExpr) == CCExpressionValueTypeList)
                        {
                            CCCollection KV = CCExpressionGetList(KVExpr);
                            
                            if (CCCollectionGetCount(KV) == 2)
                            {
                                CCExpression KeyExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(KV, 0);
                                CCExpression ValueExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(KV, 1);
                                
                                CCExpressionReflectionDeserialize(KeyExpr, ((const CCReflectCCDictionary*)Type)->keyType, Key, Zone, Allocator);
                                
                                CCDictionaryEntry Entry = CCDictionaryEntryForKey(Dictionary, Key);
                                CCExpressionReflectionDeserialize(ValueExpr, ((const CCReflectCCDictionary*)Type)->valueType, CCDictionaryGetEntry(Dictionary, Entry), Zone, Allocator);
                            }
                            
                            else goto SkipDictionary;
                        }
                        
                        else goto SkipDictionary;
                    }
                    
                    *(CCDictionary*)Data = Dictionary;
                    
                    return;
                    
                SkipDictionary:
                    
                    CCDictionaryDestroy(Dictionary);
                }
            }
            
            if ((CCExpressionGetType(Expression) == CCExpressionValueTypeList) && (CCCollectionGetCount(CCExpressionGetList(Expression)) == 2))
            {
                CCExpression TypeExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0);
                CCExpression DataExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1);
                
                CCReflectType MappedType = CCExpressionReflectionDeserializeType(TypeExpr, Zone);
                void *MappedData = CCMemoryZoneAllocate(Zone, CCReflectTypeSize(MappedType));
                CCExpressionReflectionDeserialize(DataExpr, MappedType, MappedData, Zone, CC_AUTORELEASE_ALLOCATOR(Zone));
                
                ((const CCReflectOpaque*)Type)->unmap(Type, MappedType, MappedData, &(CCExpressionReflectionDeserializeHandlerArgs){
                    .dest = Data,
                    .allocator = Allocator,
                    .zone = Zone
                }, (CCReflectTypeHandler)CCExpressionReflectionDeserializeHandler, Zone, Allocator);
            }
            
            else CC_LOG_ERROR_CUSTOM("Deserializing malformed opaque. Expects a list of two lists: (type:list data:list).");
            break;
            
        case CCReflectTypeArray:
            if (CCExpressionGetType(Expression) == CCExpressionValueTypeList)
            {
                CCReflectType ElementType = ((const CCReflectArray*)Type)->type;
                const size_t ElementSize = CCReflectTypeSize(ElementType);
                
                size_t Index = 0;
                CC_COLLECTION_FOREACH(CCExpression, Element, CCExpressionGetList(Expression))
                {
                    CCExpressionReflectionDeserialize(Element, ElementType, Data + (ElementSize * Index++), Zone, Allocator);
                }
            }
            
            else CC_LOG_ERROR_CUSTOM("Deserializing malformed array. Expects a list.");
            break;
            
        case CCReflectTypeValidator:
            CCExpressionReflectionDeserialize(Expression, ((const CCReflectValidator*)Type)->type, Data, Zone, Allocator);
            CCReflectValidate(Type, Data, Zone, Allocator);
            break;
            
        case CCReflectTypeEnumerable:
            if (CCExpressionGetType(Expression) == CCExpressionValueTypeList)
            {
                const size_t Count = CCCollectionGetCount(CCExpressionGetList(Expression));
                CCReflectType ElementType = ((const CCReflectEnumerable*)Type)->type;
                const size_t ElementSize = CCReflectTypeSize(ElementType);
                
                CCMemoryZoneBlock *Block = CCMemoryZoneGetCurrentBlock(Zone);
                ptrdiff_t Offset = CCMemoryZoneBlockGetCurrentOffset(Block);
                
                CC_COLLECTION_FOREACH(CCExpression, Expr, CCExpressionGetList(Expression))
                {
                    void *Element = CCMemoryZoneAllocate(Zone, ElementSize);
                    
                    CCMemoryZoneSave(Zone);
                    CCExpressionReflectionDeserialize(Expr, ElementType, Element, Zone, Allocator);
                    CCMemoryZoneRestore(Zone);
                }
                
                CCMemoryZoneBlockGetEnumerable(Block, Offset, ElementSize, Count, Data);
            }
            
            else CC_LOG_ERROR_CUSTOM("Deserializing malformed enumerable. Expects a list.");
            break;
    }
}
