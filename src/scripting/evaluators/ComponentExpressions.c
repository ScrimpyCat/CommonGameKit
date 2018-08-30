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
#include "ComponentBase.h"
#include "ComponentSystem.h"
#include "ExpressionHelpers.h"
#include "TextAttribute.h"
#include "GraphicsExpressions.h"
#include "ScriptableInterfaceDynamicFieldComponent.h"

static CCExpression CCComponentExpressionWrapper(CCExpression Expression);

static CCDictionary ComponentDescriptorsName = NULL, ComponentDescriptorsID = NULL;
void CCComponentExpressionRegister(CCString Name, const CCComponentExpressionDescriptor *Descriptor, _Bool Wrapper)
{
    if (!ComponentDescriptorsName)
    {
        ComponentDescriptorsName = CCDictionaryCreate(CC_STD_ALLOCATOR, CCDictionaryHintHeavyFinding | CCDictionaryHintConstantElements, sizeof(CCString), sizeof(CCComponentExpressionDescriptor*), &(CCDictionaryCallbacks){
            .keyDestructor = CCStringDestructorForDictionary,
            .getHash = CCStringHasherForDictionary,
            .compareKeys = CCStringComparatorForDictionary
        });
        
        ComponentDescriptorsID = CCDictionaryCreate(CC_STD_ALLOCATOR, CCDictionaryHintHeavyFinding | CCDictionaryHintConstantElements, sizeof(CCComponentID), sizeof(CCComponentExpressionDescriptor*), NULL);
    }
    
    CCDictionarySetValue(ComponentDescriptorsName, &(CCString){ CCStringCreateByInsertingString(Name, 0, CC_STRING(":")) }, &Descriptor);
    CCDictionarySetValue(ComponentDescriptorsID, &Descriptor->id, &Descriptor);
    
    if (Wrapper) CCExpressionEvaluatorRegister(Name, CCComponentExpressionWrapper);
}

