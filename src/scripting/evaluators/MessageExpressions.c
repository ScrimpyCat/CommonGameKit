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

#define CC_QUICK_COMPILE
#include "MessageExpressions.h"
#include "ComponentExpressions.h"
#include "Message.h"

static CCDictionary MessageDescriptors = NULL;
void CCMessageExpressionRegister(CCString Name, const CCMessageExpressionDescriptor *Descriptor)
{
    if (!MessageDescriptors)
    {
        MessageDescriptors = CCDictionaryCreate(CC_STD_ALLOCATOR, CCDictionaryHintHeavyFinding, sizeof(CCString), sizeof(CCMessageExpressionDescriptor*), &(CCDictionaryCallbacks){
            .keyDestructor = CCStringDestructorForDictionary,
            .getHash = CCStringHasherForDictionary,
            .compareKeys = CCStringComparatorForDictionary
        });
    }
    
    CCDictionarySetValue(MessageDescriptors, &(CCString){ CCStringCopy(Name) }, &Descriptor);
}

_Bool CCMessageExpressionBasicPoster(CCMessageRouter *Router, CCMessageID id, CCExpression Args)
{
    if (!Args)
    {
        CCMessagePost(CC_STD_ALLOCATOR, id, Router, 0, NULL, NULL);
        return TRUE;
    }
    
    CCMessageRouterDestroy(Router);
    
    return FALSE;
}

CCExpression CCMessageExpressionComponentRouter(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 2)
    {
        CCExpression Name = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        if (CCExpressionGetType(Name) == CCExpressionValueTypeAtom)
        {
            const CCComponentExpressionDescriptor *Descriptor = CCComponentExpressionDescriptorForName(CCExpressionGetAtom(Name));
            if (Descriptor)
            {
                CCMessageRouter *Router = CCMessageDeliverToComponent(Descriptor->id);
                
                return CCExpressionCreateCustomType(CC_STD_ALLOCATOR, CCMessageExpressionValueTypeRouter, Router, CCExpressionRetainedValueCopy, (CCExpressionValueDestructor)CCMessageRouterDestroy);
            }
            
            CC_EXPRESSION_EVALUATOR_LOG_ERROR("component-router: No component exists for name (%S)", CCExpressionGetAtom(Name));
            
            return Expression;
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("component-router", "name:atom");
    
    return Expression;
}

CCExpression CCMessageExpressionMessage(CCExpression Expression)
{
    size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    if ((ArgCount >= 2) && (ArgCount <= 3))
    {
        CCExpression Router = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        CCExpression Name = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
        if ((CCExpressionGetType(Router) == CCMessageExpressionValueTypeRouter) && (CCExpressionGetType(Name) == CCExpressionValueTypeAtom))
        {
            const CCMessageExpressionDescriptor **Descriptor = CCDictionaryGetValue(MessageDescriptors, &(CCString){ CCExpressionGetAtom(Name) });
            if (Descriptor)
            {
                return CCExpressionCreateInteger(CC_STD_ALLOCATOR, (*Descriptor)->post(CCRetain(CCExpressionGetData(Router)), (*Descriptor)->id, (ArgCount == 3 ? CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 3)) : NULL)));
            }
            
            CC_EXPRESSION_EVALUATOR_LOG_ERROR("message: No message exists for name (%S)", CCExpressionGetAtom(Name));
            
            return Expression;
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("message", "router:custom message:atom [args:expr]");
    
    return Expression;
}
