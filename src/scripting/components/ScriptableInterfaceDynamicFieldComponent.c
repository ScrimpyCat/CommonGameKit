/*
 *  Copyright (c) 2018, Stefan Johnson
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
#include "ScriptableInterfaceDynamicFieldComponent.h"
#include "ComponentExpressions.h"

const CCString CCScriptableInterfaceDynamicFieldComponentName = CC_STRING("dynamic_field");

static const CCComponentExpressionDescriptor CCScriptableInterfaceDynamicFieldComponentDescriptor = {
    .id = CC_SCRIPTABLE_INTERFACE_DYNAMIC_FIELD_COMPONENT_ID,
    .initialize = NULL,
    .deserialize = CCScriptableInterfaceDynamicFieldComponentDeserializer,
    .serialize = NULL
};

void CCScriptableInterfaceDynamicFieldComponentRegister(void)
{
    CCComponentRegister(CC_SCRIPTABLE_INTERFACE_DYNAMIC_FIELD_COMPONENT_ID, CCScriptableInterfaceDynamicFieldComponentName, CC_STD_ALLOCATOR, sizeof(CCScriptableInterfaceDynamicFieldComponentClass), CCScriptableInterfaceDynamicFieldComponentInitialize, NULL, CCScriptableInterfaceDynamicFieldComponentDeallocate);
    CCComponentRegister(CC_SCRIPTABLE_INTERFACE_DYNAMIC_FIELD_COMPONENT_ID | CCScriptableInterfaceTypeTemporaryFlag, (CCString)NULL, CC_STD_ALLOCATOR, sizeof(CCScriptableInterfaceDynamicFieldComponentClass), CCScriptableInterfaceDynamicFieldComponentInitialize, NULL, CCScriptableInterfaceDynamicFieldComponentDeallocate);
    
    CCComponentExpressionRegister(CC_STRING("dynamic-field"), &CCScriptableInterfaceDynamicFieldComponentDescriptor, TRUE);
}

void CCScriptableInterfaceDynamicFieldComponentDeregister(void)
{
    CCComponentDeregister(CC_SCRIPTABLE_INTERFACE_DYNAMIC_FIELD_COMPONENT_ID);
    CCComponentDeregister(CC_SCRIPTABLE_INTERFACE_DYNAMIC_FIELD_COMPONENT_ID | CCScriptableInterfaceTypeTemporaryFlag);
}

static CCComponentExpressionArgumentDeserializer Arguments[] = {
    { .name = CC_STRING("target:"), .serializedType = CCComponentExpressionValueTypeComponent, .setterType = CCComponentExpressionArgumentTypeData | CCComponentExpressionArgumentTypeOwnershipRetain, .setter = CCScriptableInterfaceDynamicFieldComponentSetTarget },
    { .name = CC_STRING("field:"), .serializedType = CCExpressionValueTypeUnspecified, .setterType = CCComponentExpressionArgumentTypeExpression, .setter = CCScriptableInterfaceDynamicFieldComponentSetField }
};

void CCScriptableInterfaceDynamicFieldComponentDeserializer(CCComponent Component, CCExpression Arg, _Bool Deferred)
{
    if (!CCScriptableInterfaceDynamicFieldComponentGetReferenceState(Component))
    {
        CCExpression State = CCExpressionStateGetSuper(Arg);
        if (State) CCScriptableInterfaceDynamicFieldComponentSetReferenceState(Component, State);
    }
    
    CCComponentExpressionDeserializeArgument(Component, Arg, Arguments, sizeof(Arguments) / sizeof(typeof(*Arguments)), Deferred);
}
