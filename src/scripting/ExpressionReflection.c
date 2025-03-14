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
#include "ExpressionHelpers.h"
#include "ReflectedTypes.h"

static const CCString HexPrefixes[2] = { CC_STRING("0x"), CC_STRING("-0x") };

static CCExpression CCExpressionReflectionSerializeHex(const void *Data, size_t Size, size_t Start, size_t Invert, _Bool Signed)
{
    CCString String;
    
    uint8_t InvertMultiplier;
    uint8_t InvertAdjust;
    
    if (Signed)
    {
        const uint8_t Sign = ((const uint8_t*)Data)[Start + (Invert * (Size - 1))] >> 7;
        
        String = HexPrefixes[Sign];
        
        InvertMultiplier = Sign ? 0xff : 1;
        InvertAdjust = Sign;
    }
    
    else
    {
        String = HexPrefixes[0];
        
        InvertMultiplier = 1;
        InvertAdjust = 0;
    }
    
    size_t Max = Size - 1;
    while (Max && !(uint8_t)((InvertMultiplier * ((const uint8_t*)Data)[Start + (Invert * Max)]) - InvertAdjust)) Max--;

    static const char Hex[] = "0123456789abcdef";
    char Byte[3] = { 0, 0, 0 };
    uint8_t Carry = InvertAdjust;
    CCString Digits = CC_STRING("");
    
    for (size_t Loop = 0; Loop < Max + 1; Loop++)
    {
        const uint8_t Value = (InvertMultiplier * ((const uint8_t*)Data)[Start + (Invert * Loop)]) - InvertAdjust + Carry;
        
        if ((Carry) && (Value)) Carry = 0;
        
        Byte[0] = Hex[Value >> 4];
        Byte[1] = Hex[Value & 0xf];
        
        CCString ByteStr = CCStringCreate(CC_STD_ALLOCATOR, (CCStringHint)CCStringEncodingASCII, Byte);
        CCString NewStr = CCStringCreateByJoiningStrings((CCString[2]){ ByteStr, Digits }, 2, 0);
        CCStringDestroy(ByteStr);
        CCStringDestroy(Digits);
        
        Digits = NewStr;
    }
    
    CCString NewStr = CCStringCreateByJoiningStrings((CCString[2]){ String, Digits }, 2, 0);
    CCStringDestroy(Digits);
    CCStringDestroy(String);
    
    return CCExpressionCreateString(CC_STD_ALLOCATOR, NewStr, FALSE);
}

