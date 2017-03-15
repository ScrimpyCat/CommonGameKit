/*
 *  Copyright (c) 2017, Stefan Johnson
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

#include "ComponentExpressions.h"

static CCExpression CCComponentExpressionWrapper(CCExpression Expression);

static CCDictionary ComponentDescriptors = NULL;
void CCComponentExpressionRegister(CCString Name, const CCComponentExpressionDescriptor *Descriptor, _Bool Wrapper)
{
    if (!ComponentDescriptors)
    {
        ComponentDescriptors = CCDictionaryCreate(CC_STD_ALLOCATOR, CCDictionaryHintHeavyFinding | CCDictionaryHintConstantElements, sizeof(CCString), sizeof(CCComponentExpressionDescriptor*), &(CCDictionaryCallbacks){
            .keyDestructor = CCStringDestructorForDictionary,
            .getHash = CCStringHasherForDictionary,
            .compareKeys = CCStringComparatorForDictionary
        });
    }
    
    CCDictionarySetValue(ComponentDescriptors, &(CCString){ CCStringCreateByInsertingString(Name, 0, CC_STRING(":")) }, &Descriptor);
    
    if (Wrapper) CCExpressionEvaluatorRegister(Name, CCComponentExpressionWrapper);
}

static CCExpression CCComponentExpressionCreateComponent(CCString Name, CCEnumerator *Enumerator, CCExpression DefaultResult)
{
    const CCComponentExpressionDescriptor **Descriptor = CCDictionaryGetValue(ComponentDescriptors, &Name);
    if (Descriptor)
    {
        CCComponent Component = CCComponentCreate((*Descriptor)->id);
        
        for (CCExpression *Expr = CCCollectionEnumeratorNext(Enumerator); Expr; Expr = CCCollectionEnumeratorNext(Enumerator))
        {
            (*Descriptor)->deserialize(Component, CCExpressionEvaluate(*Expr));
        }
        
        return CCExpressionCreateCustomType(CC_STD_ALLOCATOR, (CCExpressionValueType)CCComponentExpressionValueTypeComponent, Component, NULL, (CCExpressionValueDestructor)CCComponentDestroy);
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Invalid argument component: no component for name (%S)", Name);
    
    return DefaultResult;
}

static CCExpression CCComponentExpressionWrapper(CCExpression Expression)
{
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
    
    CCExpression *NameExpr = CCCollectionEnumeratorGetCurrent(&Enumerator);
    
    CCString Name = CCStringCreateByInsertingString(CCExpressionGetAtom(*NameExpr), 0, CC_STRING(":"));
    
    CCExpression Result = CCComponentExpressionCreateComponent(Name, &Enumerator, Expression);

    CCStringDestroy(Name);
    
    return Result;
}

CCExpression CCComponentExpressionComponent(CCExpression Expression)
{
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
    
    CCExpression *NameExpr = CCCollectionEnumeratorNext(&Enumerator);
    if (NameExpr)
    {
        CCExpression Name = CCExpressionEvaluate(*NameExpr);
        if (CCExpressionGetType(Name) == CCExpressionValueTypeAtom)
        {
            return CCComponentExpressionCreateComponent(CCExpressionGetAtom(Name), &Enumerator, Expression);
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("component", "name:atom [...:expr]");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("component", "name:atom [...:expr]");
    
    return Expression;
}