_Bool CCComponentExpressionDeserializeArgument(CCComponent Component, CCExpression Arg, const CCComponentExpressionArgumentDeserializer *Deserializer, size_t Count, _Bool Deferred)
{
    if (CCExpressionGetType(Arg) == CCExpressionValueTypeList)
    {
        const size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Arg));
        if (ArgCount >= 2)
        {
            CCExpression NameExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 0);
            if (CCExpressionGetType(NameExpr) == CCExpressionValueTypeAtom)
            {
                CCString Name = CCExpressionGetAtom(NameExpr);
                for (size_t Loop = 0; Loop < Count; Loop++)
                {
                    if (CCStringEqual(Name, Deserializer[Loop].name))
                    {
                        CCExpression ArgExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 1);
                        const CCExpressionValueType ArgType = CCExpressionGetType(ArgExpr);
                        
                        if (Deserializer[Loop].serializedType != CCExpressionValueTypeUnspecified)
                        {
                            if (ArgCount == 2)
                            {
                                if (ArgType != Deserializer[Loop].serializedType)
                                {
                                    if (CCComponentExpressionDeserializeDeferredArgument(Component, Arg, Deferred)) return TRUE;
                                    
                                    CC_LOG_ERROR_CUSTOM("Serialized type (%u) does not match type (%u) for argument (%S) of component (%u)", ArgType, Deserializer[Loop].serializedType, Name, CCComponentGetID(Component));
                                    return FALSE;
                                }
                            }
                            
                            else
                            {
                                if (CCComponentExpressionDeserializeDeferredArgument(Component, Arg, Deferred)) return TRUE;
                                
                                CC_LOG_ERROR_CUSTOM("Expected serialized type (%u) for argument (%S) of component (%u) but arguments do not match", Deserializer[Loop].serializedType, Name, CCComponentGetID(Component));
                                return FALSE;
                            }
                        }
                        
                        if (ArgCount == 2)
                        {
                            switch (Deserializer[Loop].setterType & CCComponentExpressionArgumentTypeMask)
                            {
                                case CCComponentExpressionArgumentTypeBool:
                                    if (ArgType == CCExpressionValueTypeInteger)
                                    {
                                        ((void(*)(CCComponent,_Bool))Deserializer[Loop].setter)(Component, CCExpressionGetInteger(ArgExpr));
                                        return TRUE;
                                    }
                                    break;
                                    
                                case CCComponentExpressionArgumentTypeUInt8:
                                    if (ArgType == CCExpressionValueTypeInteger)
                                    {
                                        ((void(*)(CCComponent,uint8_t))Deserializer[Loop].setter)(Component, CCExpressionGetInteger(ArgExpr));
                                        return TRUE;
                                    }
                                    break;
                                    
                                case CCComponentExpressionArgumentTypeUInt16:
                                    if (ArgType == CCExpressionValueTypeInteger)
                                    {
                                        ((void(*)(CCComponent,uint16_t))Deserializer[Loop].setter)(Component, CCExpressionGetInteger(ArgExpr));
                                        return TRUE;
                                    }
                                    break;
                                    
                                case CCComponentExpressionArgumentTypeUInt32:
                                    if (ArgType == CCExpressionValueTypeInteger)
                                    {
                                        ((void(*)(CCComponent,uint32_t))Deserializer[Loop].setter)(Component, CCExpressionGetInteger(ArgExpr));
                                        return TRUE;
                                    }
                                    break;
                                    
                                case CCComponentExpressionArgumentTypeUInt64:
                                    if (ArgType == CCExpressionValueTypeInteger)
                                    {
                                        ((void(*)(CCComponent,uint64_t))Deserializer[Loop].setter)(Component, CCExpressionGetInteger(ArgExpr));
                                        return TRUE;
                                    }
                                    break;
                                    
                                case CCComponentExpressionArgumentTypeInt8:
                                    if (ArgType == CCExpressionValueTypeInteger)
                                    {
                                        ((void(*)(CCComponent,int8_t))Deserializer[Loop].setter)(Component, CCExpressionGetInteger(ArgExpr));
                                        return TRUE;
                                    }
                                    break;
                                    
                                case CCComponentExpressionArgumentTypeInt16:
                                    if (ArgType == CCExpressionValueTypeInteger)
                                    {
                                        ((void(*)(CCComponent,int16_t))Deserializer[Loop].setter)(Component, CCExpressionGetInteger(ArgExpr));
                                        return TRUE;
                                    }
                                    break;
                                    
                                case CCComponentExpressionArgumentTypeInt32:
                                    if (ArgType == CCExpressionValueTypeInteger)
                                    {
                                        ((void(*)(CCComponent,int32_t))Deserializer[Loop].setter)(Component, CCExpressionGetInteger(ArgExpr));
                                        return TRUE;
                                    }
                                    break;
                                    
                                case CCComponentExpressionArgumentTypeInt64:
                                    if (ArgType == CCExpressionValueTypeInteger)
                                    {
                                        ((void(*)(CCComponent,int64_t))Deserializer[Loop].setter)(Component, CCExpressionGetInteger(ArgExpr));
                                        return TRUE;
                                    }
                                    break;
                                    
                                case CCComponentExpressionArgumentTypeFloat32:
                                    if (ArgType == CCExpressionValueTypeFloat)
                                    {
                                        ((void(*)(CCComponent,float))Deserializer[Loop].setter)(Component, CCExpressionGetFloat(ArgExpr));
                                        return TRUE;
                                    }
                                    break;
                                    
                                case CCComponentExpressionArgumentTypeFloat64:
                                    if (ArgType == CCExpressionValueTypeFloat)
                                    {
                                        ((void(*)(CCComponent,double))Deserializer[Loop].setter)(Component, CCExpressionGetFloat(ArgExpr));
                                        return TRUE;
                                    }
                                    break;
                                    
                                case CCComponentExpressionArgumentTypeVector2:
                                    if (ArgType == CCExpressionValueTypeList)
                                    {
                                        ((void(*)(CCComponent,CCVector2D))Deserializer[Loop].setter)(Component, CCExpressionGetVector2(ArgExpr));
                                        return TRUE;
                                    }
                                    break;
                                    
                                case CCComponentExpressionArgumentTypeVector3:
                                    if (ArgType == CCExpressionValueTypeList)
                                    {
                                        ((void(*)(CCComponent,CCVector3D))Deserializer[Loop].setter)(Component, CCExpressionGetVector3(ArgExpr));
                                        return TRUE;
                                    }
                                    break;
                                    
                                case CCComponentExpressionArgumentTypeVector4:
                                    if (ArgType == CCExpressionValueTypeList)
                                    {
                                        ((void(*)(CCComponent,CCVector4D))Deserializer[Loop].setter)(Component, CCExpressionGetVector4(ArgExpr));
                                        return TRUE;
                                    }
                                    break;
                                    
                                case CCComponentExpressionArgumentTypeVector2i:
                                    if (ArgType == CCExpressionValueTypeList)
                                    {
                                        ((void(*)(CCComponent,CCVector2Di))Deserializer[Loop].setter)(Component, CCExpressionGetVector2i(ArgExpr));
                                        return TRUE;
                                    }
                                    break;
                                    
                                case CCComponentExpressionArgumentTypeVector3i:
                                    if (ArgType == CCExpressionValueTypeList)
                                    {
                                        ((void(*)(CCComponent,CCVector3Di))Deserializer[Loop].setter)(Component, CCExpressionGetVector3i(ArgExpr));
                                        return TRUE;
                                    }
                                    break;
                                    
                                case CCComponentExpressionArgumentTypeVector4i:
                                    if (ArgType == CCExpressionValueTypeList)
                                    {
                                        ((void(*)(CCComponent,CCVector4Di))Deserializer[Loop].setter)(Component, CCExpressionGetVector4i(ArgExpr));
                                        return TRUE;
                                    }
                                    break;
                                    
                                case CCComponentExpressionArgumentTypeColour:
                                    if (ArgType == CCExpressionValueTypeList)
                                    {
                                        ((void(*)(CCComponent,CCColourRGBA))Deserializer[Loop].setter)(Component, CCExpressionGetColour(ArgExpr));
                                        return TRUE;
                                    }
                                    break;
                                    
                                case CCComponentExpressionArgumentTypeString:
                                    if (ArgType == CCExpressionValueTypeString)
                                    {
                                        if ((Deserializer[Loop].setterType & CCComponentExpressionArgumentTypeOwnershipMask) == CCComponentExpressionArgumentTypeOwnershipTransfer) CCExpressionChangeOwnership(ArgExpr, CCExpressionGetCopy(ArgExpr), NULL);
                                        
                                        ((void(*)(CCComponent,CCString))Deserializer[Loop].setter)(Component, CCExpressionGetString(ArgExpr));
                                        return TRUE;
                                    }
                                    break;
                                    
                                case CCComponentExpressionArgumentTypeData:
                                    if (Deserializer[Loop].serializedType != CCExpressionValueTypeUnspecified)
                                    {
                                        if ((Deserializer[Loop].setterType & CCComponentExpressionArgumentTypeOwnershipMask) == CCComponentExpressionArgumentTypeOwnershipTransfer) CCExpressionChangeOwnership(ArgExpr, CCExpressionGetCopy(ArgExpr), NULL);
                                        
                                        ((void(*)(CCComponent,void*))Deserializer[Loop].setter)(Component, CCExpressionGetData(ArgExpr));
                                        return TRUE;
                                    }
                                    break;
                                    
                                case CCComponentExpressionArgumentTypeExpression:
                                    if ((Deserializer[Loop].setterType & CCComponentExpressionArgumentTypeOwnershipMask) == CCComponentExpressionArgumentTypeOwnershipTransfer) CCExpressionChangeOwnership(ArgExpr, CCExpressionGetCopy(ArgExpr), NULL);
                                    
                                    ((void(*)(CCComponent,CCExpression))Deserializer[Loop].setter)(Component, ArgExpr);
                                    return TRUE;
                                    
                                default:
                                    break;
                            }
                        }
                        
                        else
                        {
                            switch (Deserializer[Loop].setterType & CCComponentExpressionArgumentTypeMask)
                            {
                                case CCComponentExpressionArgumentTypeVector2:
                                    ((void(*)(CCComponent,CCVector2D))Deserializer[Loop].setter)(Component, CCExpressionGetNamedVector2(Arg));
                                    return TRUE;
                                    
                                case CCComponentExpressionArgumentTypeVector3:
                                    ((void(*)(CCComponent,CCVector3D))Deserializer[Loop].setter)(Component, CCExpressionGetNamedVector3(Arg));
                                    return TRUE;
                                    
                                case CCComponentExpressionArgumentTypeVector4:
                                    ((void(*)(CCComponent,CCVector4D))Deserializer[Loop].setter)(Component, CCExpressionGetNamedVector4(Arg));
                                    return TRUE;
                                    
                                case CCComponentExpressionArgumentTypeVector2i:
                                    ((void(*)(CCComponent,CCVector2Di))Deserializer[Loop].setter)(Component, CCExpressionGetNamedVector2i(Arg));
                                    return TRUE;
                                    
                                case CCComponentExpressionArgumentTypeVector3i:
                                    ((void(*)(CCComponent,CCVector3Di))Deserializer[Loop].setter)(Component, CCExpressionGetNamedVector3i(Arg));
                                    return TRUE;
                                    
                                case CCComponentExpressionArgumentTypeVector4i:
                                    ((void(*)(CCComponent,CCVector4Di))Deserializer[Loop].setter)(Component, CCExpressionGetNamedVector4i(Arg));
                                    return TRUE;
                                    
                                case CCComponentExpressionArgumentTypeColour:
                                    ((void(*)(CCComponent,CCColourRGBA))Deserializer[Loop].setter)(Component, CCExpressionGetNamedColour(Arg));
                                    return TRUE;
                                    
                                default:
                                    break;
                            }
                        }
                        
                        switch (Deserializer[Loop].setterType & CCComponentExpressionArgumentTypeMask)
                        {
                            case CCComponentExpressionArgumentTypeTextAttribute:
                                if (ArgType == CCExpressionValueTypeList)
                                {
                                    CCOrderedCollection AttributedStrings = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintOrdered | CCCollectionHintHeavyEnumerating, sizeof(CCTextAttribute), CCTextAttributeDestructorForCollection);
                                    
                                    CCEnumerator Enumerator;
                                    CCCollectionGetEnumerator(CCExpressionGetList(Arg), &Enumerator);
                                    
                                    CCCollectionEnumeratorNext(&Enumerator);
                                    CCGraphicsExpressionGetTextOptions(&Enumerator, AttributedStrings, NULL, NULL, NULL, NULL);
                                    
                                    ((void(*)(CCComponent,CCOrderedCollection))Deserializer[Loop].setter)(Component, AttributedStrings);
                                    
                                    if ((Deserializer[Loop].setterType & CCComponentExpressionArgumentTypeOwnershipMask) == CCComponentExpressionArgumentTypeOwnershipRetain) CCCollectionDestroy(AttributedStrings);
                                    return TRUE;
                                }
                                break;
                                
                            default:
                                break;
                        }
                        
                        if (CCComponentExpressionDeserializeDeferredArgument(Component, Arg, Deferred)) return TRUE;
                        
                        CC_LOG_ERROR_CUSTOM("Could not deserialize argument (%S) of component (%u)", Name, CCComponentGetID(Component));
                    }
                }
            }
        }
    }
    
    return FALSE;
}