static void CCExpressionReflectionDeserializeHex(CCExpression Expression, void *Data, size_t Size, CCReflectEndian Endian, _Bool Sign)
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
    _Bool Neg = FALSE;
    const size_t Offset = CCStringHasPrefix(String, CC_STRING("0x")) ? 2 : ((Neg = CCStringHasPrefix(String, CC_STRING("-0x"))) ? 3 : 0);
    
    memset(Data, 0, Size);
    
    if ((Offset) && ((Sign) || (!Neg)))
    {
        CC_STRING_TEMP_BUFFER(Buffer, String)
        {
            const char *Hex = Buffer + Offset;
            
            size_t Length = 0;
            for (char Chr; ((Chr = Hex[Length]) >= '0') && ((Chr <= '9') || ((Chr <= 'f') && (Chr >= 'a')) || ((Chr <= 'F') && (Chr >= 'A'))); Length++);
            
            if ((Length / 2) <= Size)
            {
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
                
                if (Neg)
                {
                    const size_t Index = PrintStart + (PrintInvert * (Size - 1));
                    
                    if (((uint8_t*)Data)[Index] & 0x80)
                    {
                        memset(Data, 0, Size);
                    }
                    
                    else
                    {
                        for (size_t Loop = 0; Loop < Size; Loop++)
                        {
                            ((uint8_t*)Data)[Loop] = ~((uint8_t*)Data)[Loop];
                        }
                        
                        for (size_t Loop = 0; Loop < Size; Loop++)
                        {
                            const size_t Index = PrintStart + (PrintInvert * Loop);
                            ((uint8_t*)Data)[Index] += 1;
                            
                            if (!((uint8_t*)Data)[Index]) break;
                        }
                    }
                    
                    ((uint8_t*)Data)[Index] |= 0x80;
                }
                
                else if (Sign)
                {
                    const size_t Index = PrintStart + (PrintInvert * (Size - 1));
                    
                    if (((uint8_t*)Data)[Index] & 0x80)
                    {
                        memset(Data, 0xff, Size);
                        
                        ((uint8_t*)Data)[Index] ^= 0x80;
                    }
                }
            }
            
            else
            {
                if (!Neg) memset(Data, 0xff, Size);
                
                if (Sign)
                {
                    const size_t Index = PrintStart + (PrintInvert * (Size - 1));
                    ((uint8_t*)Data)[Index] ^= 0x80;
                }
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
    
    return CCExpressionReflectionSerializeHex(Data, Size, Start, Invert, Integer->sign);
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
    
    if (Integer->sign) ((uint8_t*)Data)[Start + (Invert * (Size - 1))] ^= 0x80;
    
    if (CCReflectValidateMaximum(&CC_REFLECT(intmax_t), &Value, &CC_REFLECT_VALIDATE_MAXIMUM(&CC_REFLECT(intmax_t), CC_REFLECT_VALUE(Integer, Data), CC_REFLECT_RESULT_DEFAULT, CC_REFLECT_RESULT_DEFAULT)))
    {
        memset(Data, 0, Integer->size);
        
        for (size_t Loop = 0, Count = CCMin(Size, sizeof(intmax_t)); Loop < Count; Loop++)
        {
            ((uint8_t*)Data)[Start + (Invert * Loop)] = ((uint8_t*)&Value)[Loop];
        }
        
        if (Integer->sign) ((uint8_t*)Data)[Start + (Invert * (Size - 1))] = ((uint8_t*)Data)[Start + (Invert * (Size - 1))] & ~0x80;
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
            CCExpressionReflectionDeserializeHex(Expression, Data, Integer->size, Integer->endian, Integer->sign);
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
    
    return CCExpressionReflectionSerializeHex(Data, Size, Start, Invert, FALSE);
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
            
#if LDBL_MAX_EXP > DBL_MAX_EXP
        case sizeof(long double):
            *(long double*)Data = Value;
            break;
#endif
            
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
            CCExpressionReflectionDeserializeHex(Expression, Data, Float->size, Float->endian, FALSE);
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

#define CC_EXPRESSION_REFLECTION_SERIALIZE_TYPE(type, index) CC_EXPRESSION_REFLECTION_SERIALIZE_TYPE_(CC_EXPAND type)
#define CC_EXPRESSION_REFLECTION_SERIALIZE_TYPE_(...) CC_EXPRESSION_REFLECTION_SERIALIZE_TYPE__(__VA_ARGS__)
#define CC_EXPRESSION_REFLECTION_SERIALIZE_TYPE__(type, atom) (Type == &CC_REFLECT(type)) return CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":" #atom), TRUE);

#define CC_EXPRESSION_REFLECTION_SERIALIZE_TYPES(...) if CC_SOFT_JOIN(else if, CC_MAP(CC_EXPRESSION_REFLECTION_SERIALIZE_TYPE, __VA_ARGS__))

#define CC_EXPRESSION_REFLECTION_DESERIALIZE_TYPE(type, index) CC_EXPRESSION_REFLECTION_DESERIALIZE_TYPE_(CC_EXPAND type)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_TYPE_(...) CC_EXPRESSION_REFLECTION_DESERIALIZE_TYPE__(__VA_ARGS__)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_TYPE__(type, atom) (CCStringEqual(Name, CC_STRING(":" #atom))) return &CC_REFLECT(type);

#define CC_EXPRESSION_REFLECTION_DESERIALIZE_TYPES(...) if CC_SOFT_JOIN(else if, CC_MAP(CC_EXPRESSION_REFLECTION_DESERIALIZE_TYPE, __VA_ARGS__))

#define CC_EXPRESSION_REFLECTION_SERIALIZE_WRAPPED_TYPE(type, index) CC_EXPRESSION_REFLECTION_SERIALIZE_WRAPPED_TYPE_(CC_EXPAND type)
#define CC_EXPRESSION_REFLECTION_SERIALIZE_WRAPPED_TYPE_(...) CC_EXPRESSION_REFLECTION_SERIALIZE_WRAPPED_TYPE__(__VA_ARGS__)
#define CC_EXPRESSION_REFLECTION_SERIALIZE_WRAPPED_TYPE__(type, atom, group) CC_EXPRESSION_REFLECTION_SERIALIZE_WRAPPED_TYPE_##group(type, atom)

#define CC_EXPRESSION_REFLECTION_SERIALIZE_WRAPPED_TYPE_pointer(args, index) CC_EXPRESSION_REFLECTION_SERIALIZE_WRAPPED_TYPE_pointer_ args
#define CC_EXPRESSION_REFLECTION_SERIALIZE_WRAPPED_TYPE_pointer_(type, atom) \
(Type == &CC_REFLECT(type)) \
{ \
    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("pointer:"), TRUE) }); \
    \
    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":" #atom), TRUE) }); \
    CC_EXPRESSION_REFLECTION_SERIALIZE_WRAPPED_TYPE_pointer_##type \
    \
    return Expr; \
}

#define CC_EXPRESSION_REFLECTION_SERIALIZE_WRAPPED_TYPE_pointer_PTYPE(type, ownership, storage) \
CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":" #ownership), TRUE) }); \
CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":" #storage), TRUE) });

#define CC_EXPRESSION_REFLECTION_SERIALIZE_WRAPPED_TYPE_dynamic_pointer(args, index) CC_EXPRESSION_REFLECTION_SERIALIZE_WRAPPED_TYPE_dynamic_pointer_ args
#define CC_EXPRESSION_REFLECTION_SERIALIZE_WRAPPED_TYPE_dynamic_pointer_(type, atom) \
CC_EXPRESSION_REFLECTION_SERIALIZE_WRAPPED_TYPE_pointer_(PTYPE(type, weak, dynamic), atom) \
else if CC_EXPRESSION_REFLECTION_SERIALIZE_WRAPPED_TYPE_pointer_(PTYPE(type, transfer, dynamic), atom) \
else if CC_EXPRESSION_REFLECTION_SERIALIZE_WRAPPED_TYPE_pointer_(PTYPE(type, retain, dynamic), atom)

#define CC_EXPRESSION_REFLECTION_SERIALIZE_WRAPPED_TYPES(...) CC_EXPRESSION_REFLECTION_SERIALIZE_WRAPPED_TYPES_(__VA_ARGS__)
#define CC_EXPRESSION_REFLECTION_SERIALIZE_WRAPPED_TYPES_(head, ...) CC_EXPRESSION_REFLECTION_SERIALIZE_WRAPPED_TYPES__(head, 0, if) CC_MAP_WITH(CC_EXPRESSION_REFLECTION_SERIALIZE_WRAPPED_TYPES__, else if, __VA_ARGS__)
#define CC_EXPRESSION_REFLECTION_SERIALIZE_WRAPPED_TYPES__(arg, index, statement) statement CC_EXPRESSION_REFLECTION_SERIALIZE_WRAPPED_TYPES___ arg
#define CC_EXPRESSION_REFLECTION_SERIALIZE_WRAPPED_TYPES___(group, ...) CC_SOFT_JOIN(else if, CC_MAP(CC_EXPRESSION_REFLECTION_SERIALIZE_WRAPPED_TYPE_##group, __VA_ARGS__))

#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPES(filter, ...) CC_SOFT_JOIN(, CC_RECURSIVE_0_MAP_WITH(CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPES_, filter, __VA_ARGS__))
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPES_(arg, index, filter) CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPES__(filter, CC_EXPAND arg)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPES__(...) CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPES___(__VA_ARGS__)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPES___(filter, group, ...) CC_MAP_WITH(CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_##group, filter, __VA_ARGS__)

#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_dynamic_pointer(arg, index, filter) CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_dynamic_pointer_(filter, CC_EXPAND arg)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_dynamic_pointer_(...) CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_dynamic_pointer__(__VA_ARGS__)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_dynamic_pointer__(filter, type, name) \
filter##_MATCH_weak_dynamic(PTYPE(type, weak, dynamic), name) \
filter##_MATCH_transfer_dynamic(PTYPE(type, transfer, dynamic), name) \
filter##_MATCH_retain_dynamic(PTYPE(type, retain, dynamic), name)

#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_pointer(arg, index, filter) CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_pointer_(filter, CC_EXPAND arg)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_pointer_(...) CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_pointer__(__VA_ARGS__)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_pointer__(filter, type, name) CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_pointer___(filter, type, name, CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_pointer_##type)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_pointer___(...) CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_pointer____(__VA_ARGS__)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_pointer____(filter, type, name, ownership, storage) filter##_MATCH_##ownership##_##storage(type, name)

#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_pointer_PTYPE(_, ownership, storage) ownership, storage

#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_MATCH(type, name) if (CCStringEqual(Name, CC_STRING(":" #name))) return &CC_REFLECT(type);

#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_weak_static_MATCH_weak_static(type, name) CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_MATCH(type, name)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_weak_static_MATCH_weak_dynamic(type, name)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_weak_static_MATCH_transfer_static(type, name)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_weak_static_MATCH_transfer_dynamic(type, name)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_weak_static_MATCH_retain_static(type, name)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_weak_static_MATCH_retain_dynamic(type, name)

#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_weak_dynamic_MATCH_weak_static(type, name)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_weak_dynamic_MATCH_weak_dynamic(type, name) CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_MATCH(type, name)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_weak_dynamic_MATCH_transfer_static(type, name)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_weak_dynamic_MATCH_transfer_dynamic(type, name)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_weak_dynamic_MATCH_retain_static(type, name)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_weak_dynamic_MATCH_retain_dynamic(type, name)

#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_transfer_static_MATCH_weak_static(type, name)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_transfer_static_MATCH_weak_dynamic(type, name)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_transfer_static_MATCH_transfer_static(type, name) CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_MATCH(type, name)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_transfer_static_MATCH_transfer_dynamic(type, name)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_transfer_static_MATCH_retain_static(type, name)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_transfer_static_MATCH_retain_dynamic(type, name)

#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_transfer_dynamic_MATCH_weak_static(type, name)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_transfer_dynamic_MATCH_weak_dynamic(type, name)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_transfer_dynamic_MATCH_transfer_static(type, name)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_transfer_dynamic_MATCH_transfer_dynamic(type, name) CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_MATCH(type, name)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_transfer_dynamic_MATCH_retain_static(type, name)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_transfer_dynamic_MATCH_retain_dynamic(type, name)

#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_retain_static_MATCH_weak_static(type, name)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_retain_static_MATCH_weak_dynamic(type, name)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_retain_static_MATCH_transfer_static(type, name)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_retain_static_MATCH_transfer_dynamic(type, name)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_retain_static_MATCH_retain_static(type, name) CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_MATCH(type, name)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_retain_static_MATCH_retain_dynamic(type, name)

#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_retain_dynamic_MATCH_weak_static(type, name)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_retain_dynamic_MATCH_weak_dynamic(type, name)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_retain_dynamic_MATCH_transfer_static(type, name)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_retain_dynamic_MATCH_transfer_dynamic(type, name)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_retain_dynamic_MATCH_retain_static(type, name)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_retain_dynamic_MATCH_retain_dynamic(type, name) CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_MATCH(type, name)



#define CC_EXPRESSION_REFLECTION_POINTER_TYPES \
    (CCReflectTypeMapper, CCReflectTypeMapper), \
    (CCReflectTypeUnmapper, CCReflectTypeUnmapper), \
    (CCReflectValidation, CCReflectValidation), \
    (CCMemoryDestructorCallback, CCMemoryDestructorCallback), \
    (CCComparator, CCComparator), \
    (CCCollectionElementDestructor, CCCollectionElementDestructor), \
    (CCHashMapKeyHasher, CCHashMapKeyHasher), \
    (CCDictionaryKeyHasher, CCDictionaryKeyHasher), \
    (CCDictionaryElementDestructor, CCDictionaryElementDestructor)

#define CC_EXPRESSION_REFLECTION_WRAPPED_POINTER_TYPES \
    (pointer, (PTYPE(CCHashMapInterface, weak, static), CCHashMapInterface)), \
    (dynamic_pointer, \
        (CCString, CCString), \
        (CCAllocatorType, CCAllocatorType), \
        (CCReflectType, CCReflectType), \
        (CCReflectTypeID, CCReflectTypeID), \
        (CCReflectStorage, CCReflectStorage), \
        (CCReflectOwnership, CCReflectOwnership), \
        (CCReflectPointer, CCReflectPointer), \
        (CCReflectStaticPointer, CCReflectStaticPointer), \
        (CCReflectDynamicPointer, CCReflectDynamicPointer), \
        (CCReflectEndian, CCReflectEndian), \
        (CCReflectInteger, CCReflectInteger), \
        (CCReflectFloat, CCReflectFloat), \
        (CCReflectStructField, CCReflectStructField), \
        (CCReflectStruct, CCReflectStruct), \
        (CCReflectArray, CCReflectArray), \
        (CCReflectTypeMapper, CCReflectTypeMapper), \
        (CCReflectTypeUnmapper, CCReflectTypeUnmapper), \
        (CCReflectOpaque, CCReflectOpaque), \
        (CCReflectEnumerable, CCReflectEnumerable), \
        (CCReflectValidation, CCReflectValidation), \
        (CCReflectValue, CCReflectValue), \
        (CCReflectValidator, CCReflectValidator), \
        (void, void), \
        (char, char), \
        (short, short), \
        (int, int), \
        (long, long), \
        (long long, long), \
        (unsigned char, unsigned), \
        (unsigned short, unsigned), \
        (unsigned int, unsigned), \
        (unsigned long, unsigned), \
        (unsigned long long, unsigned), \
        (int8_t, int8_t), \
        (int16_t, int16_t), \
        (int32_t, int32_t), \
        (int64_t, int64_t), \
        (uint8_t, uint8_t), \
        (uint16_t, uint16_t), \
        (uint32_t, uint32_t), \
        (uint64_t, uint64_t), \
        (ptrdiff_t, ptrdiff_t), \
        (intptr_t, intptr_t), \
        (uintptr_t, uintptr_t), \
        (size_t, size_t), \
        (int_fast8_t, int_fast8_t), \
        (int_fast16_t, int_fast16_t), \
        (int_fast32_t, int_fast32_t), \
        (int_fast64_t, int_fast64_t), \
        (int_least8_t, int_least8_t), \
        (int_least16_t, int_least16_t), \
        (int_least32_t, int_least32_t), \
        (int_least64_t, int_least64_t), \
        (uint_fast8_t, uint_fast8_t), \
        (uint_fast16_t, uint_fast16_t), \
        (uint_fast32_t, uint_fast32_t), \
        (uint_fast64_t, uint_fast64_t), \
        (uint_least8_t, uint_least8_t), \
        (uint_least16_t, uint_least16_t), \
        (uint_least32_t, uint_least32_t), \
        (uint_least64_t, uint_least64_t), \
        (intmax_t, intmax_t), \
        (uintmax_t, uintmax_t), \
        (float, float), \
        (double, double), \
        (long double, long), \
        (CCVector2D, CCVector2D), \
        (CCVector2Di, CCVector2Di), \
        (CCVector3D, CCVector3D), \
        (CCColourRGB, CCColourRGB), \
        (CCVector3Di, CCVector3Di), \
        (CCColourRGBi, CCColourRGBi), \
        (CCVector4D, CCVector4D), \
        (CCColourRGBA, CCColourRGBA), \
        (CCVector4Di, CCVector4Di), \
        (CCColourRGBAi, CCColourRGBAi), \
        (CCRect, CCRect), \
        (CCMatrix2, CCMatrix2), \
        (CCMatrix3, CCMatrix3), \
        (CCMatrix4, CCMatrix4), \
        (ARRAY(char, v8), v8), \
        (ARRAY(char, v16), v16), \
        (ARRAY(char, v32), v32), \
        (ARRAY(char, v64), v64), \
        (ARRAY(char, v128), v128), \
        (ARRAY(char, v256), v256), \
        (ARRAY(char, v512), v512), \
        (ARRAY(char, v1024), v1024), \
        (ARRAY(char, v4096), v4096), \
        (ARRAY(char, v65536), v65536), \
        (CCDictionaryCallbacks, CCDictionaryCallbacks) \
    )

#define CC_EXPRESSION_REFLECTION_INTEGER_TYPES \
    (_Bool, bool), \
    (char, char), \
    (short, short), \
    (int, int), \
    (long, long), \
    (long long, long_long), \
    (unsigned char, unsigned_char), \
    (unsigned short, unsigned_short), \
    (unsigned int, unsigned_int), \
    (unsigned long, unsigned_long), \
    (unsigned long long, unsigned_long_long), \
    (int8_t, int8_t), \
    (int16_t, int16_t), \
    (int32_t, int32_t), \
    (int64_t, int64_t), \
    (uint8_t, uint8_t), \
    (uint16_t, uint16_t), \
    (uint32_t, uint32_t), \
    (uint64_t, uint64_t), \
    (ptrdiff_t, ptrdiff_t), \
    (intptr_t, intptr_t), \
    (uintptr_t, uintptr_t), \
    (size_t, size_t), \
    (int_fast8_t, int_fast8_t), \
    (int_fast16_t, int_fast16_t), \
    (int_fast32_t, int_fast32_t), \
    (int_fast64_t, int_fast64_t), \
    (int_least8_t, int_least8_t), \
    (int_least16_t, int_least16_t), \
    (int_least32_t, int_least32_t), \
    (int_least64_t, int_least64_t), \
    (uint_fast8_t, uint_fast8_t), \
    (uint_fast16_t, uint_fast16_t), \
    (uint_fast32_t, uint_fast32_t), \
    (uint_fast64_t, uint_fast64_t), \
    (uint_least8_t, uint_least8_t), \
    (uint_least16_t, uint_least16_t), \
    (uint_least32_t, uint_least32_t), \
    (uint_least64_t, uint_least64_t), \
    (intmax_t, intmax_t), \
    (uintmax_t, uintmax_t), \
    (CCReflectTypeID, CCReflectTypeID), \
    (CCReflectStorage, CCReflectStorage), \
    (CCReflectOwnership, CCReflectOwnership), \
    (CCReflectEndian, CCReflectEndian), \
    (CCCollectionHint, CCCollectionHint), \
    (CCDictionaryHint, CCDictionaryHint)

#define CC_EXPRESSION_REFLECTION_FLOAT_TYPES \
    (float, float), \
    (double, double), \
    (long double, long_double)

#define CC_EXPRESSION_REFLECTION_STRUCT_TYPES \
    (CCVector2D,CCVector2D), \
    (CCVector2Di,CCVector2Di), \
    (CCVector3D,CCVector3D), \
    (CCColourRGB,CCColourRGB), \
    (CCVector3Di,CCVector3Di), \
    (CCColourRGBi,CCColourRGBi), \
    (CCVector4D,CCVector4D), \
    (CCColourRGBA,CCColourRGBA), \
    (CCVector4Di,CCVector4Di), \
    (CCColourRGBAi,CCColourRGBAi), \
    (CCRect,CCRect), \
    (CCMatrix2,CCMatrix2), \
    (CCMatrix3,CCMatrix3), \
    (CCMatrix4,CCMatrix4), \
    (CCReflectPointer,CCReflectPointer), \
    (CCReflectStaticPointer,CCReflectStaticPointer), \
    (CCReflectDynamicPointer,CCReflectDynamicPointer), \
    (CCReflectInteger,CCReflectInteger), \
    (CCReflectFloat,CCReflectFloat), \
    (CCReflectStructField,CCReflectStructField), \
    (CCReflectStruct,CCReflectStruct), \
    (CCReflectArray,CCReflectArray), \
    (CCReflectEnumerable,CCReflectEnumerable), \
    (CCReflectOpaque,CCReflectOpaque), \
    (CCReflectValue,CCReflectValue), \
    (CCReflectValidator,CCReflectValidator), \
    (CCDebugAllocatorInfo,CCDebugAllocatorInfo), \
    (CCDictionaryCallbacks,CCDictionaryCallbacks)

#define CC_EXPRESSION_REFLECTION_OPAQUE_TYPES \
    (void, void), \
    (CCReflectType, CCReflectType), \
    (CCString, CCString), \
    (ARRAY(char, v8), v8), \
    (ARRAY(char, v16), v16), \
    (ARRAY(char, v32), v32), \
    (ARRAY(char, v64), v64), \
    (ARRAY(char, v128), v128), \
    (ARRAY(char, v256), v256), \
    (ARRAY(char, v512), v512), \
    (ARRAY(char, v1024), v1024), \
    (ARRAY(char, v4096), v4096), \
    (ARRAY(char, v65536), v65536), \
    (CCAllocatorType, CCAllocatorType)

#undef CC_EXPRESSION_REFLECTION_SERIALIZE_TYPES
#undef CC_EXPRESSION_REFLECTION_DESERIALIZE_TYPES
#define CC_EXPRESSION_REFLECTION_SERIALIZE_TYPES(...)
#define CC_EXPRESSION_REFLECTION_DESERIALIZE_TYPES(...)

static CCExpression CCExpressionReflectionSerializeType(CCReflectType Type, CCMemoryZone Zone)
{
    CCExpression Expr = NULL;
    
    switch (*(const CCReflectTypeID*)Type)
    {
        case CCReflectTypePointer:
        {
            CC_EXPRESSION_REFLECTION_SERIALIZE_TYPES(CC_EXPRESSION_REFLECTION_POINTER_TYPES);
            
            Expr = CCExpressionCreateList(CC_STD_ALLOCATOR);
            
            CC_EXPRESSION_REFLECTION_SERIALIZE_WRAPPED_TYPES(CC_EXPRESSION_REFLECTION_WRAPPED_POINTER_TYPES);
            
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
            CC_EXPRESSION_REFLECTION_SERIALIZE_TYPES(CC_EXPRESSION_REFLECTION_INTEGER_TYPES);
            
            Expr = CCExpressionCreateList(CC_STD_ALLOCATOR);
            
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
            CC_EXPRESSION_REFLECTION_SERIALIZE_TYPES(CC_EXPRESSION_REFLECTION_FLOAT_TYPES);
            
            Expr = CCExpressionCreateList(CC_STD_ALLOCATOR);
            
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
            CC_EXPRESSION_REFLECTION_SERIALIZE_TYPES(CC_EXPRESSION_REFLECTION_STRUCT_TYPES);
            
            Expr = CCExpressionCreateList(CC_STD_ALLOCATOR);
            
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
                
                if (((const CCReflectStruct*)Type)->fields[Loop].name)
                {
                    Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("name:"), TRUE) });
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateString(CC_STD_ALLOCATOR, ((const CCReflectStruct*)Type)->fields[Loop].name, TRUE) });
                    CCOrderedCollectionAppendElement(CCExpressionGetList(StructField), &Field);
                }
                
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
            CC_EXPRESSION_REFLECTION_SERIALIZE_TYPES(CC_EXPRESSION_REFLECTION_OPAQUE_TYPES);
            
            Expr = CCExpressionCreateList(CC_STD_ALLOCATOR);
            
            if ((((const CCReflectOpaque*)Type)->map == CCReflectCCArrayMapper) && (((const CCReflectOpaque*)Type)->unmap == CCReflectCCArrayUnmapper))
            {
                CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("CCArray:"), TRUE) });
                
                CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionReflectionSerialize(&CC_REFLECT(CCAllocatorType), &((const CCReflectCCArray*)Type)->allocator, Zone) });
                
                CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionReflectionSerializeInteger(&CC_REFLECT(size_t), &((const CCReflectCCArray*)Type)->chunkSize) });
                
                CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionReflectionSerializeType(((const CCReflectCCArray*)Type)->elementType, Zone) });
                
                return Expr;
            }
            
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
            Expr = CCExpressionCreateList(CC_STD_ALLOCATOR);
            
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
            Expr = CCExpressionCreateList(CC_STD_ALLOCATOR);
            
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

