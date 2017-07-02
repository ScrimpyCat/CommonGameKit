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

#include "MessageExpressions.h"
#include "ComponentExpressions.h"
#include "Message.h"

static CCDictionary Messages = NULL;
void CCMessageExpressionRegister(CCString Name, CCMessageExpressionPoster Poster)
{
    if (!Messages)
    {
        Messages = CCDictionaryCreate(CC_STD_ALLOCATOR, CCDictionaryHintHeavyFinding, sizeof(CCString), sizeof(CCMessageExpressionPoster), &(CCDictionaryCallbacks){
            .keyDestructor = CCStringDestructorForDictionary,
            .getHash = CCStringHasherForDictionary,
            .compareKeys = CCStringComparatorForDictionary
        });
    }
    
    CCDictionarySetValue(Messages, &(CCString){ CCStringCopy(Name) }, &Poster);
}

CCExpression CCMessageExpressionComponentRouter(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 2)
    {
        CCExpression Name = CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1);
        if (CCExpressionGetType(Name) == CCExpressionValueTypeAtom)
        {
            const CCComponentExpressionDescriptor *Descriptor = CCComponentExpressionDescriptorForName(CCExpressionGetAtom(Name));
            if (Descriptor)
            {
                CCMessageRouter *Router = CCMessageDeliverToComponent(Descriptor->id);
                
                return CCExpressionCreateCustomType(CC_STD_ALLOCATOR, CCMessageExpressionValueTypeRouter, &Router, CCExpressionRetainedValueCopy, (CCExpressionValueDestructor)CCMessageRouterDestroy);
            }
            
            CC_EXPRESSION_EVALUATOR_LOG_ERROR("component-router: No component exists for name (%S)", CCExpressionGetAtom(Name));
            
            return Expression;
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("component-router", "name:atom");
    
    return Expression;
}