_Bool CCComponentExpressionDeserializeDeferredArgument(CCComponent Component, CCExpression Arg, _Bool Deferred)
{
    if (Deferred)
    {
        CCComponent DynamicFieldComponent = CCComponentCreate(CC_SCRIPTABLE_INTERFACE_DYNAMIC_FIELD_COMPONENT_ID | CCScriptableInterfaceTypeTemporaryFlag);
        CCScriptableInterfaceDynamicFieldComponentSetTarget(DynamicFieldComponent, Component);
        CCScriptableInterfaceDynamicFieldComponentSetField(DynamicFieldComponent, CCExpressionRetain(Arg));
        CCScriptableInterfaceDynamicFieldComponentSetReferenceState(DynamicFieldComponent, CCExpressionStateGetSuper(Arg));
        CCComponentSystemAddComponent(DynamicFieldComponent);
        
        CCComponentDestroy(DynamicFieldComponent);
    }
    
    return Deferred;
}

const CCComponentExpressionDescriptor *CCComponentExpressionDescriptorForName(CCString Name)
{
    const CCComponentExpressionDescriptor **Descriptor = CCDictionaryGetValue(ComponentDescriptorsName, &Name);
    
    return Descriptor ? *Descriptor : NULL;
}

const CCComponentExpressionDescriptor *CCComponentExpressionDescriptorForID(CCComponentID id)
{
    const CCComponentExpressionDescriptor **Descriptor = CCDictionaryGetValue(ComponentDescriptorsID, &id);
    
    return Descriptor ? *Descriptor : NULL;
}

