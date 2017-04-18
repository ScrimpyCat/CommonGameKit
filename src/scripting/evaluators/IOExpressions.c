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

#include "IOExpressions.h"
#include <inttypes.h>

CCExpression CCIOExpressionPrint(CCExpression Expression)
{
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
    
    for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
    {
        CCExpression Arg = CCExpressionEvaluate(*Expr);
        switch (CCExpressionGetType(Arg))
        {
            case CCExpressionValueTypeNull:
                CC_EXPRESSION_EVALUATOR_LOG("<null>");
                break;
                
            case CCExpressionValueTypeAtom:
                CC_EXPRESSION_EVALUATOR_LOG("%S", CCExpressionGetAtom(Arg));
                break;
                
            case CCExpressionValueTypeInteger:
                CC_EXPRESSION_EVALUATOR_LOG("%" PRId32, CCExpressionGetInteger(Arg));
                break;
                
            case CCExpressionValueTypeFloat:
                CC_EXPRESSION_EVALUATOR_LOG("%f", CCExpressionGetFloat(Arg));
                break;
                
            case CCExpressionValueTypeString:
                CC_EXPRESSION_EVALUATOR_LOG("\"%S\"", CCExpressionGetString(Arg));
                break;
                
            case CCExpressionValueTypeList:
                CC_EXPRESSION_EVALUATOR_LOG("[%p]", CCExpressionGetList(Arg));
                CCExpressionPrint(Arg);
                break;
                
            default:
                CC_EXPRESSION_EVALUATOR_LOG("(%d):%p", CCExpressionGetType(Arg), CCExpressionGetData(Arg));
                break;
        }
    }
    
    return Expression;
}

CCExpression CCIOExpressionSearch(CCExpression Expression)
{
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
    
    CCExpression *Dir = CCCollectionEnumeratorNext(&Enumerator), CurrentDir;
    if ((!Dir) || (CCExpressionGetType((CurrentDir = CCExpressionEvaluate(*Dir))) != CCExpressionValueTypeString))
    {
        CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("search", "path:string [matches:string|list]");
        return Expression;
    }
    
    CCCollection Matches = NULL;
    CCExpression *MatchArg = CCCollectionEnumeratorNext(&Enumerator);
    if (MatchArg)
    {
        CCExpression Arg = CCExpressionEvaluate(*MatchArg);
        Matches = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintHeavyEnumerating, sizeof(FSPath), FSPathDestructorForCollection);
        
        if ((CCExpressionGetType(Arg) == CCExpressionValueTypeString))
        {
            CC_STRING_TEMP_BUFFER(Buffer, CCExpressionGetString(Arg)) CCCollectionInsertElement(Matches, &(FSPath){ FSPathCreate(Buffer) });
        }
        
        else if ((CCExpressionGetType(Arg) == CCExpressionValueTypeList))
        {
            CC_COLLECTION_FOREACH(CCExpression, Match, CCExpressionGetList(Arg))
            {
                if ((CCExpressionGetType(Match) == CCExpressionValueTypeString))
                {
                    CC_STRING_TEMP_BUFFER(Buffer, CCExpressionGetString(Match)) CCCollectionInsertElement(Matches, &(FSPath){ FSPathCreate(Buffer) });
                }
                
                else
                {
                    CCCollectionDestroy(Matches);
                    
                    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("search", "path:string [matches:string|list]");
                    return Expression;
                }
            }
        }
        
        else
        {
            CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("search", "path:string [matches:string|list]");
            return Expression;
        }
    }
    
    FSPath Path = NULL;
    CC_STRING_TEMP_BUFFER(Buffer, CCExpressionGetString(CurrentDir)) Path = FSPathCreate(Buffer);
    
    CCOrderedCollection Paths = FSManagerGetContentsAtPath(Path, Matches, FSMatchSkipHidden | FSMatchSearchRecursively);
    
    FSPathDestroy(Path);
    
    if (Matches) CCCollectionDestroy(Matches);
    
    CCExpression List = CCExpressionCreateList(CC_STD_ALLOCATOR);
    if (Paths)
    {
        CC_COLLECTION_FOREACH(FSPath, Path, Paths)
        {
            CCOrderedCollectionAppendElement(CCExpressionGetList(List), &(CCExpression){ CCExpressionCreateString(CC_STD_ALLOCATOR, CCStringCreate(CC_STD_ALLOCATOR, CCStringEncodingUTF8 | CCStringHintCopy, FSPathGetPathString(Path)), FALSE) });
        }
        
        CCCollectionDestroy(Paths);
    }
    
    return List;
}

CCExpression CCIOExpressionEval(CCExpression Expression)
{
    CCExpression Expr = Expression;
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 2)
    {
        CCExpression Path = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        if (CCExpressionGetType(Path) == CCExpressionValueTypeString)
        {
            CC_STRING_TEMP_BUFFER(Buffer, CCExpressionGetString(Path))
            {
                FSPath Path = FSPathCreate(Buffer);
                
                CCExpression Src = CCExpressionCreateFromSourceFile(Path);
                if (Src)
                {
                    CCExpressionStateSetSuper(Src, Expression);
                    Expr = CCExpressionRetain(CCExpressionEvaluate(Src));
                    CCExpressionDestroy(Src);
                }
                
                FSPathDestroy(Path);
            }
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("eval", "path:string");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("eval", "path:string");
    
    return Expr;
}