static inline CCReflectOwnership CCExpressionReflectionResolvePointerOwnership(CCString Name)
{
    if (CCStringEqual(Name, CC_STRING(":weak")))
    {
        return CCReflectOwnershipWeak;
    }
    
    else if (CCStringEqual(Name, CC_STRING(":transfer")))
    {
        return CCReflectOwnershipTransfer;
    }
    
    else if (CCStringEqual(Name, CC_STRING(":retain")))
    {
        return CCReflectOwnershipRetain;
    }
    
    else CC_LOG_ERROR_CUSTOM("Invalid pointer ownership atom (%S). Expects ':weak', ':transfer', or ':retain'.", Name);
    
    return -1;
}

static inline CCReflectStorage CCExpressionReflectionResolvePointerStorage(CCString Name)
{
    if (CCStringEqual(Name, CC_STRING(":static")))
    {
        return CCReflectStorageStatic;
    }
    
    else if (CCStringEqual(Name, CC_STRING(":dynamic")))
    {
        return CCReflectStorageDynamic;
    }
    
    else CC_LOG_ERROR_CUSTOM("Invalid pointer storage atom (%S). Expects ':static', or ':dynamic'.", Name);
    
    return -1;
}

static CCReflectType CCExpressionReflectionDeserializeType(CCExpression Expression, CCMemoryZone Zone)
{
    if (CCExpressionGetType(Expression) == CCExpressionValueTypeAtom)
    {
        CCString Name = CCExpressionGetAtom(Expression);
        
        CC_EXPRESSION_REFLECTION_DESERIALIZE_TYPES(CC_EXPRESSION_REFLECTION_POINTER_TYPES, CC_EXPRESSION_REFLECTION_INTEGER_TYPES, CC_EXPRESSION_REFLECTION_FLOAT_TYPES, CC_EXPRESSION_REFLECTION_STRUCT_TYPES, CC_EXPRESSION_REFLECTION_OPAQUE_TYPES);
        
        CC_LOG_ERROR_CUSTOM("Deserializing unknown type (%S).", Name);
    }
    
    else if (CCExpressionGetType(Expression) == CCExpressionValueTypeList)
    {
        CCEnumerator Enumerator;
        CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
        
        CCExpression *TypeExpr = CCCollectionEnumeratorGetCurrent(&Enumerator);
        if ((TypeExpr) && (CCExpressionGetType(*TypeExpr) == CCExpressionValueTypeAtom))
        {
            CCString Type = CCExpressionGetAtom(*TypeExpr);
            
            if (CCStringEqual(Type, CC_STRING("CCArray:")))
            {
                if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 4)
                {
                    CCExpression AllocatorExpr = *(CCExpression*)CCCollectionEnumeratorNext(&Enumerator);
                    CCExpression ChunkSizeExpr = *(CCExpression*)CCCollectionEnumeratorNext(&Enumerator);
                    CCExpression ElementTypeExpr = *(CCExpression*)CCCollectionEnumeratorNext(&Enumerator);
                    

                    CCAllocatorType Allocator = CC_NULL_ALLOCATOR;
                    size_t ChunkSize;
                    CCReflectType ElementType = CCExpressionReflectionDeserializeType(ElementTypeExpr, Zone);
                    
                    CCExpressionReflectionDeserialize(AllocatorExpr, &CC_REFLECT(CCAllocatorType), &Allocator, Zone, CC_AUTORELEASE_ALLOCATOR(Zone));
                    
                    CCExpressionReflectionDeserializeInteger(&CC_REFLECT(size_t), ChunkSizeExpr, &ChunkSize);
                    
                    CCReflectCCArray *Array = CCMemoryZoneAllocate(Zone, sizeof(CCReflectCCArray));
                    
                    *Array = CC_REFLECT_CCArray(ElementType, Allocator, ChunkSize);
                    
                    return Array;
                    
                }
                
                CC_LOG_ERROR_CUSTOM("Deserializing unknown value for type CCArray:. Expects allocator, chunk-size, element-type.");
            }
            
            else if (CCStringEqual(Type, CC_STRING("pointer:")))
            {
                if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 4)
                {
                    CCExpression PointerTypeExpr = *(CCExpression*)CCCollectionEnumeratorNext(&Enumerator);
                    CCExpression OwnershipExpr = *(CCExpression*)CCCollectionEnumeratorNext(&Enumerator);
                    CCExpression StorageExpr = *(CCExpression*)CCCollectionEnumeratorNext(&Enumerator);
                    
                    if ((CCExpressionGetType(PointerTypeExpr) == CCExpressionValueTypeAtom) && (CCExpressionGetType(OwnershipExpr) == CCExpressionValueTypeAtom) && (CCExpressionGetType(StorageExpr) == CCExpressionValueTypeAtom))
                    {
                        CCString Name = CCExpressionGetAtom(PointerTypeExpr);
                        
                        const CCReflectOwnership Ownership = CCExpressionReflectionResolvePointerOwnership(CCExpressionGetAtom(OwnershipExpr));
                        const CCReflectStorage Storage = CCExpressionReflectionResolvePointerStorage(CCExpressionGetAtom(StorageExpr));
                        
                        _Static_assert((CCReflectStorageStatic == 0) &&
                                       (CCReflectStorageDynamic == 1), "Expects storage values to take first bit");
                        
                        switch ((Ownership << 1) | Storage)
                        {
                            case (CCReflectOwnershipWeak << 1) | CCReflectStorageStatic:
                                CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPES(CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_weak_static, CC_EXPRESSION_REFLECTION_WRAPPED_POINTER_TYPES);
                                break;
                                
                            case (CCReflectOwnershipWeak << 1) | CCReflectStorageDynamic:
                                CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPES(CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_weak_dynamic, CC_EXPRESSION_REFLECTION_WRAPPED_POINTER_TYPES);
                                break;
                                
                            case (CCReflectOwnershipTransfer << 1) | CCReflectStorageStatic:
                                CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPES(CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_transfer_static, CC_EXPRESSION_REFLECTION_WRAPPED_POINTER_TYPES);
                                break;
                                
                            case (CCReflectOwnershipTransfer << 1) | CCReflectStorageDynamic:
                                CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPES(CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_transfer_dynamic, CC_EXPRESSION_REFLECTION_WRAPPED_POINTER_TYPES);
                                break;
                                
                            case (CCReflectOwnershipRetain << 1) | CCReflectStorageStatic:
                                CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPES(CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_retain_static, CC_EXPRESSION_REFLECTION_WRAPPED_POINTER_TYPES);
                                break;
                                
                            case (CCReflectOwnershipRetain << 1) | CCReflectStorageDynamic:
                                CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPES(CC_EXPRESSION_REFLECTION_DESERIALIZE_WRAPPED_TYPE_FILTER_pointer_retain_dynamic, CC_EXPRESSION_REFLECTION_WRAPPED_POINTER_TYPES);
                                break;
                                
                            default:
                                break;
                        }
                    }
                }
                
                CC_LOG_ERROR_CUSTOM("Deserializing unknown value for type pointer:. Expects name, ownership, storage.");
            }
            
            else if (CCStringEqual(Type, CC_STRING(":pointer")))
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
                                    if (CCExpressionGetType(ArgExpr) == CCExpressionValueTypeAtom)
                                    {
                                        Pointer->storage = CCExpressionReflectionResolvePointerStorage(CCExpressionGetAtom(ArgExpr));
                                    }
                                    
                                    else CCExpressionReflectionDeserializeInteger(&CC_REFLECT(CCReflectStorage), ArgExpr, &Pointer->storage);
                                    
                                    if (Pointer->storage == CCReflectStorageDynamic) ((CCReflectDynamicPointer*)Pointer)->allocator = CC_NULL_ALLOCATOR;
                                }
                                
                                else if (CCStringEqual(Name, CC_STRING("ownership:")))
                                {
                                    if (CCExpressionGetType(ArgExpr) == CCExpressionValueTypeAtom)
                                    {
                                        Pointer->ownership = CCExpressionReflectionResolvePointerOwnership(CCExpressionGetAtom(ArgExpr));
                                    }
                                    
                                    else CCExpressionReflectionDeserializeInteger(&CC_REFLECT(CCReflectOwnership), ArgExpr, &Pointer->ownership);
                                }
                                
                                else if (Pointer->storage == CCReflectStorageStatic)
                                {
                                    if (CCStringEqual(Name, CC_STRING("descriptor:")))
                                    {
                                        CCExpression DescriptorTypeExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(ArgExpr), 0);
                                        CCExpression DescriptorDataExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(ArgExpr), 1);
                                        
                                        CCReflectType MappedType = CCExpressionReflectionDeserializeType(DescriptorTypeExpr, Zone);
                                        void *MappedData = CCMemoryZoneAllocate(Zone, CCReflectTypeSize(MappedType));
                                        memset(MappedData, 0, CCReflectTypeSize(MappedType));
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
                                        memset(MappedData, 0, CCReflectTypeSize(MappedType));
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
            if (Type == &CC_REFLECT(CCVector2D))
            {
                return CCExpressionCreateVector2(CC_STD_ALLOCATOR, *(const CCVector2D*)Data);
            }
            
            else if (Type == &CC_REFLECT(CCVector3D))
            {
                return CCExpressionCreateVector3(CC_STD_ALLOCATOR, *(const CCVector3D*)Data);
            }
            
            else if (Type == &CC_REFLECT(CCVector4D))
            {
                return CCExpressionCreateVector4(CC_STD_ALLOCATOR, *(const CCVector4D*)Data);
            }
            
            else if (Type == &CC_REFLECT(CCColourRGB))
            {
                return CCExpressionCreateVector3(CC_STD_ALLOCATOR, *(const CCColourRGB*)Data);
            }
            
            else if (Type == &CC_REFLECT(CCColourRGBA))
            {
                return CCExpressionCreateVector4(CC_STD_ALLOCATOR, *(const CCColourRGBA*)Data);
            }
            
            else if (Type == &CC_REFLECT(CCVector2Di))
            {
                return CCExpressionCreateVector2i(CC_STD_ALLOCATOR, *(const CCVector2Di*)Data);
            }
            
            else if (Type == &CC_REFLECT(CCVector3Di))
            {
                return CCExpressionCreateVector3i(CC_STD_ALLOCATOR, *(const CCVector3Di*)Data);
            }
            
            else if (Type == &CC_REFLECT(CCVector4Di))
            {
                return CCExpressionCreateVector4i(CC_STD_ALLOCATOR, *(const CCVector4Di*)Data);
            }
            
            else if (Type == &CC_REFLECT(CCColourRGBi))
            {
                return CCExpressionCreateVector3i(CC_STD_ALLOCATOR, *(const CCColourRGBi*)Data);
            }
            
            else if (Type == &CC_REFLECT(CCColourRGBAi))
            {
                return CCExpressionCreateVector4i(CC_STD_ALLOCATOR, *(const CCColourRGBAi*)Data);
            }
            
            else if (Type == &CC_REFLECT(CCRect))
            {
                return CCExpressionCreateRect(CC_STD_ALLOCATOR, *(const CCRect*)Data);
            }
            
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
            if (Type == &CC_REFLECT(void))
            {
                return CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":void"), TRUE);
            }
            
            else if (Type == &CC_REFLECT(CCString))
            {
                return *(CCString*)Data ? CCExpressionCreateString(CC_STD_ALLOCATOR, *(CCString*)Data, TRUE) : CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":null"), TRUE);
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
            
            CCExpression Result = CCExpressionReflectionOpaqueSerialize(Type, Data, Zone);
            
            if (!Result)
            {
                ((const CCReflectOpaque*)Type)->map(Type, Data, &(CCExpressionReflectionSerializeHandlerArgs){
                    .result = &Result,
                    .zone = Zone
                }, (CCReflectTypeHandler)CCExpressionReflectionSerializeHandler, Zone, CC_AUTORELEASE_ALLOCATOR(Zone), CCReflectMapIntentSerialize);
                
                CCAssertLog(Result, "Result should be valid expression");
            }
            
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
                            memset(Ptr, 0, Size);
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
                                memset(MappedData, 0, CCReflectTypeSize(MappedType));
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
            if (Type == &CC_REFLECT(CCVector2D))
            {
                if (CCExpressionIsVector2(Expression))
                {
                    *(CCVector2D*)Data = CCExpressionGetVector2(Expression);
                    
                    return;
                }
            }
            
            else if (Type == &CC_REFLECT(CCVector3D))
            {
                if (CCExpressionIsVector3(Expression))
                {
                    *(CCVector3D*)Data = CCExpressionGetVector3(Expression);
                    
                    return;
                }
            }
            
            else if (Type == &CC_REFLECT(CCVector4D))
            {
                if (CCExpressionIsVector4(Expression))
                {
                    *(CCVector4D*)Data = CCExpressionGetVector4(Expression);
                    
                    return;
                }
            }
            
            else if (Type == &CC_REFLECT(CCColourRGB))
            {
                if (CCExpressionIsColour(Expression))
                {
                    const CCColourRGBA Colour = CCExpressionGetColour(Expression);
                    ((CCColourRGB*)Data)->r = Colour.r;
                    ((CCColourRGB*)Data)->g = Colour.g;
                    ((CCColourRGB*)Data)->b = Colour.b;
                    
                    return;
                }
            }
            
            else if (Type == &CC_REFLECT(CCColourRGBA))
            {
                if (CCExpressionIsColour(Expression))
                {
                    *(CCColourRGBA*)Data = CCExpressionGetColour(Expression);
                    
                    return;
                }
            }
            
            else if (Type == &CC_REFLECT(CCVector2Di))
            {
                if (CCExpressionIsVector2i(Expression))
                {
                    *(CCVector2Di*)Data = CCExpressionGetVector2i(Expression);
                    
                    return;
                }
            }
            
            else if (Type == &CC_REFLECT(CCVector3Di))
            {
                if (CCExpressionIsVector3i(Expression))
                {
                    *(CCVector3Di*)Data = CCExpressionGetVector3i(Expression);
                    
                    return;
                }
            }
            
            else if (Type == &CC_REFLECT(CCVector4Di))
            {
                if (CCExpressionIsVector4i(Expression))
                {
                    *(CCVector4Di*)Data = CCExpressionGetVector4i(Expression);
                    
                    return;
                }
            }
            
            else if (Type == &CC_REFLECT(CCColourRGBi))
            {
                if (CCExpressionIsColour(Expression))
                {
                    const CCColourRGBA Colour = CCExpressionGetColour(Expression);
                    ((CCColourRGBi*)Data)->r = Colour.r * 255.0f;
                    ((CCColourRGBi*)Data)->g = Colour.g * 255.0f;
                    ((CCColourRGBi*)Data)->b = Colour.b * 255.0f;
                    
                    return;
                }
            }
            
            else if (Type == &CC_REFLECT(CCColourRGBAi))
            {
                if (CCExpressionIsColour(Expression))
                {
                    const CCColourRGBA Colour = CCExpressionGetColour(Expression);
                    ((CCColourRGBAi*)Data)->r = Colour.r * 255.0f;
                    ((CCColourRGBAi*)Data)->g = Colour.g * 255.0f;
                    ((CCColourRGBAi*)Data)->b = Colour.b * 255.0f;
                    ((CCColourRGBAi*)Data)->a = Colour.a * 255.0f;
                    
                    return;
                }
            }
            
            else if (Type == &CC_REFLECT(CCRect))
            {
                if (CCExpressionIsRect(Expression))
                {
                    *(CCRect*)Data = CCExpressionGetRect(Expression);
                    
                    return;
                }
            }
            
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
            if ((((const CCReflectOpaque*)Type)->map == CC_REFLECT(void).map) && (((const CCReflectOpaque*)Type)->unmap == CC_REFLECT(void).unmap))
            {
                if ((CCExpressionGetType(Expression) != CCExpressionValueTypeAtom) || (!CCStringEqual(CCExpressionGetAtom(Expression), CC_STRING(":void"))))
                {
                    CC_LOG_ERROR_CUSTOM("Deserializing malformed void. Expects :void.");
                }
            }
            
            else if ((((const CCReflectOpaque*)Type)->map == CC_REFLECT(CCString).map) && (((const CCReflectOpaque*)Type)->unmap == CC_REFLECT(CCString).unmap))
            {
                switch (CCExpressionGetType(Expression))
                {
                    case CCExpressionValueTypeString:
                        *(CCString*)Data = CCStringCopy(CCExpressionGetString(Expression));
                        return;
                        
                    case CCExpressionValueTypeAtom:
                        if (CCStringEqual(CCExpressionGetAtom(Expression), CC_STRING(":null")))
                        {
                            *(CCString*)Data = 0;
                        }
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
                                
                                memset(Key, 0, KeySize);
                                
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
            
            else if ((((const CCReflectOpaque*)Type)->map == CCReflectCCQueueMapper) && (((const CCReflectOpaque*)Type)->unmap == CCReflectCCQueueUnmapper))
            {
                if (CCExpressionGetType(Expression) == CCExpressionValueTypeAtom)
                {
                    if (CCStringEqual(CCExpressionGetAtom(Expression), CC_STRING(":null")))
                    {
                        *(CCQueue*)Data = NULL;
                        
                        return;
                    }
                }
                
                else if ((CCExpressionGetType(Expression) == CCExpressionValueTypeList) && (!CCExpressionReflectionListIsFallback(Expression)))
                {
                    CCAllocatorType CustomAllocator = ((const CCReflectCCLinkedList*)Type)->allocator;
                    if (CustomAllocator.allocator == CC_NULL_ALLOCATOR.allocator) CustomAllocator = Allocator;
                    
                    const size_t ElementSize = CCReflectTypeSize(((const CCReflectCCQueue*)Type)->elementType);
                    
                    CCQueue Queue = CCQueueCreate(CustomAllocator);
                    
                    CC_COLLECTION_FOREACH(CCExpression, Element, CCExpressionGetList(Expression))
                    {
                        CCQueueNode *Node = CCQueueCreateNode(CustomAllocator, ElementSize, NULL);
                        CCExpressionReflectionDeserialize(Element, ((const CCReflectCCQueue*)Type)->elementType, CCQueueGetNodeData(Node), Zone, Allocator);
                        
                        CCQueuePush(Queue, Node);
                    }
                    
                    *(CCQueue*)Data = Queue;
                    
                    return;
                }
            }
            
            else if (CCExpressionReflectionOpaqueDeserialize(Expression, Type, Data, Zone, Allocator)) return;
            
            if ((CCExpressionGetType(Expression) == CCExpressionValueTypeList) && (CCCollectionGetCount(CCExpressionGetList(Expression)) == 2))
            {
                CCExpression TypeExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0);
                CCExpression DataExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1);
                
                CCReflectType MappedType = CCExpressionReflectionDeserializeType(TypeExpr, Zone);
                void *MappedData = CCMemoryZoneAllocate(Zone, CCReflectTypeSize(MappedType));
                memset(MappedData, 0, CCReflectTypeSize(MappedType));
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
                const size_t Count = ((const CCReflectArray*)Type)->count;
                
                size_t Index = 0;
                CC_COLLECTION_FOREACH(CCExpression, Element, CCExpressionGetList(Expression))
                {
                    if (Index >= Count) break;
                    
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
                    memset(Element, 0, ElementSize);
                    
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

#include "Input.h"

CCExpressionReflectionSerializer CCExpressionReflectionOpaqueSerialize = CCExpressionReflectionOpaqueSerializeDefaults;
CCExpressionReflectionDeserializer CCExpressionReflectionOpaqueDeserialize = CCExpressionReflectionOpaqueDeserializeDefaults;

static CCString KeyboardKeycodeNames[CCKeyboardKeycodeCount] = {
    [CCKeyboardKeycodeSpace] = CC_STRING(":space"),
    [CCKeyboardKeycodeApostrophe] = CC_STRING(":apostrophe"),
    [CCKeyboardKeycodeComma] = CC_STRING(":comma"),
    [CCKeyboardKeycodeMinus] = CC_STRING(":minus"),
    [CCKeyboardKeycodePeriod] = CC_STRING(":period"),
    [CCKeyboardKeycodeSlash] = CC_STRING(":slash"),
    [CCKeyboardKeycode0] = CC_STRING(":0"),
    [CCKeyboardKeycode1] = CC_STRING(":1"),
    [CCKeyboardKeycode2] = CC_STRING(":2"),
    [CCKeyboardKeycode3] = CC_STRING(":3"),
    [CCKeyboardKeycode4] = CC_STRING(":4"),
    [CCKeyboardKeycode5] = CC_STRING(":5"),
    [CCKeyboardKeycode6] = CC_STRING(":6"),
    [CCKeyboardKeycode7] = CC_STRING(":7"),
    [CCKeyboardKeycode8] = CC_STRING(":8"),
    [CCKeyboardKeycode9] = CC_STRING(":9"),
    [CCKeyboardKeycodeSemicolon] = CC_STRING(":semicolon"),
    [CCKeyboardKeycodeEqual] = CC_STRING(":equal"),
    [CCKeyboardKeycodeA] = CC_STRING(":a"),
    [CCKeyboardKeycodeB] = CC_STRING(":b"),
    [CCKeyboardKeycodeC] = CC_STRING(":c"),
    [CCKeyboardKeycodeD] = CC_STRING(":d"),
    [CCKeyboardKeycodeE] = CC_STRING(":e"),
    [CCKeyboardKeycodeF] = CC_STRING(":f"),
    [CCKeyboardKeycodeG] = CC_STRING(":g"),
    [CCKeyboardKeycodeH] = CC_STRING(":h"),
    [CCKeyboardKeycodeI] = CC_STRING(":i"),
    [CCKeyboardKeycodeJ] = CC_STRING(":j"),
    [CCKeyboardKeycodeK] = CC_STRING(":k"),
    [CCKeyboardKeycodeL] = CC_STRING(":l"),
    [CCKeyboardKeycodeM] = CC_STRING(":m"),
    [CCKeyboardKeycodeN] = CC_STRING(":n"),
    [CCKeyboardKeycodeO] = CC_STRING(":o"),
    [CCKeyboardKeycodeP] = CC_STRING(":p"),
    [CCKeyboardKeycodeQ] = CC_STRING(":q"),
    [CCKeyboardKeycodeR] = CC_STRING(":r"),
    [CCKeyboardKeycodeS] = CC_STRING(":s"),
    [CCKeyboardKeycodeT] = CC_STRING(":t"),
    [CCKeyboardKeycodeU] = CC_STRING(":u"),
    [CCKeyboardKeycodeV] = CC_STRING(":v"),
    [CCKeyboardKeycodeW] = CC_STRING(":w"),
    [CCKeyboardKeycodeX] = CC_STRING(":x"),
    [CCKeyboardKeycodeY] = CC_STRING(":y"),
    [CCKeyboardKeycodeZ] = CC_STRING(":z"),
    [CCKeyboardKeycodeLeftBracket] = CC_STRING(":left-bracket"),
    [CCKeyboardKeycodeBackslash] = CC_STRING(":backslash"),
    [CCKeyboardKeycodeRightBracket] = CC_STRING(":right-bracket"),
    [CCKeyboardKeycodeGraveAccent] = CC_STRING(":grave-accent"),
    [CCKeyboardKeycodeWorld1] = CC_STRING(":world-1"),
    [CCKeyboardKeycodeWorld2] = CC_STRING(":world-2"),
    [CCKeyboardKeycodeEscape] = CC_STRING(":escape"),
    [CCKeyboardKeycodeEnter] = CC_STRING(":enter"),
    [CCKeyboardKeycodeTab] = CC_STRING(":tab"),
    [CCKeyboardKeycodeBackspace] = CC_STRING(":backspace"),
    [CCKeyboardKeycodeInsert] = CC_STRING(":insert"),
    [CCKeyboardKeycodeDelete] = CC_STRING(":delete"),
    [CCKeyboardKeycodeRight] = CC_STRING(":right"),
    [CCKeyboardKeycodeLeft] = CC_STRING(":left"),
    [CCKeyboardKeycodeDown] = CC_STRING(":down"),
    [CCKeyboardKeycodeUp] = CC_STRING(":up"),
    [CCKeyboardKeycodePageUp] = CC_STRING(":page-up"),
    [CCKeyboardKeycodePageDown] = CC_STRING(":page-down"),
    [CCKeyboardKeycodeHome] = CC_STRING(":home"),
    [CCKeyboardKeycodeEnd] = CC_STRING(":end"),
    [CCKeyboardKeycodeCapsLock] = CC_STRING(":caps-lock"),
    [CCKeyboardKeycodeScrollLock] = CC_STRING(":scroll-lock"),
    [CCKeyboardKeycodeNumLock] = CC_STRING(":num-lock"),
    [CCKeyboardKeycodePrintScreen] = CC_STRING(":print-screen"),
    [CCKeyboardKeycodePause] = CC_STRING(":pause"),
    [CCKeyboardKeycodeF1] = CC_STRING(":f1"),
    [CCKeyboardKeycodeF2] = CC_STRING(":f2"),
    [CCKeyboardKeycodeF3] = CC_STRING(":f3"),
    [CCKeyboardKeycodeF4] = CC_STRING(":f4"),
    [CCKeyboardKeycodeF5] = CC_STRING(":f5"),
    [CCKeyboardKeycodeF6] = CC_STRING(":f6"),
    [CCKeyboardKeycodeF7] = CC_STRING(":f7"),
    [CCKeyboardKeycodeF8] = CC_STRING(":f8"),
    [CCKeyboardKeycodeF9] = CC_STRING(":f9"),
    [CCKeyboardKeycodeF10] = CC_STRING(":f10"),
    [CCKeyboardKeycodeF11] = CC_STRING(":f11"),
    [CCKeyboardKeycodeF12] = CC_STRING(":f12"),
    [CCKeyboardKeycodeF13] = CC_STRING(":f13"),
    [CCKeyboardKeycodeF14] = CC_STRING(":f14"),
    [CCKeyboardKeycodeF15] = CC_STRING(":f15"),
    [CCKeyboardKeycodeF16] = CC_STRING(":f16"),
    [CCKeyboardKeycodeF17] = CC_STRING(":f17"),
    [CCKeyboardKeycodeF18] = CC_STRING(":f18"),
    [CCKeyboardKeycodeF19] = CC_STRING(":f19"),
    [CCKeyboardKeycodeF20] = CC_STRING(":f20"),
    [CCKeyboardKeycodeF21] = CC_STRING(":f21"),
    [CCKeyboardKeycodeF22] = CC_STRING(":f22"),
    [CCKeyboardKeycodeF23] = CC_STRING(":f23"),
    [CCKeyboardKeycodeF24] = CC_STRING(":f24"),
    [CCKeyboardKeycodeF25] = CC_STRING(":f25"),
    [CCKeyboardKeycodeKeypad0] = CC_STRING(":keypad-0"),
    [CCKeyboardKeycodeKeypad1] = CC_STRING(":keypad-1"),
    [CCKeyboardKeycodeKeypad2] = CC_STRING(":keypad-2"),
    [CCKeyboardKeycodeKeypad3] = CC_STRING(":keypad-3"),
    [CCKeyboardKeycodeKeypad4] = CC_STRING(":keypad-4"),
    [CCKeyboardKeycodeKeypad5] = CC_STRING(":keypad-5"),
    [CCKeyboardKeycodeKeypad6] = CC_STRING(":keypad-6"),
    [CCKeyboardKeycodeKeypad7] = CC_STRING(":keypad-7"),
    [CCKeyboardKeycodeKeypad8] = CC_STRING(":keypad-8"),
    [CCKeyboardKeycodeKeypad9] = CC_STRING(":keypad-9"),
    [CCKeyboardKeycodeKeypadDecimal] = CC_STRING(":keypad-decimal"),
    [CCKeyboardKeycodeKeypadDivide] = CC_STRING(":keypad-divide"),
    [CCKeyboardKeycodeKeypadMultiply] = CC_STRING(":keypad-multiply"),
    [CCKeyboardKeycodeKeypadSubtract] = CC_STRING(":keypad-subtract"),
    [CCKeyboardKeycodeKeypadAdd] = CC_STRING(":keypad-add"),
    [CCKeyboardKeycodeKeypadEnter] = CC_STRING(":keypad-enter"),
    [CCKeyboardKeycodeKeypadEqual] = CC_STRING(":keypad-equal"),
    [CCKeyboardKeycodeLeftShift] = CC_STRING(":left-shift"),
    [CCKeyboardKeycodeLeftControl] = CC_STRING(":left-control"),
    [CCKeyboardKeycodeLeftAlt] = CC_STRING(":left-alt"),
    [CCKeyboardKeycodeLeftSuper] = CC_STRING(":left-super"),
    [CCKeyboardKeycodeRightShift] = CC_STRING(":right-shift"),
    [CCKeyboardKeycodeRightControl] = CC_STRING(":right-control"),
    [CCKeyboardKeycodeRightAlt] = CC_STRING(":right-alt"),
    [CCKeyboardKeycodeRightSuper] = CC_STRING(":right-super"),
    [CCKeyboardKeycodeMenu] = CC_STRING(":menu")
};

static CCKeyboardKeycode CCExpressionReflectionGetKeyboardKeycode(CCString Name)
{
    static _Atomic(CCDictionary(CCString, CCKeyboardKeycode)) KeycodeLookup = NULL;
    static _Thread_local CCDictionary(CCString, CCKeyboardKeycode) Keycodes = NULL;
    
    if (!Keycodes)
    {
        CCDictionary Lookup = atomic_load_explicit(&KeycodeLookup, memory_order_acquire);
        
        if (!Lookup)
        {
            static atomic_flag Lock = ATOMIC_FLAG_INIT;
            while (!atomic_flag_test_and_set_explicit(&Lock, memory_order_relaxed)) CC_SPIN_WAIT();
            
            Lookup = atomic_load_explicit(&KeycodeLookup, memory_order_acquire);
            
            if (!Lookup)
            {
                Lookup = CCDictionaryCreate(CC_STD_ALLOCATOR, CCDictionaryHintHeavyFinding | CCDictionaryHintConstantLength | CCDictionaryHintConstantElements, sizeof(CCString), sizeof(CCKeyboardKeycode), &(CCDictionaryCallbacks){
                    .getHash = CCStringHasherForDictionary,
                    .compareKeys = CCStringComparatorForDictionary
                });
                
                for (size_t Loop = 0; Loop < CCKeyboardKeycodeCount; Loop++)
                {
                    if (KeyboardKeycodeNames[Loop]) CCDictionarySetValue(Lookup, &KeyboardKeycodeNames[Loop], &(CCKeyboardKeycode){ (CCKeyboardKeycode)Loop });
                }
                
                atomic_store_explicit(&KeycodeLookup, Lookup, memory_order_release);
            }
            
            atomic_flag_clear_explicit(&Lock, memory_order_relaxed);
        }
        
        Keycodes = Lookup;
    }
    
    const CCKeyboardKeycode *Keycode = CCDictionaryGetValue(Keycodes, &Name);
    
    return Keycode ? *Keycode : CCKeyboardKeycodeUnknown;
}

static CCString KeyboardModifierNames[] = {
    CC_STRING(":none"),
    [CCKeyboardModifierShift] = CC_STRING(":shift"),
    [CCKeyboardModifierControl] = CC_STRING(":control"),
    [CCKeyboardModifierAlt] = CC_STRING(":alt"),
    [CCKeyboardModifierSuper] = CC_STRING(":super"),
    [CCKeyboardModifierShift | CCKeyboardModifierControl] = CC_STRING(":control-shift"),
    [CCKeyboardModifierShift | CCKeyboardModifierAlt] = CC_STRING(":alt-shift"),
    [CCKeyboardModifierShift | CCKeyboardModifierSuper] = CC_STRING(":shift-super"),
    [CCKeyboardModifierControl | CCKeyboardModifierAlt] = CC_STRING(":control-alt"),
    [CCKeyboardModifierControl | CCKeyboardModifierSuper] = CC_STRING(":control-super"),
    [CCKeyboardModifierAlt | CCKeyboardModifierSuper] = CC_STRING(":alt-super"),
    [CCKeyboardModifierShift | CCKeyboardModifierControl | CCKeyboardModifierAlt] = CC_STRING(":control-alt-shift"),
    [CCKeyboardModifierShift | CCKeyboardModifierControl | CCKeyboardModifierSuper] = CC_STRING(":control-shift-super"),
    [CCKeyboardModifierShift | CCKeyboardModifierAlt | CCKeyboardModifierSuper] = CC_STRING(":alt-shift-super"),
    [CCKeyboardModifierControl | CCKeyboardModifierAlt | CCKeyboardModifierSuper] = CC_STRING(":control-alt-super"),
    [CCKeyboardModifierShift | CCKeyboardModifierControl | CCKeyboardModifierAlt | CCKeyboardModifierSuper] = CC_STRING(":control-alt-shift-super")
};

static CCKeyboardModifier CCExpressionReflectionGetKeyboardModifier(CCString Name)
{
    static _Atomic(CCDictionary(CCString, CCKeyboardModifier)) ModifierLookup = NULL;
    static _Thread_local CCDictionary(CCString, CCKeyboardModifier) Modifiers = NULL;
    
    if (!Modifiers)
    {
        CCDictionary Lookup = atomic_load_explicit(&ModifierLookup, memory_order_acquire);
        
        if (!Lookup)
        {
            static atomic_flag Lock = ATOMIC_FLAG_INIT;
            while (!atomic_flag_test_and_set_explicit(&Lock, memory_order_relaxed)) CC_SPIN_WAIT();
            
            Lookup = atomic_load_explicit(&ModifierLookup, memory_order_acquire);
            
            if (!Lookup)
            {
                Lookup = CCDictionaryCreate(CC_STD_ALLOCATOR, CCDictionaryHintHeavyFinding | CCDictionaryHintConstantLength | CCDictionaryHintConstantElements, sizeof(CCString), sizeof(CCKeyboardModifier), &(CCDictionaryCallbacks){
                    .getHash = CCStringHasherForDictionary,
                    .compareKeys = CCStringComparatorForDictionary
                });
                
                for (size_t Loop = 0; Loop < sizeof(KeyboardModifierNames) / sizeof(*KeyboardModifierNames); Loop++)
                {
                    if (KeyboardModifierNames[Loop]) CCDictionarySetValue(Lookup, &KeyboardModifierNames[Loop], &(CCKeyboardModifier){ (CCKeyboardModifier)Loop });
                }
                
                atomic_store_explicit(&ModifierLookup, Lookup, memory_order_release);
            }
            
            atomic_flag_clear_explicit(&Lock, memory_order_relaxed);
        }
        
        Modifiers = Lookup;
    }
    
    const CCKeyboardModifier *Modifier = CCDictionaryGetValue(Modifiers, &Name);
    
    return Modifier ? *Modifier : -1;
}

static CCString MouseButtonNames[CCMouseButtonCount] = {
    [CCMouseButton1] = CC_STRING(":btn-1"),
    [CCMouseButton2] = CC_STRING(":btn-2"),
    [CCMouseButton3] = CC_STRING(":btn-3"),
    [CCMouseButton4] = CC_STRING(":btn-4"),
    [CCMouseButton5] = CC_STRING(":btn-5"),
    [CCMouseButton6] = CC_STRING(":btn-6"),
    [CCMouseButton7] = CC_STRING(":btn-7"),
    [CCMouseButton8] = CC_STRING(":btn-8")
};

static CCString MouseButtonAliases[CCMouseButtonCount] = {
    [CCMouseButtonLeft] = CC_STRING(":left"),
    [CCMouseButtonRight] = CC_STRING(":right"),
    [CCMouseButtonMiddle] = CC_STRING(":middle")
};

static CCMouseButton CCExpressionReflectionGetMouseButton(CCString Name)
{
    static _Atomic(CCDictionary(CCString, CCMouseButton)) ButtonLookup = NULL;
    static _Thread_local CCDictionary(CCString, CCMouseButton) Buttons = NULL;
    
    if (!Buttons)
    {
        CCDictionary Lookup = atomic_load_explicit(&ButtonLookup, memory_order_acquire);
        
        if (!Lookup)
        {
            static atomic_flag Lock = ATOMIC_FLAG_INIT;
            while (!atomic_flag_test_and_set_explicit(&Lock, memory_order_relaxed)) CC_SPIN_WAIT();
            
            Lookup = atomic_load_explicit(&ButtonLookup, memory_order_acquire);
            
            if (!Lookup)
            {
                Lookup = CCDictionaryCreate(CC_STD_ALLOCATOR, CCDictionaryHintHeavyFinding | CCDictionaryHintConstantLength | CCDictionaryHintConstantElements, sizeof(CCString), sizeof(CCMouseButton), &(CCDictionaryCallbacks){
                    .getHash = CCStringHasherForDictionary,
                    .compareKeys = CCStringComparatorForDictionary
                });
                
                CCDictionarySetValue(Lookup, &MouseButtonAliases[CCMouseButtonLeft], &(CCMouseButton){ CCMouseButtonLeft });
                CCDictionarySetValue(Lookup, &MouseButtonAliases[CCMouseButtonRight], &(CCMouseButton){ CCMouseButtonRight });
                CCDictionarySetValue(Lookup, &MouseButtonAliases[CCMouseButtonMiddle], &(CCMouseButton){ CCMouseButtonMiddle });
                
                for (size_t Loop = 0; Loop < CCMouseButtonCount; Loop++)
                {
                    if (MouseButtonNames[Loop]) CCDictionarySetValue(Lookup, &MouseButtonNames[Loop], &(CCMouseButton){ (CCMouseButton)Loop });
                }
                
                atomic_store_explicit(&ButtonLookup, Lookup, memory_order_release);
            }
            
            atomic_flag_clear_explicit(&Lock, memory_order_relaxed);
        }
        
        Buttons = Lookup;
    }
    
    const CCMouseButton *Button = CCDictionaryGetValue(Buttons, &Name);
    
    return Button ? *Button : CCMouseButtonUnknown;
}

static CCString InputMapNames[CCInputMapTypeCount] = {
    [CCInputMapTypeNone] = CC_STRING("input-none"),
    [CCInputMapTypeGroup] = CC_STRING("input-group"),
    [CCInputMapTypeKeyboard] = CC_STRING("input-keyboard"),
    [CCInputMapTypeMouseButton] = CC_STRING("input-mouse-button"),
    [CCInputMapTypeControllerAxis] = CC_STRING("input-controller-axes"),
    [CCInputMapTypeControllerButton] = CC_STRING("input-controller-button"),
    [CCInputMapTypeControllerHat] = CC_STRING("input-controller-hat"),
    [CCInputMapTypeVirtualJoystick] = CC_STRING("input-virtual-joystick")
};

static CCInputMapType CCExpressionReflectionGetInputMapType(CCString Name)
{
    static _Atomic(CCDictionary(CCString, CCInputMapType)) TypeLookup = NULL;
    static _Thread_local CCDictionary(CCString, CCInputMapType) Types = NULL;
    
    if (!Types)
    {
        CCDictionary Lookup = atomic_load_explicit(&TypeLookup, memory_order_acquire);
        
        if (!Lookup)
        {
            static atomic_flag Lock = ATOMIC_FLAG_INIT;
            while (!atomic_flag_test_and_set_explicit(&Lock, memory_order_relaxed)) CC_SPIN_WAIT();
            
            Lookup = atomic_load_explicit(&TypeLookup, memory_order_acquire);
            
            if (!Lookup)
            {
                Lookup = CCDictionaryCreate(CC_STD_ALLOCATOR, CCDictionaryHintHeavyFinding | CCDictionaryHintConstantLength | CCDictionaryHintConstantElements, sizeof(CCString), sizeof(CCInputMapType), &(CCDictionaryCallbacks){
                    .getHash = CCStringHasherForDictionary,
                    .compareKeys = CCStringComparatorForDictionary
                });
                
                for (size_t Loop = 0; Loop < CCInputMapTypeCount; Loop++)
                {
                    if (InputMapNames[Loop]) CCDictionarySetValue(Lookup, &InputMapNames[Loop], &(CCInputMapType){ (CCInputMapType)Loop });
                }
                
                atomic_store_explicit(&TypeLookup, Lookup, memory_order_release);
            }
            
            atomic_flag_clear_explicit(&Lock, memory_order_relaxed);
        }
        
        Types = Lookup;
    }
    
    const CCInputMapType *Type = CCDictionaryGetValue(Types, &Name);
    
    return Type ? *Type : -1;
}

CCExpression CCExpressionReflectionOpaqueSerializeDefaults(CCReflectType Type, const void *Data, CCMemoryZone Zone)
{
    if (Type == &CC_REFLECT(CCInputMap))
    {
        CCString Name;
        
        if ((((const CCInputMap*)Data)->type < CCInputMapTypeCount) && (Name = InputMapNames[((const CCInputMap*)Data)->type]))
        {
            CCExpression Expr = CCExpressionCreateList(CC_STD_ALLOCATOR);
            
            CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, Name, TRUE) });
            
            switch (((const CCInputMap*)Data)->type)
            {
                case CCInputMapTypeGroup:
                {
                    CCExpression Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("all-active:"), TRUE) });
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateInteger(CC_STD_ALLOCATOR, ((const CCInputMap*)Data)->group.wantsAllActive) });
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
                    
                    Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("input:"), TRUE) });
                    
                    for (size_t Loop = 0; Loop < ((const CCInputMap*)Data)->group.count; Loop++)
                    {
                        CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionReflectionSerialize(&CC_REFLECT(CCInputMap), &((const CCInputMap*)Data)->group.inputMaps[Loop], Zone) });
                    }
                    
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
                    
                    break;
                }
                    
                case CCInputMapTypeKeyboard:
                {
                    if (((const CCInputMap*)Data)->keyboard.isKeycode)
                    {
                        CCExpression Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
                        CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("keycode:"), TRUE) });
                        
                        if ((((const CCInputMap*)Data)->keyboard.keycode < CCKeyboardKeycodeCount) && (KeyboardKeycodeNames[((const CCInputMap*)Data)->keyboard.keycode]))
                        {
                            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, KeyboardKeycodeNames[((const CCInputMap*)Data)->keyboard.keycode], TRUE) });
                        }
                        
                        else CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateInteger(CC_STD_ALLOCATOR, ((const CCInputMap*)Data)->keyboard.keycode) });
                        
                        CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
                    }
                    
                    if (((const CCInputMap*)Data)->keyboard.character)
                    {
                        CCExpression Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
                        CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("character:"), TRUE) });
                        CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateString(CC_STD_ALLOCATOR, CCStringCreateWithCharacter(CC_STD_ALLOCATOR, ((const CCInputMap*)Data)->keyboard.character), FALSE) });
                        CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
                    }
                    
                    if (!((const CCInputMap*)Data)->keyboard.ignoreModifier)
                    {
                        CCExpression Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
                        CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("flags:"), TRUE) });
                        
                        if ((((const CCInputMap*)Data)->keyboard.flags < sizeof(KeyboardModifierNames) / sizeof(*KeyboardModifierNames)) && (KeyboardModifierNames[((const CCInputMap*)Data)->keyboard.flags]))
                        {
                            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, KeyboardModifierNames[((const CCInputMap*)Data)->keyboard.flags], TRUE) });
                        }
                        
                        else CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateInteger(CC_STD_ALLOCATOR, ((const CCInputMap*)Data)->keyboard.flags) });
                        
                        CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
                    }
                    
                    CCExpression Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("ramp:"), TRUE) });
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateFloat(CC_STD_ALLOCATOR, ((const CCInputMap*)Data)->keyboard.ramp) });
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
                    
                    Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("repeat:"), TRUE) });
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateInteger(CC_STD_ALLOCATOR, ((const CCInputMap*)Data)->keyboard.repeats) });
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
                    
                    break;
                }
                    
                case CCInputMapTypeMouseButton:
                {
                    CCExpression Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("button:"), TRUE) });
                    
                    if ((((const CCInputMap*)Data)->mouse.button.code < CCMouseButtonCount) && (MouseButtonNames[((const CCInputMap*)Data)->mouse.button.code]))
                    {
                        CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, MouseButtonNames[((const CCInputMap*)Data)->mouse.button.code], TRUE) });
                    }
                    
                    else CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateInteger(CC_STD_ALLOCATOR, ((const CCInputMap*)Data)->mouse.button.code) });
                    
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
                    
                    if (!((const CCInputMap*)Data)->mouse.button.ignoreModifier)
                    {
                        CCExpression Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
                        CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("flags:"), TRUE) });
                        
                        if ((((const CCInputMap*)Data)->mouse.button.flags < sizeof(KeyboardModifierNames) / sizeof(*KeyboardModifierNames)) && (KeyboardModifierNames[((const CCInputMap*)Data)->mouse.button.flags]))
                        {
                            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, KeyboardModifierNames[((const CCInputMap*)Data)->mouse.button.flags], TRUE) });
                        }
                        
                        else CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateInteger(CC_STD_ALLOCATOR, ((const CCInputMap*)Data)->mouse.button.flags) });
                        
                        CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
                    }
                    
                    Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("ramp:"), TRUE) });
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateFloat(CC_STD_ALLOCATOR, ((const CCInputMap*)Data)->mouse.button.ramp) });
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
                    
                    break;
                }
                    
                case CCInputMapTypeControllerAxis:
                case CCInputMapTypeControllerButton:
                case CCInputMapTypeControllerHat:
                {
                    if (((const CCInputMap*)Data)->controller.device)
                    {
                        CCExpression Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
                        CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("device:"), TRUE) });
                        CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateString(CC_STD_ALLOCATOR, ((const CCInputMap*)Data)->controller.device, TRUE) });
                        CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
                    }
                    
                    CCExpression Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("connection:"), TRUE) });
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateInteger(CC_STD_ALLOCATOR, ((const CCInputMap*)Data)->controller.connection) });
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
                    
                    if (((const CCInputMap*)Data)->type == CCInputMapTypeControllerAxis)
                    {
                        Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
                        CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("deadzone:"), TRUE) });
                        CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateFloat(CC_STD_ALLOCATOR, ((const CCInputMap*)Data)->controller.axis.deadzone) });
                        CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
                        
                        Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
                        CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("resolution:"), TRUE) });
                        CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateInteger(CC_STD_ALLOCATOR, ((const CCInputMap*)Data)->controller.axis.resolution) });
                        CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
                        
                        const CCString AxesName[4] = {
                            CC_STRING("x:"),
                            CC_STRING("y:"),
                            CC_STRING("z:"),
                            CC_STRING("w:")
                        };
                        
                        for (size_t Loop = 0; Loop < 4; Loop++)
                        {
                            const CCControllerAxisMap *Axis = &((const CCInputMap*)Data)->controller.axis.xyzw[Loop];
                            
                            if (Axis->index >= 0)
                            {
                                Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
                                CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, AxesName[Loop], TRUE) });
                                CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateInteger(CC_STD_ALLOCATOR, Axis->index) });
                                
                                if (Axis->flip) CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":flip"), TRUE) });
                                
                                if (Axis->oneSided) CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":one-sided"), TRUE) });
                                
                                CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
                            }
                        }
                    }
                    
                    else
                    {
                        const struct {
                            const typeof(((const CCInputMap*)Data)->controller.button) *map;
                            CCString name;
                        } Input[2] = {
                            { &((const CCInputMap*)Data)->controller.button, CC_STRING("button:") },
                            { &((const CCInputMap*)Data)->controller.hat, CC_STRING("hat:") }
                        };
                        
                        const size_t Index = ((const CCInputMap*)Data)->type == CCInputMapTypeControllerHat;
                        
                        Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
                        CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, Input[Index].name, TRUE) });
                        CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateInteger(CC_STD_ALLOCATOR, Input[Index].map->index) });
                        CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
                        
                        Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
                        CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("ramp:"), TRUE) });
                        CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateInteger(CC_STD_ALLOCATOR, Input[Index].map->ramp) });
                        CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
                    }
                    
                    break;
                }
                    
                case CCInputMapTypeVirtualJoystick:
                {
                    CCExpression Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
                    
                    switch (((const CCInputMap*)Data)->virtual.joystick.count)
                    {
                        case 8:
                            CCOrderedCollectionPrependElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionReflectionSerialize(&CC_REFLECT(CCInputMap), &((const CCInputMap*)Data)->virtual.joystick.inputMaps[7], Zone) });
                        case 7:
                            CCOrderedCollectionPrependElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionReflectionSerialize(&CC_REFLECT(CCInputMap), &((const CCInputMap*)Data)->virtual.joystick.inputMaps[6], Zone) });
                            CCOrderedCollectionPrependElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("w:"), TRUE) });
                            CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
                            
                            Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
                        case 6:
                            CCOrderedCollectionPrependElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionReflectionSerialize(&CC_REFLECT(CCInputMap), &((const CCInputMap*)Data)->virtual.joystick.inputMaps[5], Zone) });
                        case 5:
                            CCOrderedCollectionPrependElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionReflectionSerialize(&CC_REFLECT(CCInputMap), &((const CCInputMap*)Data)->virtual.joystick.inputMaps[4], Zone) });
                            CCOrderedCollectionPrependElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("z:"), TRUE) });
                            CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
                            
                            Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
                        case 4:
                            CCOrderedCollectionPrependElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionReflectionSerialize(&CC_REFLECT(CCInputMap), &((const CCInputMap*)Data)->virtual.joystick.inputMaps[3], Zone) });
                        case 3:
                            CCOrderedCollectionPrependElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionReflectionSerialize(&CC_REFLECT(CCInputMap), &((const CCInputMap*)Data)->virtual.joystick.inputMaps[2], Zone) });
                            CCOrderedCollectionPrependElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("y:"), TRUE) });
                            CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
                            
                            Field = CCExpressionCreateList(CC_STD_ALLOCATOR);
                        case 2:
                            CCOrderedCollectionPrependElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionReflectionSerialize(&CC_REFLECT(CCInputMap), &((const CCInputMap*)Data)->virtual.joystick.inputMaps[1], Zone) });
                        case 1:
                            CCOrderedCollectionPrependElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionReflectionSerialize(&CC_REFLECT(CCInputMap), &((const CCInputMap*)Data)->virtual.joystick.inputMaps[0], Zone) });
                            CCOrderedCollectionPrependElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("x:"), TRUE) });
                            CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
                            break;
                            
                        default:
                            CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING("input:"), TRUE) });
                            
                            for (size_t Loop = 0; Loop < ((const CCInputMap*)Data)->group.count; Loop++)
                            {
                                CCOrderedCollectionAppendElement(CCExpressionGetList(Field), &(CCExpression){ CCExpressionReflectionSerialize(&CC_REFLECT(CCInputMap), &((const CCInputMap*)Data)->group.inputMaps[Loop], Zone) });
                            }
                            
                            CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Field);
                            break;
                    }
                    
                    break;
                }
                    
                default:
                    break;
            }
            
            return Expr;
        }
    }
    
    return NULL;
}

