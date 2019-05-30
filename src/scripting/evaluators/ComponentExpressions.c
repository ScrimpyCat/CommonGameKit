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

#define CC_COMPONENT_EXPRESSION_STORE_EXPR(type, expr) \
type const Value = expr; \
switch (ContainerType) \
{ \
    case CCComponentExpressionArgumentTypeContainerComponent: \
        ((void(*)(CCComponent,type))Deserializer->setter)(Container, Value); \
        break; \
    case CCComponentExpressionArgumentTypeContainerArray: \
        CCArrayAppendElement(Container, &Value); \
        break; \
    case CCComponentExpressionArgumentTypeContainerList: \
        CCLinkedListAppend(Container, CCLinkedListCreateNode(CC_STD_ALLOCATOR, sizeof(type), &Value)); \
        break; \
    case CCComponentExpressionArgumentTypeContainerOrderedCollection: \
        CCOrderedCollectionAppendElement(Container, &Value); \
        break; \
    default: \
        return FALSE; \
}

static _Bool CCComponentExpressionDeserializeExpression(const CCComponentExpressionArgumentDeserializer *Deserializer, CCExpressionValueType ExprType, CCExpression Expr, void *Container)
{
    const CCComponentExpressionArgumentSetterType ContainerType = Deserializer->setterType & CCComponentExpressionArgumentTypeContainerMask;
    switch (Deserializer->setterType & CCComponentExpressionArgumentTypeMask)
    {
        case CCComponentExpressionArgumentTypeBool:
            if (ExprType == CCExpressionValueTypeInteger)
            {
                CC_COMPONENT_EXPRESSION_STORE_EXPR(_Bool, CCExpressionGetInteger(Expr));
                return TRUE;
            }
            break;

        case CCComponentExpressionArgumentTypeUInt8:
            if (ExprType == CCExpressionValueTypeInteger)
            {
                CC_COMPONENT_EXPRESSION_STORE_EXPR(uint8_t, CCExpressionGetInteger(Expr));
                return TRUE;
            }
            
            else if (ExprType == CCExpressionValueTypeFloat)
            {
                CC_COMPONENT_EXPRESSION_STORE_EXPR(uint8_t, CCExpressionGetFloat(Expr));
                return TRUE;
            }
            break;

        case CCComponentExpressionArgumentTypeUInt16:
            if (ExprType == CCExpressionValueTypeInteger)
            {
                CC_COMPONENT_EXPRESSION_STORE_EXPR(uint16_t, CCExpressionGetInteger(Expr));
                return TRUE;
            }
            
            else if (ExprType == CCExpressionValueTypeFloat)
            {
                CC_COMPONENT_EXPRESSION_STORE_EXPR(uint16_t, CCExpressionGetFloat(Expr));
                return TRUE;
            }
            break;

        case CCComponentExpressionArgumentTypeUInt32:
            if (ExprType == CCExpressionValueTypeInteger)
            {
                CC_COMPONENT_EXPRESSION_STORE_EXPR(uint32_t, CCExpressionGetInteger(Expr));
                return TRUE;
            }
            
            else if (ExprType == CCExpressionValueTypeFloat)
            {
                CC_COMPONENT_EXPRESSION_STORE_EXPR(uint32_t, CCExpressionGetFloat(Expr));
                return TRUE;
            }
            break;

        case CCComponentExpressionArgumentTypeUInt64:
            if (ExprType == CCExpressionValueTypeInteger)
            {
                CC_COMPONENT_EXPRESSION_STORE_EXPR(uint64_t, CCExpressionGetInteger(Expr));
                return TRUE;
            }
            
            else if (ExprType == CCExpressionValueTypeFloat)
            {
                CC_COMPONENT_EXPRESSION_STORE_EXPR(uint64_t, CCExpressionGetFloat(Expr));
                return TRUE;
            }
            break;

        case CCComponentExpressionArgumentTypeInt8:
            if (ExprType == CCExpressionValueTypeInteger)
            {
                CC_COMPONENT_EXPRESSION_STORE_EXPR(int8_t, CCExpressionGetInteger(Expr));
                return TRUE;
            }
            
            else if (ExprType == CCExpressionValueTypeFloat)
            {
                CC_COMPONENT_EXPRESSION_STORE_EXPR(int8_t, CCExpressionGetFloat(Expr));
                return TRUE;
            }
            break;

        case CCComponentExpressionArgumentTypeInt16:
            if (ExprType == CCExpressionValueTypeInteger)
            {
                CC_COMPONENT_EXPRESSION_STORE_EXPR(int16_t, CCExpressionGetInteger(Expr));
                return TRUE;
            }
            
            else if (ExprType == CCExpressionValueTypeFloat)
            {
                CC_COMPONENT_EXPRESSION_STORE_EXPR(int16_t, CCExpressionGetFloat(Expr));
                return TRUE;
            }
            break;

        case CCComponentExpressionArgumentTypeInt32:
            if (ExprType == CCExpressionValueTypeInteger)
            {
                CC_COMPONENT_EXPRESSION_STORE_EXPR(int32_t, CCExpressionGetInteger(Expr));
                return TRUE;
            }
            
            else if (ExprType == CCExpressionValueTypeFloat)
            {
                CC_COMPONENT_EXPRESSION_STORE_EXPR(int32_t, CCExpressionGetFloat(Expr));
                return TRUE;
            }
            break;

        case CCComponentExpressionArgumentTypeInt64:
            if (ExprType == CCExpressionValueTypeInteger)
            {
                CC_COMPONENT_EXPRESSION_STORE_EXPR(int64_t, CCExpressionGetInteger(Expr));
                return TRUE;
            }
            
            else if (ExprType == CCExpressionValueTypeFloat)
            {
                CC_COMPONENT_EXPRESSION_STORE_EXPR(int64_t, CCExpressionGetFloat(Expr));
                return TRUE;
            }
            break;

        case CCComponentExpressionArgumentTypeFloat32:
            if (ExprType == CCExpressionValueTypeFloat)
            {
                CC_COMPONENT_EXPRESSION_STORE_EXPR(float, CCExpressionGetFloat(Expr));
                return TRUE;
            }
            
            else if (ExprType == CCExpressionValueTypeInteger)
            {
                CC_COMPONENT_EXPRESSION_STORE_EXPR(float, CCExpressionGetInteger(Expr));
                return TRUE;
            }
            break;

        case CCComponentExpressionArgumentTypeFloat64:
            if (ExprType == CCExpressionValueTypeFloat)
            {
                CC_COMPONENT_EXPRESSION_STORE_EXPR(double, CCExpressionGetFloat(Expr));
                return TRUE;
            }
            
            else if (ExprType == CCExpressionValueTypeInteger)
            {
                CC_COMPONENT_EXPRESSION_STORE_EXPR(double, CCExpressionGetInteger(Expr));
                return TRUE;
            }
            break;

        case CCComponentExpressionArgumentTypeVector2:
            if ((ExprType == CCExpressionValueTypeList) && (CCExpressionIsVector2(Expr)))
            {
                CC_COMPONENT_EXPRESSION_STORE_EXPR(CCVector2D, CCExpressionGetVector2(Expr));
                return TRUE;
            }
            break;

        case CCComponentExpressionArgumentTypeVector3:
            if ((ExprType == CCExpressionValueTypeList) && (CCExpressionIsVector3(Expr)))
            {
                CC_COMPONENT_EXPRESSION_STORE_EXPR(CCVector3D, CCExpressionGetVector3(Expr));
                return TRUE;
            }
            break;

        case CCComponentExpressionArgumentTypeVector4:
            if ((ExprType == CCExpressionValueTypeList) && (CCExpressionIsVector4(Expr)))
            {
                CC_COMPONENT_EXPRESSION_STORE_EXPR(CCVector4D, CCExpressionGetVector4(Expr));
                return TRUE;
            }
            break;

        case CCComponentExpressionArgumentTypeVector2i:
            if ((ExprType == CCExpressionValueTypeList) && (CCExpressionIsVector2i(Expr)))
            {
                CC_COMPONENT_EXPRESSION_STORE_EXPR(CCVector2Di, CCExpressionGetVector2i(Expr));
                return TRUE;
            }
            break;

        case CCComponentExpressionArgumentTypeVector3i:
            if ((ExprType == CCExpressionValueTypeList) && (CCExpressionIsVector3i(Expr)))
            {
                CC_COMPONENT_EXPRESSION_STORE_EXPR(CCVector3Di, CCExpressionGetVector3i(Expr));
                return TRUE;
            }
            break;

        case CCComponentExpressionArgumentTypeVector4i:
            if ((ExprType == CCExpressionValueTypeList) && (CCExpressionIsVector4i(Expr)))
            {
                CC_COMPONENT_EXPRESSION_STORE_EXPR(CCVector4Di, CCExpressionGetVector4i(Expr));
                return TRUE;
            }
            break;

        case CCComponentExpressionArgumentTypeColour:
            if ((ExprType == CCExpressionValueTypeList) && (CCExpressionIsColour(Expr)))
            {
                CC_COMPONENT_EXPRESSION_STORE_EXPR(CCColourRGBA, CCExpressionGetColour(Expr));
                return TRUE;
            }
            break;

        case CCComponentExpressionArgumentTypeString:
            if (ExprType == CCExpressionValueTypeString)
            {
                if ((Deserializer->setterType & CCComponentExpressionArgumentTypeOwnershipMask) == CCComponentExpressionArgumentTypeOwnershipTransfer) CCExpressionChangeOwnership(Expr, CCExpressionGetCopy(Expr), NULL);
                
                CC_COMPONENT_EXPRESSION_STORE_EXPR(CCString, CCExpressionGetString(Expr));
                return TRUE;
            }
            break;

        case CCComponentExpressionArgumentTypeData:
            if (Deserializer->serializedType != CCExpressionValueTypeUnspecified)
            {
                if ((Deserializer->setterType & CCComponentExpressionArgumentTypeOwnershipMask) == CCComponentExpressionArgumentTypeOwnershipTransfer) CCExpressionChangeOwnership(Expr, CCExpressionGetCopy(Expr), NULL);
                
                CC_COMPONENT_EXPRESSION_STORE_EXPR(void*, CCExpressionGetData(Expr));
                return TRUE;
            }
            break;

        case CCComponentExpressionArgumentTypeExpression:
            if ((Deserializer->setterType & CCComponentExpressionArgumentTypeOwnershipMask) == CCComponentExpressionArgumentTypeOwnershipTransfer) CCExpressionChangeOwnership(Expr, CCExpressionGetCopy(Expr), NULL);
            
            CC_COMPONENT_EXPRESSION_STORE_EXPR(CCExpression, Expr);
            return TRUE;

        default:
            break;
    }

    return FALSE;
}

