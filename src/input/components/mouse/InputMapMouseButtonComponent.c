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

#define CC_QUICK_COMPILE
#include "InputMapMouseButtonComponent.h"
#include "ComponentExpressions.h"
#include "InputMapKeyboardComponent.h"

const CCString CCInputMapMouseButtonComponentName = CC_STRING("input_map_mouse_button");

const CCMouseButton CCInputMapMouseButtonComponentButtonAny = CCMouseButtonUnknown;

static const CCComponentExpressionDescriptor CCInputMapMouseButtonComponentDescriptor = {
    .id = CC_INPUT_MAP_MOUSE_BUTTON_COMPONENT_ID,
    .initialize = NULL,
    .deserialize = CCInputMapMouseButtonComponentDeserializer,
    .serialize = NULL
};

static const struct {
    CCString name;
    CCMouseButton button;
} NamedButtons[] = {
    { .name = CC_STRING(":left"), .button = CCMouseButtonLeft },
    { .name = CC_STRING(":right"), .button = CCMouseButtonRight },
    { .name = CC_STRING(":middle"), .button = CCMouseButtonMiddle },
    { .name = CC_STRING(":btn-1"), .button = CCMouseButton1 },
    { .name = CC_STRING(":btn-2"), .button = CCMouseButton2 },
    { .name = CC_STRING(":btn-3"), .button = CCMouseButton3 },
    { .name = CC_STRING(":btn-4"), .button = CCMouseButton4 },
    { .name = CC_STRING(":btn-5"), .button = CCMouseButton5 },
    { .name = CC_STRING(":btn-6"), .button = CCMouseButton6 },
    { .name = CC_STRING(":btn-7"), .button = CCMouseButton7 },
    { .name = CC_STRING(":btn-8"), .button = CCMouseButton8 },
    { .name = CC_STRING(":any"), .button = CCInputMapMouseButtonComponentButtonAny }
};

static CCDictionary(CCString, CCMouseButton) Buttons = NULL;

const CCDictionary(CCString, CCMouseButton) * const CCInputMapMouseButtonComponentButtonAtoms = &Buttons;

void CCInputMapMouseButtonComponentRegister(void)
{
    CCComponentRegister(CC_INPUT_MAP_MOUSE_BUTTON_COMPONENT_ID, CCInputMapMouseButtonComponentName, CC_STD_ALLOCATOR, sizeof(CCInputMapMouseButtonComponentClass), CCInputMapMouseButtonComponentInitialize, NULL, CCInputMapMouseButtonComponentDeallocate);
    
    CCComponentExpressionRegister(CC_STRING("input-mouse-button"), &CCInputMapMouseButtonComponentDescriptor, TRUE);
    
    
    Buttons = CCDictionaryCreate(CC_STD_ALLOCATOR, CCDictionaryHintHeavyFinding | CCDictionaryHintConstantLength | CCDictionaryHintConstantElements, sizeof(CCString), sizeof(CCMouseButton), &(CCDictionaryCallbacks){
        .getHash = CCStringHasherForDictionary,
        .compareKeys = CCStringComparatorForDictionary
    });
    
    for (size_t Loop = 0; Loop < sizeof(NamedButtons) / sizeof(typeof(*NamedButtons)); Loop++)
    {
        CCDictionarySetValue(Buttons, &NamedButtons[Loop].name, &NamedButtons[Loop].button);
    }
}

void CCInputMapMouseButtonComponentDeregister(void)
{
    CCComponentDeregister(CC_INPUT_MAP_MOUSE_BUTTON_COMPONENT_ID);
}

static CCComponentExpressionArgumentDeserializer Arguments[] = {
    { .name = CC_STRING("ramp:"), .serializedType = CCExpressionValueTypeUnspecified, .setterType = CCComponentExpressionArgumentTypeFloat32, .setter = (CCComponentExpressionSetter)CCInputMapMouseButtonComponentSetRamp }
};