static void CCExpressionReflectionDeserializeControllerAxisArgs(CCString Name, CCExpression Expr, size_t Count, void *Data)
{
    if (CCStringEqual(Name, CC_STRING("deadzone:")))
    {
        CCExpression DeadzoneExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expr), 1);
        
        CCExpressionReflectionDeserializeFloat(&CC_REFLECT(float), DeadzoneExpr, &((CCInputMap*)Data)->controller.axis.deadzone);
    }
    
    else if (CCStringEqual(Name, CC_STRING("resolution:")))
    {
        CCExpression ResolutionExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expr), 1);
        
        if (CCExpressionGetType(ResolutionExpr) == CCExpressionValueTypeInteger)
        {
            ((CCInputMap*)Data)->controller.axis.resolution = CCExpressionGetInteger(ResolutionExpr);
        }
        
        else CC_LOG_ERROR_CUSTOM("Deserializing malformed resolution. Expects integer.");
    }
    
    else
    {
        const CCString AxesName[4] = {
            CC_STRING("x:"),
            CC_STRING("y:"),
            CC_STRING("z:"),
            CC_STRING("w:")
        };
        
        for (size_t Loop = 0; Loop < 4; Loop++)
        {
            if (CCStringEqual(Name, AxesName[Loop]))
            {
                CCControllerAxisMap *Axis = &((CCInputMap*)Data)->controller.axis.xyzw[Loop];
                
                Axis->index = -1;
                Axis->flip = FALSE;
                Axis->oneSided = FALSE;
                
                CCExpression IndexExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expr), 1);
                
                if (CCExpressionGetType(IndexExpr) == CCExpressionValueTypeInteger)
                {
                    Axis->index = CCExpressionGetInteger(IndexExpr);
                }
                
                else CC_LOG_ERROR_CUSTOM("Deserializing malformed axis identifier. Expects integer.");
                
                for (size_t Loop = 2; Loop < Count; Loop++)
                {
                    CCExpression ArgExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expr), Loop);
                    
                    if (CCExpressionGetType(ArgExpr) == CCExpressionValueTypeAtom)
                    {
                        CCString ArgName = CCExpressionGetAtom(ArgExpr);
                        
                        if (CCStringEqual(ArgName, CC_STRING(":flip")))
                        {
                            Axis->flip = TRUE;
                        }
                        
                        else if (CCStringEqual(ArgName, CC_STRING(":one-sided")))
                        {
                            Axis->oneSided = TRUE;
                        }
                        
                        else CC_LOG_ERROR_CUSTOM("Deserializing malformed argument. Expects :flip or :one-sided got %S.", ArgName);
                    }
                    
                    else CC_LOG_ERROR_CUSTOM("Deserializing malformed argument. Expects atom.");
                }
                
                break;
            }
        }
    }
}

