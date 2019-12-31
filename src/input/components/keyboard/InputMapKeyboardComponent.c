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
#include "InputMapKeyboardComponent.h"
#include "ComponentExpressions.h"

const CCString CCInputMapKeyboardComponentName = CC_STRING("input_map_keyboard");

const CCKeyboardKeycode CCInputMapKeyboardComponentKeycodeAny = CCKeyboardKeycodeUnknown;

static const CCComponentExpressionDescriptor CCInputMapKeyboardComponentDescriptor = {
    .id = CC_INPUT_MAP_KEYBOARD_COMPONENT_ID,
    .initialize = NULL,
    .deserialize = CCInputMapKeyboardComponentDeserializer,
    .serialize = NULL
};

static const struct {
    CCString key;
    CCKeyboardKeycode code;
} NamedKeys[] = {
    { .key = CC_STRING(":space"), .code = CCKeyboardKeycodeSpace },
    { .key = CC_STRING(":apostrophe"), .code = CCKeyboardKeycodeApostrophe },
    { .key = CC_STRING(":comma"), .code = CCKeyboardKeycodeComma },
    { .key = CC_STRING(":minus"), .code = CCKeyboardKeycodeMinus },
    { .key = CC_STRING(":period"), .code = CCKeyboardKeycodePeriod },
    { .key = CC_STRING(":slash"), .code = CCKeyboardKeycodeSlash },
    { .key = CC_STRING(":0"), .code = CCKeyboardKeycode0 },
    { .key = CC_STRING(":1"), .code = CCKeyboardKeycode1 },
    { .key = CC_STRING(":2"), .code = CCKeyboardKeycode2 },
    { .key = CC_STRING(":3"), .code = CCKeyboardKeycode3 },
    { .key = CC_STRING(":4"), .code = CCKeyboardKeycode4 },
    { .key = CC_STRING(":5"), .code = CCKeyboardKeycode5 },
    { .key = CC_STRING(":6"), .code = CCKeyboardKeycode6 },
    { .key = CC_STRING(":7"), .code = CCKeyboardKeycode7 },
    { .key = CC_STRING(":8"), .code = CCKeyboardKeycode8 },
    { .key = CC_STRING(":9"), .code = CCKeyboardKeycode9 },
    { .key = CC_STRING(":semicolon"), .code = CCKeyboardKeycodeSemicolon },
    { .key = CC_STRING(":equal"), .code = CCKeyboardKeycodeEqual },
    { .key = CC_STRING(":a"), .code = CCKeyboardKeycodeA },
    { .key = CC_STRING(":b"), .code = CCKeyboardKeycodeB },
    { .key = CC_STRING(":c"), .code = CCKeyboardKeycodeC },
    { .key = CC_STRING(":d"), .code = CCKeyboardKeycodeD },
    { .key = CC_STRING(":e"), .code = CCKeyboardKeycodeE },
    { .key = CC_STRING(":f"), .code = CCKeyboardKeycodeF },
    { .key = CC_STRING(":g"), .code = CCKeyboardKeycodeG },
    { .key = CC_STRING(":h"), .code = CCKeyboardKeycodeH },
    { .key = CC_STRING(":i"), .code = CCKeyboardKeycodeI },
    { .key = CC_STRING(":j"), .code = CCKeyboardKeycodeJ },
    { .key = CC_STRING(":k"), .code = CCKeyboardKeycodeK },
    { .key = CC_STRING(":l"), .code = CCKeyboardKeycodeL },
    { .key = CC_STRING(":m"), .code = CCKeyboardKeycodeM },
    { .key = CC_STRING(":n"), .code = CCKeyboardKeycodeN },
    { .key = CC_STRING(":o"), .code = CCKeyboardKeycodeO },
    { .key = CC_STRING(":p"), .code = CCKeyboardKeycodeP },
    { .key = CC_STRING(":q"), .code = CCKeyboardKeycodeQ },
    { .key = CC_STRING(":r"), .code = CCKeyboardKeycodeR },
    { .key = CC_STRING(":s"), .code = CCKeyboardKeycodeS },
    { .key = CC_STRING(":t"), .code = CCKeyboardKeycodeT },
    { .key = CC_STRING(":u"), .code = CCKeyboardKeycodeU },
    { .key = CC_STRING(":v"), .code = CCKeyboardKeycodeV },
    { .key = CC_STRING(":w"), .code = CCKeyboardKeycodeW },
    { .key = CC_STRING(":x"), .code = CCKeyboardKeycodeX },
    { .key = CC_STRING(":y"), .code = CCKeyboardKeycodeY },
    { .key = CC_STRING(":z"), .code = CCKeyboardKeycodeZ },
    { .key = CC_STRING(":left-bracket"), .code = CCKeyboardKeycodeLeftBracket },
    { .key = CC_STRING(":backslash"), .code = CCKeyboardKeycodeBackslash },
    { .key = CC_STRING(":right-bracket"), .code = CCKeyboardKeycodeRightBracket },
    { .key = CC_STRING(":grave-accent"), .code = CCKeyboardKeycodeGraveAccent },
    { .key = CC_STRING(":world-1"), .code = CCKeyboardKeycodeWorld1 },
    { .key = CC_STRING(":world-2"), .code = CCKeyboardKeycodeWorld2 },
    { .key = CC_STRING(":escape"), .code = CCKeyboardKeycodeEscape },
    { .key = CC_STRING(":enter"), .code = CCKeyboardKeycodeEnter },
    { .key = CC_STRING(":tab"), .code = CCKeyboardKeycodeTab },
    { .key = CC_STRING(":backspace"), .code = CCKeyboardKeycodeBackspace },
    { .key = CC_STRING(":insert"), .code = CCKeyboardKeycodeInsert },
    { .key = CC_STRING(":delete"), .code = CCKeyboardKeycodeDelete },
    { .key = CC_STRING(":right"), .code = CCKeyboardKeycodeRight },
    { .key = CC_STRING(":left"), .code = CCKeyboardKeycodeLeft },
    { .key = CC_STRING(":down"), .code = CCKeyboardKeycodeDown },
    { .key = CC_STRING(":up"), .code = CCKeyboardKeycodeUp },
    { .key = CC_STRING(":page-up"), .code = CCKeyboardKeycodePageUp },
    { .key = CC_STRING(":page-down"), .code = CCKeyboardKeycodePageDown },
    { .key = CC_STRING(":home"), .code = CCKeyboardKeycodeHome },
    { .key = CC_STRING(":end"), .code = CCKeyboardKeycodeEnd },
    { .key = CC_STRING(":caps-lock"), .code = CCKeyboardKeycodeCapsLock },
    { .key = CC_STRING(":scroll-lock"), .code = CCKeyboardKeycodeScrollLock },
    { .key = CC_STRING(":num-lock"), .code = CCKeyboardKeycodeNumLock },
    { .key = CC_STRING(":print-screen"), .code = CCKeyboardKeycodePrintScreen },
    { .key = CC_STRING(":pause"), .code = CCKeyboardKeycodePause },
    { .key = CC_STRING(":f1"), .code = CCKeyboardKeycodeF1 },
    { .key = CC_STRING(":f2"), .code = CCKeyboardKeycodeF2 },
    { .key = CC_STRING(":f3"), .code = CCKeyboardKeycodeF3 },
    { .key = CC_STRING(":f4"), .code = CCKeyboardKeycodeF4 },
    { .key = CC_STRING(":f5"), .code = CCKeyboardKeycodeF5 },
    { .key = CC_STRING(":f6"), .code = CCKeyboardKeycodeF6 },
    { .key = CC_STRING(":f7"), .code = CCKeyboardKeycodeF7 },
    { .key = CC_STRING(":f8"), .code = CCKeyboardKeycodeF8 },
    { .key = CC_STRING(":f9"), .code = CCKeyboardKeycodeF9 },
    { .key = CC_STRING(":f10"), .code = CCKeyboardKeycodeF10 },
    { .key = CC_STRING(":f11"), .code = CCKeyboardKeycodeF11 },
    { .key = CC_STRING(":f12"), .code = CCKeyboardKeycodeF12 },
    { .key = CC_STRING(":f13"), .code = CCKeyboardKeycodeF13 },
    { .key = CC_STRING(":f14"), .code = CCKeyboardKeycodeF14 },
    { .key = CC_STRING(":f15"), .code = CCKeyboardKeycodeF15 },
    { .key = CC_STRING(":f16"), .code = CCKeyboardKeycodeF16 },
    { .key = CC_STRING(":f17"), .code = CCKeyboardKeycodeF17 },
    { .key = CC_STRING(":f18"), .code = CCKeyboardKeycodeF18 },
    { .key = CC_STRING(":f19"), .code = CCKeyboardKeycodeF19 },
    { .key = CC_STRING(":f20"), .code = CCKeyboardKeycodeF20 },
    { .key = CC_STRING(":f21"), .code = CCKeyboardKeycodeF21 },
    { .key = CC_STRING(":f22"), .code = CCKeyboardKeycodeF22 },
    { .key = CC_STRING(":f23"), .code = CCKeyboardKeycodeF23 },
    { .key = CC_STRING(":f24"), .code = CCKeyboardKeycodeF24 },
    { .key = CC_STRING(":f25"), .code = CCKeyboardKeycodeF25 },
    { .key = CC_STRING(":kp-0"), .code = CCKeyboardKeycodeKeypad0 },
    { .key = CC_STRING(":kp-1"), .code = CCKeyboardKeycodeKeypad1 },
    { .key = CC_STRING(":kp-2"), .code = CCKeyboardKeycodeKeypad2 },
    { .key = CC_STRING(":kp-3"), .code = CCKeyboardKeycodeKeypad3 },
    { .key = CC_STRING(":kp-4"), .code = CCKeyboardKeycodeKeypad4 },
    { .key = CC_STRING(":kp-5"), .code = CCKeyboardKeycodeKeypad5 },
    { .key = CC_STRING(":kp-6"), .code = CCKeyboardKeycodeKeypad6 },
    { .key = CC_STRING(":kp-7"), .code = CCKeyboardKeycodeKeypad7 },
    { .key = CC_STRING(":kp-8"), .code = CCKeyboardKeycodeKeypad8 },
    { .key = CC_STRING(":kp-9"), .code = CCKeyboardKeycodeKeypad9 },
    { .key = CC_STRING(":kp-decimal"), .code = CCKeyboardKeycodeKeypadDecimal },
    { .key = CC_STRING(":kp-divide"), .code = CCKeyboardKeycodeKeypadDivide },
    { .key = CC_STRING(":kp-multiply"), .code = CCKeyboardKeycodeKeypadMultiply },
    { .key = CC_STRING(":kp-subtract"), .code = CCKeyboardKeycodeKeypadSubtract },
    { .key = CC_STRING(":kp-add"), .code = CCKeyboardKeycodeKeypadAdd },
    { .key = CC_STRING(":kp-enter"), .code = CCKeyboardKeycodeKeypadEnter },
    { .key = CC_STRING(":kp-equal"), .code = CCKeyboardKeycodeKeypadEqual },
    { .key = CC_STRING(":left-shift"), .code = CCKeyboardKeycodeLeftShift },
    { .key = CC_STRING(":left-control"), .code = CCKeyboardKeycodeLeftControl },
    { .key = CC_STRING(":left-alt"), .code = CCKeyboardKeycodeLeftAlt },
    { .key = CC_STRING(":left-super"), .code = CCKeyboardKeycodeLeftSuper },
    { .key = CC_STRING(":right-shift"), .code = CCKeyboardKeycodeRightShift },
    { .key = CC_STRING(":right-control"), .code = CCKeyboardKeycodeRightControl },
    { .key = CC_STRING(":right-alt"), .code = CCKeyboardKeycodeRightAlt },
    { .key = CC_STRING(":right-super"), .code = CCKeyboardKeycodeRightSuper },
    { .key = CC_STRING(":menu"), .code = CCKeyboardKeycodeMenu },
    { .key = CC_STRING(":any"), .code = CCInputMapKeyboardComponentKeycodeAny }
};