static size_t CCComponentExpressionTypeSize[] = {
    0,
    sizeof(_Bool), //CCComponentExpressionArgumentTypeBool = 1,
    sizeof(uint8_t), //CCComponentExpressionArgumentTypeUInt8,
    sizeof(uint16_t), //CCComponentExpressionArgumentTypeUInt16,
    sizeof(uint32_t), //CCComponentExpressionArgumentTypeUInt32,
    sizeof(uint64_t), //CCComponentExpressionArgumentTypeUInt64,
    sizeof(int8_t), //CCComponentExpressionArgumentTypeInt8,
    sizeof(int16_t), //CCComponentExpressionArgumentTypeInt16,
    sizeof(int32_t), //CCComponentExpressionArgumentTypeInt32,
    sizeof(int64_t), //CCComponentExpressionArgumentTypeInt64,
    sizeof(float), //CCComponentExpressionArgumentTypeFloat32,
    sizeof(double), //CCComponentExpressionArgumentTypeFloat64,
    sizeof(CCVector2D), //CCComponentExpressionArgumentTypeVector2,
    sizeof(CCVector3D), //CCComponentExpressionArgumentTypeVector3,
    sizeof(CCVector4D), //CCComponentExpressionArgumentTypeVector4,
    sizeof(CCVector2Di), //CCComponentExpressionArgumentTypeVector2i,
    sizeof(CCVector3Di), //CCComponentExpressionArgumentTypeVector3i,
    sizeof(CCVector4Di), //CCComponentExpressionArgumentTypeVector4i,
    sizeof(CCColourRGBA), //CCComponentExpressionArgumentTypeColour,
    sizeof(CCString), //CCComponentExpressionArgumentTypeString,
    sizeof(void*), //CCComponentExpressionArgumentTypeData,
    sizeof(CCExpression), //CCComponentExpressionArgumentTypeExpression,
    sizeof(CCOrderedCollection), //CCComponentExpressionArgumentTypeTextAttribute,
};