static void CCExpressionReflectionDeserializeControllerButtonArgs(CCString Name, CCExpression Expr, size_t Count, void *Data)
{
    if (CCStringEqual(Name, CC_STRING("button:")))
    {
        CCExpression IndexExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expr), 1);
        
        if (CCExpressionGetType(IndexExpr) == CCExpressionValueTypeInteger)
        {
            ((CCInputMap*)Data)->controller.button.index = CCExpressionGetInteger(IndexExpr);
        }
        
        else CC_LOG_ERROR_CUSTOM("Deserializing malformed identifier. Expects integer.");
    }
    
    else if (CCStringEqual(Name, CC_STRING("ramp:")))
    {
        CCExpression RampExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expr), 1);
        
        CCExpressionReflectionDeserializeFloat(&CC_REFLECT(float), RampExpr, &((CCInputMap*)Data)->controller.button.ramp);
    }
}

static void CCExpressionReflectionDeserializeControllerHatArgs(CCString Name, CCExpression Expr, size_t Count, void *Data)
{
    if (CCStringEqual(Name, CC_STRING("hat:")))
    {
        CCExpression IndexExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expr), 1);
        
        if (CCExpressionGetType(IndexExpr) == CCExpressionValueTypeInteger)
        {
            ((CCInputMap*)Data)->controller.hat.index = CCExpressionGetInteger(IndexExpr);
        }
        
        else CC_LOG_ERROR_CUSTOM("Deserializing malformed identifier. Expects integer.");
    }
    
    else if (CCStringEqual(Name, CC_STRING("ramp:")))
    {
        CCExpression RampExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expr), 1);
        
        CCExpressionReflectionDeserializeFloat(&CC_REFLECT(float), RampExpr, &((CCInputMap*)Data)->controller.hat.ramp);
    }
}

