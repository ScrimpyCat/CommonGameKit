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

#include "InputMapGroupComponent.h"
#include "ComponentExpressions.h"

const char * const CCInputMapGroupComponentName = "input_map_group";

static CCExpression CCInputMapGroupComponentSerializer(CCAllocatorType Allocator, CCComponent Component);

static const CCComponentExpressionDescriptor CCInputMapGroupComponentDescriptor = {
    .id = CC_INPUT_MAP_GROUP_COMPONENT_ID,
    .initialize = NULL,
    .deserialize = CCInputMapGroupComponentDeserializer,
    .serialize = CCInputMapGroupComponentSerializer
};

void CCInputMapGroupComponentRegister(void)
{
    CCComponentRegister(CC_INPUT_MAP_GROUP_COMPONENT_ID, CCInputMapGroupComponentName, CC_STD_ALLOCATOR, sizeof(CCInputMapGroupComponentClass), CCInputMapGroupComponentInitialize, NULL, CCInputMapGroupComponentDeallocate);
    
    CCComponentExpressionRegister(CC_STRING("input-group"), &CCInputMapGroupComponentDescriptor, TRUE);
}

void CCInputMapGroupComponentDeregister(void)
{
    CCComponentDeregister(CC_INPUT_MAP_GROUP_COMPONENT_ID);
}

void CCInputMapGroupComponentElementDestructor(CCCollection Collection, CCComponent *Element)
{
    CCComponentDestroy(*Element);
}

static CCComponentExpressionArgumentDeserializer Arguments[] = {
    { .name = CC_STRING("all-active:"), .serializedType = CCExpressionValueTypeUnspecified, .setterType = CCComponentExpressionArgumentTypeBool, .setter = (void(*)())CCInputMapGroupComponentSetWantsAllActive }
};

void CCInputMapGroupComponentDeserializer(CCComponent Component, CCExpression Arg)
{
    if ((CCExpressionGetType(Arg) == CCExpressionValueTypeList) && (CCCollectionGetCount(CCExpressionGetList(Arg)) >= 2))
    {
        CCExpression Name = *(CCExpression*)CCOrderedCollectionGetEntryAtIndex(CCExpressionGetList(Arg), 0);
        if ((CCExpressionGetType(Arg) == CCExpressionValueTypeString) && (CCStringEqual(CCExpressionGetString(Name), CC_STRING("input:"))))
        {
            CCEnumerator Enumerator;
            CCCollectionGetEnumerator(CCExpressionGetList(Arg), &Enumerator);
            
            for (CCExpression *Expr = CCCollectionEnumeratorNext(&Enumerator); Expr; Expr = CCCollectionEnumeratorNext(&Enumerator))
            {
                if (CCExpressionGetType(*Expr) == CCComponentExpressionValueTypeComponent)
                {
                    CCExpressionChangeOwnership(*Expr, NULL, NULL);
                    
                    CCComponent Map = CCExpressionGetData(*Expr);
                    if ((CCComponentGetID(Map) & CC_INPUT_COMPONENT_FLAG) == CC_INPUT_COMPONENT_FLAG)
                    {
                        CCInputMapGroupComponentAddInputMap(Component, Map);
                    }
                    
                    else CC_LOG_ERROR_CUSTOM("Invalid component type (%u) for argument (input:)", CCComponentGetID(Map));
                }
                
                else if (CCExpressionGetType(*Expr) == CCExpressionValueTypeList)
                {
                    CC_COLLECTION_FOREACH(CCExpression, MapExpr, CCExpressionGetList(*Expr))
                    {
                        if (CCExpressionGetType(MapExpr) == CCComponentExpressionValueTypeComponent)
                        {
                            CCExpressionChangeOwnership(MapExpr, NULL, NULL);
                            
                            CCComponent Map = CCExpressionGetData(MapExpr);
                            if ((CCComponentGetID(Map) & CC_INPUT_COMPONENT_FLAG) == CC_INPUT_COMPONENT_FLAG)
                            {
                                CCInputMapGroupComponentAddInputMap(Component, Map);
                            }
                            
                            else CC_LOG_ERROR_CUSTOM("Invalid component type (%u) for argument (input:)", CCComponentGetID(Map));
                        }
                    }
                }
            }
            
            return;
        }
    }
    
    if (!CCComponentExpressionDeserializeArgument(Component, Arg, Arguments, sizeof(Arguments) / sizeof(typeof(*Arguments))))
    {
        CCInputMapComponentDeserializer(Component, Arg);
    }
}

static CCExpression CCInputMapGroupComponentSerializer(CCAllocatorType Allocator, CCComponent Component)
{
    return NULL;
}