CCExpression CCComponentExpressionCreate(CCAllocatorType Allocator, CCComponent Component)
{
    CCAssertLog(Component, "Component must not be null");
    
    CCExpression Result = NULL;
    const CCComponentExpressionDescriptor **Descriptor = CCDictionaryGetValue(ComponentDescriptorsID, &(CCComponentID){ CCComponentGetID(Component) });
    if (Descriptor) Result = (*Descriptor)->serialize(Allocator, Component);
    else CC_LOG_ERROR("No serializer for the given component (%u)", CCComponentGetID(Component));
    
    return Result;
}

static void CCComponentExpressionComponentDestructor(CCComponent Component)
{
    CCComponentSystemRemoveComponent(Component);
    CCComponentDestroy(Component);
}

static CCExpression CCComponentExpressionCreateComponent(CCString Name, CCEnumerator *Enumerator, CCExpression DefaultResult)
{
    const CCComponentExpressionDescriptor **Descriptor = CCDictionaryGetValue(ComponentDescriptorsName, &Name);
    if (Descriptor)
    {
        CCComponent Component = CCComponentCreate((*Descriptor)->id);
        if ((*Descriptor)->initialize) (*Descriptor)->initialize(Component);
        
        for (CCExpression *Expr = CCCollectionEnumeratorNext(Enumerator); Expr; Expr = CCCollectionEnumeratorNext(Enumerator))
        {
            (*Descriptor)->deserialize(Component, CCExpressionEvaluate(*Expr), TRUE);
        }
        
        CCComponentSystemAddComponent(Component);
        
        return CCExpressionCreateCustomType(CC_STD_ALLOCATOR, (CCExpressionValueType)CCComponentExpressionValueTypeComponent, Component, NULL, (CCExpressionValueDestructor)CCComponentExpressionComponentDestructor);
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