_Bool CCExpressionReflectionOpaqueDeserializeDefaults(CCExpression Expression, CCReflectType Type, void *Data, CCMemoryZone Zone, CCAllocatorType Allocator)
{
    if ((Type == &CC_REFLECT(CCInputMap)) || ((((const CCReflectOpaque*)Type)->map == CC_REFLECT(CCInputMap).map) && (((const CCReflectOpaque*)Type)->unmap == CC_REFLECT(CCInputMap).unmap)))
    {
        if (CCExpressionGetType(Expression) == CCExpressionValueTypeList)
        {
            CCEnumerator Enumerator;
            CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
            
            CCExpression *Expr = CCCollectionEnumeratorGetCurrent(&Enumerator);
            
            if ((Expr) && (CCExpressionGetType(*Expr) == CCExpressionValueTypeAtom))
            {
                const CCInputMapType InputMapType = CCExpressionReflectionGetInputMapType(CCExpressionGetAtom(*Expr));
                
                switch (InputMapType)
                {
                    case CCInputMapTypeNone:
                        break;
                        
                    case CCInputMapTypeGroup:
                        ((CCInputMap*)Data)->group.wantsAllActive = FALSE;
                        ((CCInputMap*)Data)->group.inputMaps = NULL;
                        ((CCInputMap*)Data)->group.count = 0;
                        
                        while ((Expr = CCCollectionEnumeratorNext(&Enumerator)))
                        {
                            if (CCExpressionGetType(*Expr) == CCExpressionValueTypeList)
                            {
                                const size_t Count = CCCollectionGetCount(CCExpressionGetList(*Expr));
                                
                                if (Count >= 2)
                                {
                                    CCExpression NameExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(*Expr), 0);
                                    
                                    if (CCExpressionGetType(NameExpr) == CCExpressionValueTypeAtom)
                                    {
                                        CCString Name = CCExpressionGetAtom(NameExpr);
                                        
                                        if (CCStringEqual(Name, CC_STRING("input:")))
                                        {
                                            CCOrderedCollection List = CCExpressionGetList(*Expr);
                                            size_t ItemCount = Count - 1, Start = 1;
                                            
                                            if (Count == 2)
                                            {
                                                CCExpression ArgExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(List, 1);
                                                
                                                if (CCExpressionGetType(ArgExpr) == CCExpressionValueTypeList)
                                                {
                                                    const size_t InnerCount = CCCollectionGetCount(CCExpressionGetList(ArgExpr));
                                                    
                                                    if (InnerCount)
                                                    {
                                                        if (CCExpressionGetType(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(ArgExpr), 0)) == CCExpressionValueTypeList)
                                                        {
                                                            List = CCExpressionGetList(ArgExpr);
                                                            Start = 0;
                                                            ItemCount = InnerCount;
                                                        }
                                                    }
                                                    
                                                    else ItemCount = 0;
                                                }
                                            }
                                            
                                            if (ItemCount)
                                            {
                                                ((CCInputMap*)Data)->group.inputMaps = CCMalloc(CC_STD_ALLOCATOR, sizeof(CCInputMap) * ItemCount, NULL, CC_DEFAULT_ERROR_CALLBACK);
                                                
                                                for (size_t Loop = 0; Loop < ItemCount; Loop++)
                                                {
                                                    ((CCInputMap*)Data)->group.inputMaps[Loop].type = CCInputMapTypeNone;
                                                    
                                                    CCExpressionReflectionDeserialize(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(List, Loop + Start), &CC_REFLECT(CCInputMap), &((CCInputMap*)Data)->group.inputMaps[Loop], Zone, Allocator);
                                                }
                                                
                                                ((CCInputMap*)Data)->group.count = ItemCount;
                                            }
                                        }
                                        
                                        else if (CCStringEqual(Name, CC_STRING("all-active:")))
                                        {
                                            CCExpression ActiveExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(*Expr), 1);
                                            
                                            if (CCExpressionGetType(ActiveExpr) == CCExpressionValueTypeInteger)
                                            {
                                                ((CCInputMap*)Data)->group.wantsAllActive = CCExpressionGetInteger(ActiveExpr);
                                            }
                                            
                                            else CC_LOG_ERROR_CUSTOM("Deserializing malformed all-active. Expects boolean.");
                                        }
                                    }
                                }
                            }
                        }
                        break;
                        
                    case CCInputMapTypeKeyboard:
                        ((CCInputMap*)Data)->keyboard.character = 0;
                        ((CCInputMap*)Data)->keyboard.ramp = 0.0f;
                        ((CCInputMap*)Data)->keyboard.isKeycode = FALSE;
                        ((CCInputMap*)Data)->keyboard.ignoreModifier = TRUE;
                        ((CCInputMap*)Data)->keyboard.repeats = FALSE;
                        
                        while ((Expr = CCCollectionEnumeratorNext(&Enumerator)))
                        {
                            if (CCExpressionGetType(*Expr) == CCExpressionValueTypeList)
                            {
                                const size_t Count = CCCollectionGetCount(CCExpressionGetList(*Expr));
                                
                                if (Count >= 2)
                                {
                                    CCExpression NameExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(*Expr), 0);
                                    
                                    if (CCExpressionGetType(NameExpr) == CCExpressionValueTypeAtom)
                                    {
                                        CCString Name = CCExpressionGetAtom(NameExpr);
                                        
                                        if (CCStringEqual(Name, CC_STRING("keycode:")))
                                        {
                                            CCExpression KeycodeExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(*Expr), 1);
                                            
                                            if (CCExpressionGetType(KeycodeExpr) == CCExpressionValueTypeAtom)
                                            {
                                                const CCKeyboardKeycode Keycode = CCExpressionReflectionGetKeyboardKeycode(CCExpressionGetAtom(KeycodeExpr));
                                                
                                                if (Keycode != CCKeyboardKeycodeUnknown)
                                                {
                                                    ((CCInputMap*)Data)->keyboard.keycode = Keycode;
                                                    ((CCInputMap*)Data)->keyboard.isKeycode = TRUE;
                                                }
                                                
                                                else CC_LOG_ERROR_CUSTOM("Deserializing malformed keycode. Expects keycode got %S.", CCExpressionGetAtom(KeycodeExpr));
                                            }
                                            
                                            else if (CCExpressionGetType(KeycodeExpr) == CCExpressionValueTypeInteger)
                                            {
                                                const CCKeyboardKeycode Keycode = CCExpressionGetInteger(KeycodeExpr);
                                                
                                                if ((Keycode >= 0) && (Keycode < CCKeyboardKeycodeCount))
                                                {
                                                    ((CCInputMap*)Data)->keyboard.keycode = Keycode;
                                                    ((CCInputMap*)Data)->keyboard.isKeycode = TRUE;
                                                }
                                                
                                                else CC_LOG_ERROR_CUSTOM("Deserializing malformed keycode. Expects keycode got %S.", CCExpressionGetInteger(KeycodeExpr));
                                            }
                                            
                                            else CC_LOG_ERROR_CUSTOM("Deserializing malformed keycode. Expects atom or integer.");
                                        }
                                        
                                        else if (CCStringEqual(Name, CC_STRING("character:")))
                                        {
                                            CCExpression CharacterExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(*Expr), 1);
                                            
                                            if (CCExpressionGetType(CharacterExpr) == CCExpressionValueTypeString)
                                            {
                                                CCString Character = CCExpressionGetString(CharacterExpr);
                                                
                                                ((CCInputMap*)Data)->keyboard.character = CCStringGetLength(Character) ? CCStringGetCharacterAtIndex(Character, 0) : 0;
                                            }
                                            
                                            else CC_LOG_ERROR_CUSTOM("Deserializing malformed character. Expects string.");
                                        }
                                        
                                        else if (CCStringEqual(Name, CC_STRING("flags:")))
                                        {
                                            CCExpression FlagsExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(*Expr), 1);
                                            
                                            if (CCExpressionGetType(FlagsExpr) == CCExpressionValueTypeAtom)
                                            {
                                                const CCKeyboardModifier Flags = CCExpressionReflectionGetKeyboardModifier(CCExpressionGetAtom(FlagsExpr));
                                                
                                                if (Flags != -1)
                                                {
                                                    ((CCInputMap*)Data)->keyboard.flags = Flags;
                                                    ((CCInputMap*)Data)->keyboard.ignoreModifier = FALSE;
                                                }
                                                
                                                else CC_LOG_ERROR_CUSTOM("Deserializing malformed flags. Expects flags got %S.", CCExpressionGetAtom(FlagsExpr));
                                            }
                                            
                                            else if (CCExpressionGetType(FlagsExpr) == CCExpressionValueTypeInteger)
                                            {
                                                const CCKeyboardModifier Flags = CCExpressionGetInteger(FlagsExpr);
                                                
                                                ((CCInputMap*)Data)->keyboard.flags = Flags;
                                                ((CCInputMap*)Data)->keyboard.ignoreModifier = FALSE;
                                            }
                                            
                                            else CC_LOG_ERROR_CUSTOM("Deserializing malformed flags. Expects atom or integer.");
                                        }
                                        
                                        else if (CCStringEqual(Name, CC_STRING("ramp:")))
                                        {
                                            CCExpression RampExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(*Expr), 1);
                                            
                                            CCExpressionReflectionDeserializeFloat(&CC_REFLECT(float), RampExpr, &((CCInputMap*)Data)->keyboard.ramp);
                                        }
                                        
                                        else if (CCStringEqual(Name, CC_STRING("repeat:")))
                                        {
                                            CCExpression RepeatExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(*Expr), 1);
                                            
                                            if (CCExpressionGetType(RepeatExpr) == CCExpressionValueTypeInteger)
                                            {
                                                ((CCInputMap*)Data)->keyboard.repeats = CCExpressionGetInteger(RepeatExpr);
                                            }
                                            
                                            else CC_LOG_ERROR_CUSTOM("Deserializing malformed repeat. Expects boolean.");
                                        }
                                    }
                                }
                            }
                        }
                        break;
                        
                    case CCInputMapTypeMouseButton:
                        ((CCInputMap*)Data)->mouse.button.ramp = 0.0f;
                        ((CCInputMap*)Data)->mouse.button.code = CCMouseButtonUnknown;
                        ((CCInputMap*)Data)->mouse.button.ignoreModifier = TRUE;
                        
                        while ((Expr = CCCollectionEnumeratorNext(&Enumerator)))
                        {
                            if (CCExpressionGetType(*Expr) == CCExpressionValueTypeList)
                            {
                                const size_t Count = CCCollectionGetCount(CCExpressionGetList(*Expr));
                                
                                if (Count >= 2)
                                {
                                    CCExpression NameExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(*Expr), 0);
                                    
                                    if (CCExpressionGetType(NameExpr) == CCExpressionValueTypeAtom)
                                    {
                                        CCString Name = CCExpressionGetAtom(NameExpr);
                                        
                                        if (CCStringEqual(Name, CC_STRING("button:")))
                                        {
                                            CCExpression ButtonExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(*Expr), 1);
                                            
                                            if (CCExpressionGetType(ButtonExpr) == CCExpressionValueTypeAtom)
                                            {
                                                const CCMouseButton Button = CCExpressionReflectionGetMouseButton(CCExpressionGetAtom(ButtonExpr));
                                                
                                                if (Button != CCMouseButtonUnknown)
                                                {
                                                    ((CCInputMap*)Data)->mouse.button.code = Button;
                                                }
                                                
                                                else CC_LOG_ERROR_CUSTOM("Deserializing malformed button. Expects button got %S.", CCExpressionGetAtom(ButtonExpr));
                                            }
                                            
                                            else if (CCExpressionGetType(ButtonExpr) == CCExpressionValueTypeInteger)
                                            {
                                                const CCMouseButton Button = CCExpressionGetInteger(ButtonExpr);
                                                
                                                if ((Button >= 0) && (Button < CCMouseButtonCount))
                                                {
                                                    ((CCInputMap*)Data)->mouse.button.code = Button;
                                                }
                                                
                                                else CC_LOG_ERROR_CUSTOM("Deserializing malformed button. Expects button got %S.", CCExpressionGetInteger(ButtonExpr));
                                            }
                                            
                                            else CC_LOG_ERROR_CUSTOM("Deserializing malformed button. Expects atom or integer.");
                                        }
                                        
                                        else if (CCStringEqual(Name, CC_STRING("flags:")))
                                        {
                                            CCExpression FlagsExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(*Expr), 1);
                                            
                                            if (CCExpressionGetType(FlagsExpr) == CCExpressionValueTypeAtom)
                                            {
                                                const CCKeyboardModifier Flags = CCExpressionReflectionGetKeyboardModifier(CCExpressionGetAtom(FlagsExpr));
                                                
                                                if (Flags != -1)
                                                {
                                                    ((CCInputMap*)Data)->mouse.button.flags = Flags;
                                                    ((CCInputMap*)Data)->mouse.button.ignoreModifier = FALSE;
                                                }
                                                
                                                else CC_LOG_ERROR_CUSTOM("Deserializing malformed flags. Expects flags got %S.", CCExpressionGetAtom(FlagsExpr));
                                            }
                                            
                                            else if (CCExpressionGetType(FlagsExpr) == CCExpressionValueTypeInteger)
                                            {
                                                const CCKeyboardModifier Flags = CCExpressionGetInteger(FlagsExpr);
                                                
                                                ((CCInputMap*)Data)->mouse.button.flags = Flags;
                                                ((CCInputMap*)Data)->mouse.button.ignoreModifier = FALSE;
                                            }
                                            
                                            else CC_LOG_ERROR_CUSTOM("Deserializing malformed flags. Expects atom or integer.");
                                        }
                                        
                                        else if (CCStringEqual(Name, CC_STRING("ramp:")))
                                        {
                                            CCExpression RampExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(*Expr), 1);
                                            
                                            CCExpressionReflectionDeserializeFloat(&CC_REFLECT(float), RampExpr, &((CCInputMap*)Data)->mouse.button.ramp);
                                        }
                                    }
                                }
                            }
                        }
                        break;
                        
                    case CCInputMapTypeControllerAxis:
                        ((CCInputMap*)Data)->controller.axis.x.index = -1;
                        ((CCInputMap*)Data)->controller.axis.y.index = -1;
                        ((CCInputMap*)Data)->controller.axis.z.index = -1;
                        ((CCInputMap*)Data)->controller.axis.w.index = -1;
                        goto DeserializeController;
                    case CCInputMapTypeControllerButton:
                        ((CCInputMap*)Data)->controller.button.index = -1;
                        ((CCInputMap*)Data)->controller.hat.ramp = 0.0f;
                        goto DeserializeController;
                    case CCInputMapTypeControllerHat:
                        ((CCInputMap*)Data)->controller.hat.index = -1;
                        ((CCInputMap*)Data)->controller.hat.ramp = 0.0f;
                        
                    DeserializeController:;
                        _Static_assert((CCInputMapTypeControllerAxis + 1 == CCInputMapTypeControllerButton) && (CCInputMapTypeControllerAxis + 2 == CCInputMapTypeControllerHat), "Type ordering changed");
                        
                        void (*DeserializeControllerCustomArgs)(CCString, CCExpression, size_t, void *) = (void(*[3])(CCString,CCExpression,size_t,void*)){
                            CCExpressionReflectionDeserializeControllerAxisArgs,
                            CCExpressionReflectionDeserializeControllerButtonArgs,
                            CCExpressionReflectionDeserializeControllerHatArgs
                        }[InputMapType - CCInputMapTypeControllerAxis];
                        
                        ((CCInputMap*)Data)->controller.device = 0;
                        ((CCInputMap*)Data)->controller.connection = -1;
                        
                        while ((Expr = CCCollectionEnumeratorNext(&Enumerator)))
                        {
                            if (CCExpressionGetType(*Expr) == CCExpressionValueTypeList)
                            {
                                const size_t Count = CCCollectionGetCount(CCExpressionGetList(*Expr));
                                
                                if (Count >= 2)
                                {
                                    CCExpression NameExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(*Expr), 0);
                                    
                                    if (CCExpressionGetType(NameExpr) == CCExpressionValueTypeAtom)
                                    {
                                        CCString Name = CCExpressionGetAtom(NameExpr);
                                        
                                        if (CCStringEqual(Name, CC_STRING("device:")))
                                        {
                                            CCExpression DeviceExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(*Expr), 1);
                                            
                                            if (CCExpressionGetType(DeviceExpr) == CCExpressionValueTypeString)
                                            {
                                                CCString Device = CCExpressionGetString(DeviceExpr);
                                                
                                                if (((CCInputMap*)Data)->controller.device) CCStringDestroy(((CCInputMap*)Data)->controller.device);
                                                
                                                ((CCInputMap*)Data)->controller.device = CCStringCopy(Device);
                                            }
                                            
                                            else CC_LOG_ERROR_CUSTOM("Deserializing malformed device. Expects string.");
                                        }
                                        
                                        else if (CCStringEqual(Name, CC_STRING("connection:")))
                                        {
                                            CCExpression ConnectionExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(*Expr), 1);
                                            
                                            if (CCExpressionGetType(ConnectionExpr) == CCExpressionValueTypeInteger)
                                            {
                                                ((CCInputMap*)Data)->controller.connection = CCExpressionGetInteger(ConnectionExpr);
                                            }
                                            
                                            else CC_LOG_ERROR_CUSTOM("Deserializing malformed connection. Expects integer.");
                                        }
                                        
                                        else DeserializeControllerCustomArgs(Name, *Expr, Count, Data);
                                    }
                                }
                            }
                        }
                        break;
                        
                    case CCInputMapTypeVirtualJoystick:
                        ((CCInputMap*)Data)->virtual.joystick.inputMaps = NULL;
                        ((CCInputMap*)Data)->virtual.joystick.count = 0;
                        
                        while ((Expr = CCCollectionEnumeratorNext(&Enumerator)))
                        {
                            if (CCExpressionGetType(*Expr) == CCExpressionValueTypeList)
                            {
                                const size_t Count = CCCollectionGetCount(CCExpressionGetList(*Expr));
                                
                                if (Count >= 2)
                                {
                                    CCExpression NameExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(*Expr), 0);
                                    
                                    if (CCExpressionGetType(NameExpr) == CCExpressionValueTypeAtom)
                                    {
                                        CCString Name = CCExpressionGetAtom(NameExpr);
                                        
                                        if (CCStringEqual(Name, CC_STRING("input:")))
                                        {
                                            if (((CCInputMap*)Data)->virtual.joystick.inputMaps)
                                            {
                                                for (size_t Loop = 0; Loop < ((CCInputMap*)Data)->virtual.joystick.count; Loop++)
                                                {
                                                    CCInputMapDestructor(&((CCInputMap*)Data)->virtual.joystick.inputMaps[Loop]);
                                                }
                                                
                                                CCFree(((CCInputMap*)Data)->virtual.joystick.inputMaps);
                                            }
                                            
                                            CCOrderedCollection List = CCExpressionGetList(*Expr);
                                            size_t ItemCount = Count - 1, Start = 1;
                                            
                                            if (Count == 2)
                                            {
                                                CCExpression ArgExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(List, 1);
                                                
                                                if (CCExpressionGetType(ArgExpr) == CCExpressionValueTypeList)
                                                {
                                                    const size_t InnerCount = CCCollectionGetCount(CCExpressionGetList(ArgExpr));
                                                    
                                                    if (InnerCount)
                                                    {
                                                        if (CCExpressionGetType(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(ArgExpr), 0)) == CCExpressionValueTypeList)
                                                        {
                                                            List = CCExpressionGetList(ArgExpr);
                                                            Start = 0;
                                                            ItemCount = InnerCount;
                                                        }
                                                    }
                                                    
                                                    else ItemCount = 0;
                                                }
                                            }
                                            
                                            if (ItemCount)
                                            {
                                                ((CCInputMap*)Data)->virtual.joystick.inputMaps = CCMalloc(CC_STD_ALLOCATOR, sizeof(CCInputMap) * CCMax(ItemCount -1, 8), NULL, CC_DEFAULT_ERROR_CALLBACK);
                                                
                                                for (size_t Loop = 0; Loop < ItemCount; Loop++)
                                                {
                                                    ((CCInputMap*)Data)->virtual.joystick.inputMaps[Loop].type = CCInputMapTypeNone;
                                                    
                                                    CCExpressionReflectionDeserialize(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(List, Loop + Start), &CC_REFLECT(CCInputMap), &((CCInputMap*)Data)->virtual.joystick.inputMaps[Loop], Zone, Allocator);
                                                }
                                                
                                                for (size_t Loop = ItemCount, Max = CCMax(ItemCount, 8); Loop < Max; Loop++) ((CCInputMap*)Data)->virtual.joystick.inputMaps[Loop].type = CCInputMapTypeNone;
                                                
                                                ((CCInputMap*)Data)->virtual.joystick.count = ItemCount;
                                            }
                                        }
                                        
                                        else
                                        {
                                            const CCString AxesName[4] = {
                                                CC_STRING("x:"),
                                                CC_STRING("y:"),
                                                CC_STRING("z:"),
                                                CC_STRING("w:")
                                            };
                                            
                                            for (size_t Loop = 0; Loop < 4; Loop++)
                                            {
                                                if (CCStringEqual(Name, AxesName[Loop]))
                                                {
                                                    if (!((CCInputMap*)Data)->virtual.joystick.inputMaps)
                                                    {
                                                        ((CCInputMap*)Data)->virtual.joystick.inputMaps = CCMalloc(CC_STD_ALLOCATOR, sizeof(CCInputMap) * 8, NULL, CC_DEFAULT_ERROR_CALLBACK);
                                                        
                                                        for (size_t Loop2 = 0; Loop2 < 8; Loop2++) ((CCInputMap*)Data)->virtual.joystick.inputMaps[Loop2].type = CCInputMapTypeNone;
                                                    }
                                                    
                                                    for (size_t Loop2 = 0; Loop2 < 2; Loop2++)
                                                    {
                                                        CCInputMapDestructor(&((CCInputMap*)Data)->virtual.joystick.inputMaps[(Loop * 2) + Loop2]);
                                                        
                                                        ((CCInputMap*)Data)->virtual.joystick.inputMaps[(Loop * 2) + Loop2].type = CCInputMapTypeNone;
                                                    }
                                                    
                                                    for (size_t Loop2 = 1; Loop2 < Count; Loop2++)
                                                    {
                                                        CCExpressionReflectionDeserialize(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(*Expr), Loop2), &CC_REFLECT(CCInputMap), &((CCInputMap*)Data)->virtual.joystick.inputMaps[(Loop * 2) + (Loop2 - 1)], Zone, Allocator);
                                                    }
                                                    
                                                    ((CCInputMap*)Data)->virtual.joystick.count = CCMax(((CCInputMap*)Data)->virtual.joystick.count, (Loop * 2) + 2);
                                                    
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        break;
                        
                    default:
                        return FALSE;
                }
                
                ((CCInputMap*)Data)->type = InputMapType;
                
                return TRUE;
            }
        }
    }
    
    return FALSE;
}
