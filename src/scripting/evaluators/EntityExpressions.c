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

#include "EntityExpressions.h"
#include "ComponentExpressions.h"
#include "Entity.h"
#include "EntityManager.h"

CCExpression CCEntityExpressionEntity(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) >= 2)
    {
        CCEnumerator Enumerator;
        CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
        
        CCExpression Name = CCExpressionEvaluate(*(CCExpression*)CCCollectionEnumeratorNext(&Enumerator));
        if (CCExpressionGetType(Name) == CCExpressionValueTypeString)
        {
            CCEntity Entity = CCEntityCreate(CCExpressionGetString(Name), CC_STD_ALLOCATOR);
            for (CCExpression *Expr = CCCollectionEnumeratorNext(&Enumerator); Expr; Expr = CCCollectionEnumeratorNext(&Enumerator))
            {
                CCExpression Component = CCExpressionEvaluate(*Expr);
                if (CCExpressionGetType(Component) == CCComponentExpressionValueTypeComponent)
                {
                    CCExpressionChangeOwnership(Component, NULL, NULL);
                    CCEntityAttachComponent(Entity, CCExpressionGetData(Component));
                }
            }
            
            CCEntityManagerAddEntity(Entity);
            
            return CCExpressionCreateCustomType(CC_STD_ALLOCATOR, CCEntityExpressionValueTypeEntity, Entity, NULL, NULL);
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("entity", "name:string [...:component]");
    
    return Expression;
}
