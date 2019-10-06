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
#include "InputMapComponent.h"
#include "ComponentExpressions.h"

CC_DICTIONARY_DECLARE(CCString, CCInputMapComponentCallback);

static CCComponentExpressionArgumentDeserializer Arguments[] = {
    { .name = CC_STRING("action:"), .serializedType = CCExpressionValueTypeUnspecified, .setterType = CCComponentExpressionArgumentTypeString, .setter = CCInputMapComponentSetAction }
};

static CCDictionary(CCString, CCInputMapComponentCallback) Callbacks[CCInputMapTypeCount];
void CCInputMapComponentDeserializer(CCComponent Component, CCExpression Arg, _Bool Deferred)
{
    if (CCExpressionGetType(Arg) == CCExpressionValueTypeList)
    {
        const size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Arg));
        if (CCCollectionGetCount(CCExpressionGetList(Arg)) >= 2)
        {
            CCExpression NameExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 0);
            if (CCExpressionGetType(NameExpr) == CCExpressionValueTypeAtom)
            {
                CCString Name = CCExpressionGetAtom(NameExpr);
                if (CCStringEqual(Name, CC_STRING("callback:")))
                {
                    if (ArgCount == 2)
                    {
                        CCExpression CallbackNameExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 1);
                        if (CCExpressionGetType(CallbackNameExpr) == CCExpressionValueTypeAtom)
                        {
                            CCString CallbackName = CCExpressionGetAtom(CallbackNameExpr);
                            
                            CCInputMapComponentCallback *Callback = Callbacks[CCComponentGetID(Component) & CCInputMapTypeMask] ? CCDictionaryGetValue(Callbacks[CCComponentGetID(Component) & CCInputMapTypeMask], &CallbackName) : NULL;
                            if (Callback)
                            {
                                CCInputMapComponentSetCallback(Component, *Callback);
                            }
                            
                            else if (!CCComponentExpressionDeserializeDeferredArgument(Component, Arg, Deferred)) CC_LOG_ERROR_CUSTOM("Value (:%S) for argument (callback:) is not a valid atom", CallbackName);
                        }
                        
                        else if (!CCComponentExpressionDeserializeDeferredArgument(Component, Arg, Deferred)) CC_LOG_ERROR("Expect value for argument (callback:) to be an atom");
                    }
                    
                    else if (!CCComponentExpressionDeserializeDeferredArgument(Component, Arg, Deferred)) CC_LOG_ERROR("Expect value for argument (callback:) to be an atom");
                    
                    return;
                }
            }
        }
    }
    
    CCComponentExpressionDeserializeArgument(Component, Arg, Arguments, sizeof(Arguments) / sizeof(typeof(*Arguments)), Deferred);
}

void CCInputMapComponentRegisterCallback(CCString Name, CCInputMapType Type, CCInputMapComponentCallback Callback)
{
    if (!Callbacks[Type])
    {
        Callbacks[Type] = CCDictionaryCreate(CC_STD_ALLOCATOR, CCDictionaryHintHeavyFinding | CCDictionaryHintConstantLength | CCDictionaryHintConstantElements, sizeof(CCString), sizeof(Callback), &(CCDictionaryCallbacks){
            .keyDestructor = CCStringDestructorForDictionary,
            .getHash = CCStringHasherForDictionary,
            .compareKeys = CCStringComparatorForDictionary
        });
    }
    
    CCDictionarySetValue(Callbacks[Type], &(CCString){ CCStringCopy(Name) }, &Callback);
}