void CCInputMapMouseButtonComponentDeserializer(CCComponent Component, CCExpression Arg, _Bool Deferred)
{
    CCInputMapMouseButtonComponentSetIgnoreModifier(Component, TRUE);
    
    if (CCExpressionGetType(Arg) == CCExpressionValueTypeList)
    {
        const size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Arg));
        if (CCCollectionGetCount(CCExpressionGetList(Arg)) >= 2)
        {
            CCExpression NameExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 0);
            if (CCExpressionGetType(NameExpr) == CCExpressionValueTypeAtom)
            {
                CCString Name = CCExpressionGetAtom(NameExpr);
                if (CCStringEqual(Name, CC_STRING("button:")))
                {
                    if (ArgCount == 2)
                    {
                        CCExpression Button = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 1);
                        if (CCExpressionGetType(Button) == CCExpressionValueTypeAtom)
                        {
                            CCString Code = CCExpressionGetAtom(Button);
                            
                            CCMouseButton *Value = CCDictionaryGetValue(Buttons, &Code);
                            if (Value)
                            {
                                CCInputMapMouseButtonComponentSetButton(Component, *Value);
                            }
                            
                            else if (!CCComponentExpressionDeserializeDeferredArgument(Component, Arg, Deferred)) CC_LOG_ERROR_CUSTOM("Value (:%S) for argument (button:) is not a valid atom", Code);
                        }
                        
                        else if (!CCComponentExpressionDeserializeDeferredArgument(Component, Arg, Deferred)) CC_LOG_ERROR("Expect value for argument (button:) to be an atom");
                    }
                    
                    else if (!CCComponentExpressionDeserializeDeferredArgument(Component, Arg, Deferred)) CC_LOG_ERROR("Expect value for argument (button:) to be an atom");
                    
                    return;
                }
                
                else if (CCStringEqual(Name, CC_STRING("flags:")))
                {
                    CCKeyboardModifier Flags = 0;
                    
                    CCEnumerator Enumerator;
                    CCCollectionGetEnumerator(CCExpressionGetList(Arg), &Enumerator);
                    
                    _Bool FlagsValid = TRUE;
                    for (CCExpression *Expr = CCCollectionEnumeratorNext(&Enumerator); Expr; Expr = CCCollectionEnumeratorNext(&Enumerator))
                    {
                        if (CCExpressionGetType(*Expr) == CCExpressionValueTypeAtom)
                        {
                            CCString Flag = CCExpressionGetAtom(*Expr);
                            CCKeyboardModifier *Value = CCDictionaryGetValue(*CCInputMapKeyboardComponentModifierAtoms, &Flag);
                            if (Value) Flags |= *Value;
                            else if (!Deferred) CC_LOG_ERROR_CUSTOM("Value (:%S) for argument (flags:) is not a valid atom", Flag);
                            else FlagsValid = FALSE;
                        }
                        
                        else if (CCExpressionGetType(*Expr) == CCExpressionValueTypeList)
                        {
                            CC_COLLECTION_FOREACH(CCExpression, MapExpr, CCExpressionGetList(*Expr))
                            {
                                if (CCExpressionGetType(MapExpr) == CCExpressionValueTypeAtom)
                                {
                                    CCString Flag = CCExpressionGetAtom(*Expr);
                                    CCKeyboardModifier *Value = CCDictionaryGetValue(*CCInputMapKeyboardComponentModifierAtoms, &Flag);
                                    if (Value) Flags |= *Value;
                                    else if (!Deferred) CC_LOG_ERROR_CUSTOM("Value (:%S) for argument (flags:) is not a valid atom", Flag);
                                    else FlagsValid = FALSE;
                                }
                            }
                        }
                    }
                    
                    if (!FlagsValid) CCComponentExpressionDeserializeDeferredArgument(Component, Arg, Deferred);
                    
                    CCInputMapMouseButtonComponentSetFlags(Component, Flags);
                    CCInputMapMouseButtonComponentSetIgnoreModifier(Component, FALSE);
                    
                    return;
                }
            }
        }
    }
    
    if (!CCComponentExpressionDeserializeArgument(Component, Arg, Arguments, sizeof(Arguments) / sizeof(typeof(*Arguments)), Deferred))
    {
        CCInputMapComponentDeserializer(Component, Arg, Deferred);
    }
}