static const struct {
    CCString modifier;
    CCKeyboardModifier flag;
} NamedModifiers[] = {
    { .modifier = CC_STRING(":none"), .flag = 0 },
    { .modifier = CC_STRING(":shift"), .flag = CCKeyboardModifierShift },
    { .modifier = CC_STRING(":control"), .flag = CCKeyboardModifierControl },
    { .modifier = CC_STRING(":alt"), .flag = CCKeyboardModifierAlt },
    { .modifier = CC_STRING(":super"), .flag = CCKeyboardModifierSuper }
};

static CCDictionary(CCString, CCKeyboardKeycode) Keycodes = NULL;
static CCDictionary(CCString, CCKeyboardModifier) Modifiers = NULL;

const CCDictionary(CCString, CCKeyboardKeycode) * const CCInputMapKeyboardComponentKeycodeAtoms = &Keycodes;
const CCDictionary(CCString, CCKeyboardModifier) * const CCInputMapKeyboardComponentModifierAtoms = &Modifiers;

void CCInputMapKeyboardComponentRegister(void)
{
    CCComponentRegister(CC_INPUT_MAP_KEYBOARD_COMPONENT_ID, CCInputMapKeyboardComponentName, CC_STD_ALLOCATOR, sizeof(CCInputMapKeyboardComponentClass), CCInputMapKeyboardComponentInitialize, NULL, CCInputMapKeyboardComponentDeallocate);
    
    CCComponentExpressionRegister(CC_STRING("input-keyboard"), &CCInputMapKeyboardComponentDescriptor, TRUE);
    
    
    Keycodes = CCDictionaryCreate(CC_STD_ALLOCATOR, CCDictionaryHintHeavyFinding | CCDictionaryHintConstantLength | CCDictionaryHintConstantElements, sizeof(CCString), sizeof(CCKeyboardKeycode), &(CCDictionaryCallbacks){
        .getHash = CCStringHasherForDictionary,
        .compareKeys = CCStringComparatorForDictionary
    });
    
    for (size_t Loop = 0; Loop < sizeof(NamedKeys) / sizeof(typeof(*NamedKeys)); Loop++)
    {
        CCDictionarySetValue(Keycodes, &NamedKeys[Loop].key, &NamedKeys[Loop].code);
    }
    
    
    Modifiers = CCDictionaryCreate(CC_STD_ALLOCATOR, CCDictionaryHintHeavyFinding | CCDictionaryHintConstantLength | CCDictionaryHintConstantElements, sizeof(CCString), sizeof(CCKeyboardModifier), &(CCDictionaryCallbacks){
        .getHash = CCStringHasherForDictionary,
        .compareKeys = CCStringComparatorForDictionary
    });
    
    for (size_t Loop = 0; Loop < sizeof(NamedModifiers) / sizeof(typeof(*NamedModifiers)); Loop++)
    {
        CCDictionarySetValue(Modifiers, &NamedModifiers[Loop].modifier, &NamedModifiers[Loop].flag);
    }
}

