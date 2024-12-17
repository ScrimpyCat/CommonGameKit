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

#include "ExpressionMap.h"

static _Bool CCExpressionMapArgs(const CCExpressionMapArg **Args, CCExpression ArgExpr, CCEnumerator *Enumerator, CCOrderedCollection List)
{
    _Bool Result = FALSE;
    const CCExpressionMapMatch *Match = (*Args)->match;
    const CCExpressionMapTo **To = (*Args)->to;
    uintmax_t EnumValue = 0;
    
    if (Match)
    {
        switch (Match->type)
        {
            case CCExpressionMapMatchTypeAlways:
                Result = TRUE;
                break;
                
            case CCExpressionMapMatchTypeArgs:
                Result = CCExpressionMapArgs(Match->args, ArgExpr, Enumerator, List);
                break;
                
            case CCExpressionMapMatchTypeAtom:
                Result = ((CCExpressionGetType(ArgExpr) == CCExpressionValueTypeAtom) && (CCStringEqual(CCExpressionGetAtom(ArgExpr), Match->atom)));
                break;
                
            case CCExpressionMapMatchTypeString:
                Result = ((CCExpressionGetType(ArgExpr) == CCExpressionValueTypeString) && (CCStringEqual(CCExpressionGetString(ArgExpr), Match->string)));
                break;
                
            case CCExpressionMapMatchTypeInteger:
                Result = ((CCExpressionGetType(ArgExpr) == CCExpressionValueTypeInteger) && (CCExpressionGetInteger(ArgExpr) == Match->integer));
                break;
                
            case CCExpressionMapMatchTypeEnums:
                if (CCExpressionGetType(ArgExpr) == CCExpressionValueTypeAtom)
                {
                    const uintmax_t *Value = CCDictionaryGetValue(Match->enums, &(CCString){ CCExpressionGetAtom(ArgExpr) });
                    
                    if (Value)
                    {
                        EnumValue = *Value;
                        
                        Result = TRUE;
                    }
                }
                break;
                
            case CCExpressionMapMatchTypeValueType:
                Result = (CCExpressionGetType(ArgExpr) == Match->valueType);
                break;
                
            default:
                break;
        }
        
        if (Result)
        {
            
            if (*(Args + 1))
            {
                CCExpression *NextArgExpr = CCCollectionEnumeratorNext(Enumerator);
                
                Result = NextArgExpr ? CCExpressionMapArgs(Args + 1, *NextArgExpr, Enumerator, List) : FALSE;
            }
            
            if (Result)
            {
                switch (Match->type)
                {
                    case CCExpressionMapMatchTypeAlways:
                    case CCExpressionMapMatchTypeArgs:
                    case CCExpressionMapMatchTypeAtom:
                    case CCExpressionMapMatchTypeString:
                    case CCExpressionMapMatchTypeInteger:
                    case CCExpressionMapMatchTypeValueType:
                        while (*To)
                        {
                            CCExpression MappedArg = CCExpressionCreateList(CC_STD_ALLOCATOR);
                            
                            CCOrderedCollectionAppendElement(CCExpressionGetList(MappedArg), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, (*To)->name, TRUE) });
                            
                            if ((*To)->value)
                            {
                                CCOrderedCollectionAppendElement(CCExpressionGetList(MappedArg), &(CCExpression){ CCExpressionCopy((*To)->value) });
                            }
                            
                            else
                            {
                                CCOrderedCollectionAppendElement(CCExpressionGetList(MappedArg), &(CCExpression){ CCExpressionCopy(ArgExpr) });
                            }
                            
                            CCOrderedCollectionAppendElement(List, &MappedArg);
                            
                            To++;
                        }
                        break;
                        
                    case CCExpressionMapMatchTypeEnums:
                        while (*To)
                        {
                            CCExpression MappedArg = CCExpressionCreateList(CC_STD_ALLOCATOR);
                            
                            CCOrderedCollectionAppendElement(CCExpressionGetList(MappedArg), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, (*To)->name, TRUE) });
                            
                            if ((*To)->value)
                            {
                                CCOrderedCollectionAppendElement(CCExpressionGetList(MappedArg), &(CCExpression){ CCExpressionCopy((*To)->value) });
                            }
                            
                            else
                            {
                                if (!(EnumValue & ~(uintmax_t)0xffffffff))
                                {
                                    CCOrderedCollectionAppendElement(CCExpressionGetList(MappedArg), &(CCExpression){ CCExpressionCreateInteger(CC_STD_ALLOCATOR, (int32_t)EnumValue) });
                                }
                                
                                else
                                {
                                    CCBigIntFast Integer = CCBigIntFastCreate(CC_STD_ALLOCATOR);
                                    CCBigIntFastSetInt(&Integer,  EnumValue);
                                    
                                    CCOrderedCollectionAppendElement(CCExpressionGetList(MappedArg), &(CCExpression){ CCExpressionCreateString(CC_STD_ALLOCATOR, CCBigIntFastGetString(Integer), FALSE) });
                                    
                                    CCBigIntFastDestroy(Integer);
                                }
                            }
                            
                            CCOrderedCollectionAppendElement(List, &MappedArg);
                            
                            To++;
                        }
                        break;
                        
                    default:
                        break;
                }
            }
        }
    }
    
    return Result;
}

CCExpression CCExpressionMap(CCExpression Expression, CCDictionary Map)
{
    CCExpression Result = CCExpressionCreateList(CC_STD_ALLOCATOR);
    CCCollection Fields = CCExpressionGetList(Result);
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
    
    for (CCExpression *Expr = CCCollectionEnumeratorNext(&Enumerator); Expr; Expr = CCCollectionEnumeratorNext(&Enumerator))
    {
        CCExpression OptionExpr = CCExpressionEvaluate(*Expr);
        
        if (CCExpressionGetType(OptionExpr) == CCExpressionValueTypeList)
        {
            CCCollection Option = CCExpressionGetList(OptionExpr);
            const size_t ArgCount = CCCollectionGetCount(Option);
            
            if (ArgCount >= 2)
            {
                CCEnumerator OptionEnumerator;
                CCCollectionGetEnumerator(Option, &OptionEnumerator);
                
                CCExpression NameExpr = *(CCExpression*)CCCollectionEnumeratorGetCurrent(&OptionEnumerator);
                
                if (CCExpressionGetType(NameExpr) == CCExpressionValueTypeAtom)
                {
                    CCString Name = CCExpressionGetAtom(NameExpr);
                    const CCExpressionMapArg ***Mappings = CCDictionaryGetValue(Map, &Name);
                    
                    if (Mappings)
                    {
                        CCExpression ArgExpr = *(CCExpression*)CCCollectionEnumeratorNext(&OptionEnumerator);
                        const CCExpressionMapArg *Mapping;
                        
                        for (size_t Loop = 0; (Mapping = (*Mappings)[Loop]); Loop++)
                        {
                            CCEnumerator ArgEnumerator = OptionEnumerator;
                            
                            if (Mapping->match)
                            {
                                if (CCExpressionMapArgs((const CCExpressionMapArg*[]){ Mapping, NULL }, ArgExpr, &ArgEnumerator, Fields)) break;
                            }
                            
                            else if (Mapping->map(ArgExpr, &ArgEnumerator, Fields)) break;
                        }
                    }
                }
            }
        }
    }
    
    return Result;
}
