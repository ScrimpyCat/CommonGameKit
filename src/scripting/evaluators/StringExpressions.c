/*
 *  Copyright (c) 2016, Stefan Johnson
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

#include "StringExpressions.h"
#include "ExpressionHelpers.h"
#include <inttypes.h>

CCExpression CCStringExpressionPrefix(CCExpression Expression)
{
    size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    if (ArgCount == 2)
    {
        CCExpression Prefix = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        CCExpression String = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
        if ((CCExpressionGetType(Prefix) == CCExpressionValueTypeString) && (CCExpressionGetType(String) == CCExpressionValueTypeString))
        {
            return CCExpressionCreateInteger(CC_STD_ALLOCATOR, CCStringHasPrefix(CCExpressionGetString(String), CCExpressionGetString(Prefix)));
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("prefix", "prefix:string string:string");
    
    return Expression;
}

CCExpression CCStringExpressionSuffix(CCExpression Expression)
{
    size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    if (ArgCount == 2)
    {
        CCExpression Suffix = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        CCExpression String = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
        if ((CCExpressionGetType(Suffix) == CCExpressionValueTypeString) && (CCExpressionGetType(String) == CCExpressionValueTypeString))
        {
            return CCExpressionCreateInteger(CC_STD_ALLOCATOR, CCStringHasSuffix(CCExpressionGetString(String), CCExpressionGetString(Suffix)));
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("suffix", "suffix:string string:string");
    
    return Expression;
}

CCExpression CCStringExpressionFilename(CCExpression Expression)
{
    size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    if (ArgCount == 1)
    {
        CCExpression Arg = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        if (CCExpressionGetType(Arg) == CCExpressionValueTypeString)
        {
            CCExpression Result = Expression;
            CC_STRING_TEMP_BUFFER(Buffer, CCExpressionGetString(Arg))
            {
                FSPath Path = FSPathCreate(Buffer);
                
                const char *Filename = FSPathGetFilenameString(Path);
                if (Filename) Result = CCExpressionCreateString(CC_STD_ALLOCATOR, CCStringCreate(CC_STD_ALLOCATOR, CCStringEncodingUTF8 | CCStringHintCopy, Filename), FALSE);
                else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Path does not contain a filename: %s", Buffer);
                
                FSPathDestroy(Path);
            }
            
            return Result;
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("filename", "path:string");
    
    return Expression;
}

CCExpression CCStringExpressionReplace(CCExpression Expression)
{
    size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    if (ArgCount == 3)
    {
        CCExpression Occurrence = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        CCExpression Replacement = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
        CCExpression String = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 3));
        
        if ((CCExpressionGetType(Occurrence) == CCExpressionValueTypeString) &&
            (CCExpressionGetType(Replacement) == CCExpressionValueTypeString) &&
            (CCExpressionGetType(String) == CCExpressionValueTypeString))
        {
            return CCExpressionCreateString(CC_STD_ALLOCATOR, CCStringCreateByReplacingOccurrencesOfString(CCExpressionGetString(String), CCExpressionGetString(Occurrence), CCExpressionGetString(Replacement)), FALSE);
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("replace", "occurrence:string replacement:string string:string");
    
    return Expression;
}

CCExpression CCStringExpressionConcatenate(CCExpression Expression)
{
    size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    
    if ((ArgCount == 1) || (ArgCount == 2))
    {
        CCExpression StringsExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        if (CCExpressionGetType(StringsExpr) == CCExpressionValueTypeList)
        {
            CCOrderedCollection Strings = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintOrdered, sizeof(CCString), NULL);
            CC_COLLECTION_FOREACH(CCExpression, String, CCExpressionGetList(StringsExpr))
            {
                if (CCExpressionGetType(String) == CCExpressionValueTypeString)
                {
                    CCOrderedCollectionAppendElement(Strings, &(CCString){ CCExpressionGetString(String) });
                }
                
                else
                {
                    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("cat", "strings:list [separator:string]");
                    CCCollectionDestroy(Strings);
                    
                    return Expression;
                }
            }
            
            CCString Separator = 0;
            if (ArgCount == 2)
            {
                CCExpression SeparatorExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
                if (CCExpressionGetType(SeparatorExpr) != CCExpressionValueTypeString)
                {
                    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("cat", "strings:list [separator:string]");
                    CCCollectionDestroy(Strings);
                    
                    return Expression;
                }
                
                Separator = CCExpressionGetString(SeparatorExpr);
            }
            
            Expression = CCExpressionCreateString(CC_STD_ALLOCATOR, CCStringCreateByJoiningEntries(Strings, Separator), FALSE);
            CCCollectionDestroy(Strings);
            
            return Expression;
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("cat", "strings:list [separator:string]");
    
    return Expression;
}

CCExpression CCStringExpressionLength(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 2)
    {
        CCExpression String = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        if (CCExpressionGetType(String) == CCExpressionValueTypeString)
        {
            return CCExpressionCreateInteger(CC_STD_ALLOCATOR, (int32_t)CCStringGetLength(CCExpressionGetString(String)));
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("length", "string:string");
    
    return Expression;
}

CCExpression CCStringExpressionInsert(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 4)
    {
        CCExpression SrcExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        CCExpression DstExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
        CCExpression OffsetExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 3));
        if ((CCExpressionGetType(SrcExpr) == CCExpressionValueTypeString) && (CCExpressionGetType(DstExpr) == CCExpressionValueTypeString) && (CCExpressionGetType(OffsetExpr) == CCExpressionValueTypeInteger))
        {
            return CCExpressionCreateString(CC_STD_ALLOCATOR, CCStringCreateByInsertingString(CCExpressionGetString(DstExpr), CCExpressionGetInteger(OffsetExpr), CCExpressionGetString(SrcExpr)), FALSE);
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("insert", "source:string destination:string offset:integer");
    
    return Expression;
}

CCExpression CCStringExpressionRemove(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 4)
    {
        CCExpression OffsetExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        CCExpression LengthExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
        CCExpression StringExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 3));
        if ((CCExpressionGetType(OffsetExpr) == CCExpressionValueTypeInteger) && (CCExpressionGetType(LengthExpr) == CCExpressionValueTypeInteger) && (CCExpressionGetType(StringExpr) == CCExpressionValueTypeString))
        {
            return CCExpressionCreateString(CC_STD_ALLOCATOR, CCStringCreateWithoutRange(CCExpressionGetString(StringExpr), CCExpressionGetInteger(OffsetExpr), CCExpressionGetInteger(LengthExpr)), FALSE);
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("remove", "offset:integer length:integer offset:integer");
    
    return Expression;
}

CCExpression CCStringExpressionChop(CCExpression Expression)
{
    const size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    if (ArgCount == 2)
    {
        CCExpression Indexes = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        CCExpression StringExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
        if ((CCExpressionGetType(Indexes) == CCExpressionValueTypeList) && (CCExpressionGetType(StringExpr) == CCExpressionValueTypeString))
        {
            CCString String = CCExpressionGetString(StringExpr);
            const size_t Length = CCStringGetLength(String);
            
            CCExpression Parts = CCExpressionCreateList(CC_STD_ALLOCATOR);
            size_t PrevIndex = 0;
            
            size_t Count = 0;
            CC_COLLECTION_FOREACH(CCExpression, SplitIndex, CCExpressionGetList(Indexes))
            {
                size_t Index = CCExpressionGetInteger(SplitIndex) + 1;
                if (Index >= Length) break;
                else if ((Index <= PrevIndex) && (Count++))
                {
                    CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of chop: indexes must contain a sorted list of integers only");

                    CCExpressionDestroy(Parts);

                    return Expression;
                }
                
                CCOrderedCollectionAppendElement(CCExpressionGetList(Parts), &(CCExpression){ CCExpressionCreateString(CC_STD_ALLOCATOR, CCStringCopySubstring(String, PrevIndex, Index - PrevIndex), FALSE) });
                
                PrevIndex = Index;
            }
            
            CCOrderedCollectionAppendElement(CCExpressionGetList(Parts), &(CCExpression){ CCExpressionCreateString(CC_STD_ALLOCATOR, CCStringCopySubstring(String, PrevIndex, Length - PrevIndex), FALSE) });
            
            return Parts;
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("chop", "indexes:list string:string");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("chop", "indexes:list string:string");
    
    return Expression;
}

typedef struct {
    CCString separator;
    CCString prefix;
    CCString suffix;
    int precision;
    _Bool compact;
} CCStringExpressionFormattingOptions;

static CCString CCStringExpressionBinaryFormatter(CCExpression Expression, CCExpressionValueType Type, const CCStringExpressionFormattingOptions *Options)
{
    char Bits[(sizeof(int32_t) * 8) + 1];
    uint32_t Value = Type == CCExpressionValueTypeInteger ? CCExpressionGetInteger(Expression) : *(uint32_t*)&(float){ CCExpressionGetFloat(Expression) };
    size_t Count = sizeof(uint32_t) * 8;
    for (size_t Loop = 0; Loop < Count; Loop++)
    {
        Bits[(Count - Loop) - 1] = ((Value >> Loop) & 1) + '0';
    }
    Bits[Count] = 0;
    
    
    size_t FormattedLength = Count;
    const char *Formatted = Bits;
    if (Options->compact)
    {
        uint64_t HighestSetBitIndex = CCBitCountSet(CCBitMaskForValue(CCBitHighestSet(Value)));
        if (HighestSetBitIndex)
        {
            Formatted = Bits + (Count - HighestSetBitIndex);
            FormattedLength = HighestSetBitIndex;
        }
        
        else FormattedLength = 1;
    }
    
    else
    {
        int Precision = Options->precision == -1 ? 32 : Options->precision;
        
        Formatted = Bits + (Count - Precision);
        FormattedLength = Precision;
    }
    
    CCString String = CCStringCreateWithSize(CC_STD_ALLOCATOR, CCStringEncodingASCII | CCStringHintCopy, Formatted, FormattedLength);
    
    size_t PartCount = 0;
    CCString Parts[3];
    
    if (Options->prefix) Parts[PartCount++] = Options->prefix;
    Parts[PartCount++] = String;
    if (Options->suffix) Parts[PartCount++] = Options->suffix;
    
    if (PartCount == 1) return String;
    
    CCString FormattedString = CCStringCreateByJoiningStrings(Parts, PartCount, 0);
    CCStringDestroy(String);
    
    return FormattedString;
}

static CCString CCStringExpressionDecimalFormatter(CCExpression Expression, CCExpressionValueType Type, const CCStringExpressionFormattingOptions *Options)
{
    int Precision = Options->precision == -1 ? 0 : Options->precision;
    
    int32_t Value = Type == CCExpressionValueTypeInteger ? CCExpressionGetInteger(Expression) : (int32_t)CCExpressionGetFloat(Expression);
    char Digits[12];
    size_t Length = Options->compact ? sprintf(Digits, "%" PRId32, Value) : sprintf(Digits, "%.*" PRId32, Precision, Value);
    
    CCString String = CCStringCreateWithSize(CC_STD_ALLOCATOR, CCStringEncodingASCII | CCStringHintCopy, Digits, Length);
    
    size_t PartCount = 0;
    CCString Parts[3];
    
    if (Options->prefix) Parts[PartCount++] = Options->prefix;
    Parts[PartCount++] = String;
    if (Options->suffix) Parts[PartCount++] = Options->suffix;
    
    if (PartCount == 1) return String;
    
    CCString FormattedString = CCStringCreateByJoiningStrings(Parts, PartCount, 0);
    CCStringDestroy(String);
    
    return FormattedString;
}

static CCString CCStringExpressionHexFormatter(CCExpression Expression, CCExpressionValueType Type, const CCStringExpressionFormattingOptions *Options)
{
    int Precision = Options->precision == -1 ? 8 : Options->precision;
    
    uint32_t Value = Type == CCExpressionValueTypeInteger ? CCExpressionGetInteger(Expression) : *(uint32_t*)&(float){ CCExpressionGetFloat(Expression) };
    char Digits[9];
    size_t Length = Options->compact ? sprintf(Digits, "%" PRIx32, Value) : sprintf(Digits, "%.*" PRIx32, Precision, Value);
    
    CCString String = CCStringCreateWithSize(CC_STD_ALLOCATOR, CCStringEncodingASCII | CCStringHintCopy, Digits, Length);
    
    size_t PartCount = 0;
    CCString Parts[3];
    
    if (Options->prefix) Parts[PartCount++] = Options->prefix;
    Parts[PartCount++] = String;
    if (Options->suffix) Parts[PartCount++] = Options->suffix;
    
    if (PartCount == 1) return String;
    
    CCString FormattedString = CCStringCreateByJoiningStrings(Parts, PartCount, 0);
    CCStringDestroy(String);
    
    return FormattedString;
}

static CCString CCStringExpressionFloatFormatter(CCExpression Expression, CCExpressionValueType Type, const CCStringExpressionFormattingOptions *Options)
{
    int Precision = Options->precision == -1 ? 6 : Options->precision;
    
    char Digits[32];
    size_t Length = sprintf(Digits, "%.*f", Precision, (Type == CCExpressionValueTypeInteger ? (float)CCExpressionGetInteger(Expression) : CCExpressionGetFloat(Expression)));
    
    if ((Options->compact) && (Precision > 1))
    {
        size_t Loop = 0;
        for (size_t CompactPrecision = Precision - 1; Loop < CompactPrecision; Loop++)
        {
            if (Digits[Length - (Loop + 1)] != '0') break;
        }
        
        Length -= Loop;
    }
    
    CCString String = CCStringCreateWithSize(CC_STD_ALLOCATOR, CCStringEncodingASCII | CCStringHintCopy, Digits, Length);
    
    size_t PartCount = 0;
    CCString Parts[3];
    
    if (Options->prefix) Parts[PartCount++] = Options->prefix;
    Parts[PartCount++] = String;
    if (Options->suffix) Parts[PartCount++] = Options->suffix;
    
    if (PartCount == 1) return String;
    
    CCString FormattedString = CCStringCreateByJoiningStrings(Parts, PartCount, 0);
    CCStringDestroy(String);
    
    return FormattedString;
}

static CCString CCStringExpressionConvertToString(CCExpression Expression, const CCStringExpressionFormattingOptions *Options, CCString (*Formatter)(CCExpression, CCExpressionValueType, const CCStringExpressionFormattingOptions *))
{
    const CCExpressionValueType Type = CCExpressionGetType(Expression);
    switch (Type)
    {
        case CCExpressionValueTypeInteger:
        case CCExpressionValueTypeFloat:
            return Formatter(Expression, Type, Options);
            
        case CCExpressionValueTypeString:
            return CCStringCopy(CCExpressionGetString(Expression));

        case CCExpressionValueTypeAtom:
            return CCStringCopy(CCExpressionGetAtom(Expression));
            
        case CCExpressionValueTypeList:
        {
            CCString String = 0;
            CC_COLLECTION_FOREACH(CCExpression, Expr, CCExpressionGetList(Expression))
            {
                CCString Result = CCStringExpressionConvertToString(Expr, Options, Formatter);
                if (Result)
                {
                    if (String)
                    {
                        CCString FormattedString = CCStringCreateByJoiningStrings((CCString[2]){ String, Result }, 2, Options->separator);
                        
                        CCStringDestroy(Result);
                        CCStringDestroy(String);
                        
                        String = FormattedString;
                    }
                    
                    else String = Result;
                }
            }
            
            return String;
        }
    }
    
    return 0;
}

CCExpression CCStringExpressionFormat(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) >= 3)
    {
        CCEnumerator Enumerator;
        CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
        
        CCExpression TypeExpr = CCExpressionEvaluate(*(CCExpression*)CCCollectionEnumeratorNext(&Enumerator));
        CCExpression ValueExpr = CCExpressionEvaluate(*(CCExpression*)CCCollectionEnumeratorNext(&Enumerator));
        
        if (CCExpressionGetType(TypeExpr) == CCExpressionValueTypeAtom)
        {
            CCString (*Formatter)(CCExpression, CCExpressionValueType, const CCStringExpressionFormattingOptions *);
            CCString Type = CCExpressionGetAtom(TypeExpr);
            
            if (CCStringEqual(Type, CC_STRING(":dec"))) Formatter = CCStringExpressionDecimalFormatter;
            else if (CCStringEqual(Type, CC_STRING(":hex"))) Formatter = CCStringExpressionHexFormatter;
            else if (CCStringEqual(Type, CC_STRING(":flt"))) Formatter = CCStringExpressionFloatFormatter;
            else if (CCStringEqual(Type, CC_STRING(":bin"))) Formatter = CCStringExpressionBinaryFormatter;
            else
            {
                CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("format", "type:atom value:expr ...:options");
                
                return Expression;
            }
            
            
            CCStringExpressionFormattingOptions Options = {
                .separator = 0, .prefix = 0, .suffix = 0,
                .compact = TRUE,
                .precision = -1
            };
            
            for (CCExpression *OptionExpr; (OptionExpr = CCCollectionEnumeratorNext(&Enumerator)); )
            {
                CCExpression Option = CCExpressionEvaluate(*OptionExpr);
                if ((CCExpressionGetType(Option) == CCExpressionValueTypeList) && (CCCollectionGetCount(CCExpressionGetList(Option)) == 2))
                {
                    CCExpression NameExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Option), 0);
                    if (CCExpressionGetType(NameExpr) == CCExpressionValueTypeAtom)
                    {
                        CCString Name = CCExpressionGetAtom(NameExpr);
                        if (CCStringEqual(Name, CC_STRING("separator:")))
                        {
                            Options.separator = CCExpressionGetNamedString(Option);
                        }
                        
                        else if (CCStringEqual(Name, CC_STRING("prefix:")))
                        {
                            Options.prefix = CCExpressionGetNamedString(Option);
                        }
                        
                        else if (CCStringEqual(Name, CC_STRING("suffix:")))
                        {
                            Options.suffix = CCExpressionGetNamedString(Option);
                        }
                        
                        else if (CCStringEqual(Name, CC_STRING("compact:")))
                        {
                            Options.compact = CCExpressionGetNamedInteger(Option);
                        }
                        
                        else if (CCStringEqual(Name, CC_STRING("precision:")))
                        {
                            Options.precision = CCExpressionGetNamedInteger(Option);
                        }
                        
                        else
                        {
                            CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("format", "type:atom value:expr ...:options");
                            
                            return Expression;
                        }
                    }
                    
                    else
                    {
                        CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("format", "type:atom value:expr ...:options");
                        
                        return Expression;
                    }
                }
                
                else
                {
                    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("format", "type:atom value:expr ...:options");
                    
                    return Expression;
                }
            }
            
            
            CCString Result = CCStringExpressionConvertToString(ValueExpr, &Options, Formatter);
            
            return Result ? CCExpressionCreateString(CC_STD_ALLOCATOR, Result, FALSE) : CCExpressionCreateString(CC_STD_ALLOCATOR, CC_STRING(""), TRUE);
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("format", "type:atom value:expr ...:options");
    
    return Expression;
}