void CCInputMapKeyboardComponentDeregister(void)
{
    CCComponentDeregister(CC_INPUT_MAP_KEYBOARD_COMPONENT_ID);
}

static CCComponentExpressionArgumentDeserializer Arguments[] = {
    { .name = CC_STRING("ramp:"), .serializedType = CCExpressionValueTypeUnspecified, .setterType = CCComponentExpressionArgumentTypeFloat32, .setter = (CCComponentExpressionSetter)CCInputMapKeyboardComponentSetRamp },
    { .name = CC_STRING("repeat:"), .serializedType = CCExpressionValueTypeUnspecified, .setterType = CCComponentExpressionArgumentTypeBool, .setter = (CCComponentExpressionSetter)CCInputMapKeyboardComponentSetRepeats },
};

void CCInputMapKeyboardComponentDeserializer(CCComponent Component, CCExpression Arg, _Bool Deferred)
{
    CCInputMapKeyboardComponentSetIgnoreModifier(Component, TRUE);
    
    if (CCExpressionGetType(Arg) == CCExpressionValueTypeList)
    {
        const size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Arg));
        if (CCCollectionGetCount(CCExpressionGetList(Arg)) >= 2)
        {
            CCExpression NameExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 0);
            if (CCExpressionGetType(NameExpr) == CCExpressionValueTypeAtom)
            {
                CCString Name = CCExpressionGetAtom(NameExpr);
                if (CCStringEqual(Name, CC_STRING("keycode:")))
                {
                    if (ArgCount == 2)
                    {
                        CCExpression Keycode = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 1);
                        if (CCExpressionGetType(Keycode) == CCExpressionValueTypeAtom)
                        {
                            CCString Code = CCExpressionGetAtom(Keycode);
                            
                            CCKeyboardKeycode *Value = CCDictionaryGetValue(Keycodes, &Code);
                            if (Value)
                            {
                                CCInputMapKeyboardComponentSetKeycode(Component, *Value);
                                CCInputMapKeyboardComponentSetIsKeycode(Component, TRUE);
                            }
                            
                            else if (!CCComponentExpressionDeserializeDeferredArgument(Component, Arg, Deferred)) CC_LOG_ERROR_CUSTOM("Value (:%S) for argument (keycode:) is not a valid atom", Code);
                        }
                        
                        else if (!CCComponentExpressionDeserializeDeferredArgument(Component, Arg, Deferred)) CC_LOG_ERROR("Expect value for argument (keycode:) to be an atom");
                    }
                    
                    else if (!CCComponentExpressionDeserializeDeferredArgument(Component, Arg, Deferred)) CC_LOG_ERROR("Expect value for argument (keycode:) to be an atom");
                    
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
                            CCKeyboardModifier *Value = CCDictionaryGetValue(Modifiers, &Flag);
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
                                    CCKeyboardModifier *Value = CCDictionaryGetValue(Modifiers, &Flag);
                                    if (Value) Flags |= *Value;
                                    else if (!Deferred) CC_LOG_ERROR_CUSTOM("Value (:%S) for argument (flags:) is not a valid atom", Flag);
                                    else FlagsValid = FALSE;
                                }
                            }
                        }
                    }
                    
                    if (!FlagsValid) CCComponentExpressionDeserializeDeferredArgument(Component, Arg, Deferred);
                    
                    CCInputMapKeyboardComponentSetFlags(Component, Flags);
                    CCInputMapKeyboardComponentSetIgnoreModifier(Component, FALSE);
                    
                    return;
                }
                
                else if (CCStringEqual(Name, CC_STRING("char:")))
                {
                    if (ArgCount == 2)
                    {
                        CCExpression Char = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 1);
                        if ((CCExpressionGetType(Char) == CCExpressionValueTypeString) && (CCStringGetLength(CCExpressionGetString(Char)) == 1))
                        {
                            CCInputMapKeyboardComponentSetCharacter(Component, CCStringGetCharacterAtIndex(CCExpressionGetString(Char), 0));
                            CCInputMapKeyboardComponentSetIsKeycode(Component, FALSE);
                        }
                        
                        else if (!CCComponentExpressionDeserializeDeferredArgument(Component, Arg, Deferred)) CC_LOG_ERROR("Expect value for argument (char:) to be a single character string");
                    }
                    
                    else if (!CCComponentExpressionDeserializeDeferredArgument(Component, Arg, Deferred)) CC_LOG_ERROR("Expect value for argument (char:) to be a single character string");
                    
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