_Static_assert((sizeof(CCComponentExpressionTypeSize) / sizeof(size_t)) == CCComponentExpressionArgumentTypeMax, "Must add the missing type entries");

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
                        
                        if (ArgCount == 2)
                        {
                            const CCComponentExpressionArgumentSetterType ContainerType = Deserializer[Loop].setterType & CCComponentExpressionArgumentTypeContainerMask;
                            if (ContainerType != CCComponentExpressionArgumentTypeContainerComponent)
                            {
                                if (ArgType == CCExpressionValueTypeList)
                                {
                                    if (Deserializer[Loop].serializedType != CCExpressionValueTypeUnspecified)
                                    {
                                        CC_COLLECTION_FOREACH(CCExpression, ElementExpr, CCExpressionGetList(ArgExpr))
                                        {
                                            const CCExpressionValueType ElementType = CCExpressionGetType(ElementExpr);
                                            if (ElementType != Deserializer[Loop].serializedType)
                                            {
                                                if (CCComponentExpressionDeserializeDeferredArgument(Component, Arg, Deferred)) return TRUE;
                                                
                                                CC_LOG_ERROR_CUSTOM("Serialized type (%u) does not match type (%u) for argument (%S) of component (%u)", ElementType, Deserializer[Loop].serializedType, Name, CCComponentGetID(Component));
                                                return FALSE;
                                            }
                                        }
                                    }
                                    
                                    const CCComponentExpressionArgumentSetterType Type = Deserializer[Loop].setterType & CCComponentExpressionArgumentTypeMask;
                                    
                                    void *Container = NULL, (*ContainerDestroy)(void*);
                                    switch (Deserializer[Loop].setterType & CCComponentExpressionArgumentTypeContainerMask)
                                    {
                                        case CCComponentExpressionArgumentTypeContainerOrderedCollection:
                                        {
                                            CCCollectionElementDestructor Destructor;
                                            switch (Type)
                                            {
                                                case CCComponentExpressionArgumentTypeBool:
                                                case CCComponentExpressionArgumentTypeUInt8:
                                                case CCComponentExpressionArgumentTypeUInt16:
                                                case CCComponentExpressionArgumentTypeUInt32:
                                                case CCComponentExpressionArgumentTypeUInt64:
                                                case CCComponentExpressionArgumentTypeInt8:
                                                case CCComponentExpressionArgumentTypeInt16:
                                                case CCComponentExpressionArgumentTypeInt32:
                                                case CCComponentExpressionArgumentTypeInt64:
                                                case CCComponentExpressionArgumentTypeFloat32:
                                                case CCComponentExpressionArgumentTypeFloat64:
                                                case CCComponentExpressionArgumentTypeVector2:
                                                case CCComponentExpressionArgumentTypeVector3:
                                                case CCComponentExpressionArgumentTypeVector4:
                                                case CCComponentExpressionArgumentTypeVector2i:
                                                case CCComponentExpressionArgumentTypeVector3i:
                                                case CCComponentExpressionArgumentTypeVector4i:
                                                case CCComponentExpressionArgumentTypeColour:
                                                    Destructor = NULL;
                                                    break;
                                                    
                                                case CCComponentExpressionArgumentTypeString:
                                                    Destructor = CCStringDestructorForCollection;
                                                    break;
                                                    
                                                case CCComponentExpressionArgumentTypeData:
                                                    Destructor = CCGenericDestructorForCollection;
                                                    break;

                                                default:
                                                    CC_LOG_ERROR_CUSTOM("Cannot deserialize type (%u) for argument (%S) of component (%u). Containers of this type are not supported.", Deserializer[Loop].serializedType, Name, CCComponentGetID(Component));
                                                    return FALSE;
                                            }
                                            
                                            Container = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintOrdered, CCComponentExpressionTypeSize[Type], Destructor);
                                            ContainerDestroy = (void(*)(void*))CCCollectionDestroy;
                                            break;
                                        }
                                            
                                        default:
                                            CC_LOG_ERROR_CUSTOM("Cannot deserialize type (%u) for argument (%S) of component (%u). Containers of this type are not supported.", Deserializer[Loop].serializedType, Name, CCComponentGetID(Component));
                                            return FALSE;
                                    }
                                    
                                    CCComponentExpressionArgumentDeserializer ContainerDeserializer = Deserializer[Loop];
                                    ContainerDeserializer.setterType = (ContainerDeserializer.setterType & ~CCComponentExpressionArgumentTypeOwnershipMask) | CCComponentExpressionArgumentTypeOwnershipTransfer;
                                    
                                    _Bool Deserialized = TRUE;
                                    CC_COLLECTION_FOREACH(CCExpression, ElementExpr, CCExpressionGetList(ArgExpr))
                                    {
                                        if (!CCComponentExpressionDeserializeExpression(&ContainerDeserializer, CCExpressionGetType(ElementExpr), ElementExpr, Container))
                                        {
                                            Deserialized = FALSE;
                                            break;
                                        }
                                    }
                                    
                                    if (Deserialized)
                                    {
                                         ((void(*)(CCComponent,void*))Deserializer[Loop].setter)(Component, Container);
                                        
                                        if ((Deserializer[Loop].setterType & CCComponentExpressionArgumentTypeOwnershipMask) == CCComponentExpressionArgumentTypeOwnershipRetain) ContainerDestroy(Container);
                                        return TRUE;
                                    }
                                    
                                    else
                                    {
                                        /*
                                         Handle manual cleanup if the container does not have an element destructor
                                         */
                                        
                                        ContainerDestroy(Container);
                                    }
                                }
                            }
                            
                            else
                            {
                                if (Deserializer[Loop].serializedType != CCExpressionValueTypeUnspecified)
                                {
                                    if (ArgType != Deserializer[Loop].serializedType)
                                    {
                                        if (CCComponentExpressionDeserializeDeferredArgument(Component, Arg, Deferred)) return TRUE;
                                        
                                        CC_LOG_ERROR_CUSTOM("Serialized type (%u) does not match type (%u) for argument (%S) of component (%u)", ArgType, Deserializer[Loop].serializedType, Name, CCComponentGetID(Component));
                                        return FALSE;
                                    }
                                }
                                
                                if (CCComponentExpressionDeserializeExpression(&Deserializer[Loop], ArgType, ArgExpr, Component)) return TRUE;
                            }
                        }
                        
                        else
                        {
                            if (Deserializer[Loop].serializedType != CCExpressionValueTypeUnspecified)
                            {
                                if (CCComponentExpressionDeserializeDeferredArgument(Component, Arg, Deferred)) return TRUE;
                                
                                CC_LOG_ERROR_CUSTOM("Expected serialized type (%u) for argument (%S) of component (%u) but arguments do not match", Deserializer[Loop].serializedType, Name, CCComponentGetID(Component));
                                return FALSE;
                            }
                            
                            switch (Deserializer[Loop].setterType & CCComponentExpressionArgumentTypeMask)
                            {
                                case CCComponentExpressionArgumentTypeVector2:
                                    if (CCExpressionIsNamedVector2(Arg))
                                    {
                                        ((void(*)(CCComponent,CCVector2D))Deserializer[Loop].setter)(Component, CCExpressionGetNamedVector2(Arg));
                                        return TRUE;
                                    }
                                    return FALSE;
                                    
                                case CCComponentExpressionArgumentTypeVector3:
                                    if (CCExpressionIsNamedVector3(Arg))
                                    {
                                        ((void(*)(CCComponent,CCVector3D))Deserializer[Loop].setter)(Component, CCExpressionGetNamedVector3(Arg));
                                        return TRUE;
                                    }
                                    return FALSE;
                                    
                                case CCComponentExpressionArgumentTypeVector4:
                                    if (CCExpressionIsNamedVector4(Arg))
                                    {
                                        ((void(*)(CCComponent,CCVector4D))Deserializer[Loop].setter)(Component, CCExpressionGetNamedVector4(Arg));
                                        return TRUE;
                                    }
                                    return FALSE;
                                    
                                case CCComponentExpressionArgumentTypeVector2i:
                                    if (CCExpressionIsNamedVector2i(Arg))
                                    {
                                        ((void(*)(CCComponent,CCVector2Di))Deserializer[Loop].setter)(Component, CCExpressionGetNamedVector2i(Arg));
                                        return TRUE;
                                    }
                                    return FALSE;
                                    
                                case CCComponentExpressionArgumentTypeVector3i:
                                    if (CCExpressionIsNamedVector3i(Arg))
                                    {
                                        ((void(*)(CCComponent,CCVector3Di))Deserializer[Loop].setter)(Component, CCExpressionGetNamedVector3i(Arg));
                                        return TRUE;
                                    }
                                    return FALSE;
                                    
                                case CCComponentExpressionArgumentTypeVector4i:
                                    if (CCExpressionIsNamedVector4i(Arg))
                                    {
                                        ((void(*)(CCComponent,CCVector4Di))Deserializer[Loop].setter)(Component, CCExpressionGetNamedVector4i(Arg));
                                        return TRUE;
                                    }
                                    return FALSE;
                                    
                                case CCComponentExpressionArgumentTypeColour:
                                    if (CCExpressionIsNamedColour(Arg))
                                    {
                                        ((void(*)(CCComponent,CCColourRGBA))Deserializer[Loop].setter)(Component, CCExpressionGetNamedColour(Arg));
                                        return TRUE;
                                    }
                                    return FALSE;
                                    
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
