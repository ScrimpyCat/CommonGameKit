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

#include "AnimationComponent.h"
#include "ComponentExpressions.h"

static CCComponentExpressionArgumentDeserializer Arguments[] = {
    { .name = CC_STRING("name:"), .serializedType = CCExpressionValueTypeUnspecified, .setterType = CCComponentExpressionArgumentTypeString, .setter = CCAnimationComponentSetName },
    { .name = CC_STRING("speed:"), .serializedType = CCExpressionValueTypeUnspecified, .setterType = CCComponentExpressionArgumentTypeFloat32, .setter = (CCComponentExpressionSetter)CCAnimationComponentSetSpeed },
    { .name = CC_STRING("position:"), .serializedType = CCExpressionValueTypeUnspecified, .setterType = CCComponentExpressionArgumentTypeFloat64, .setter = (CCComponentExpressionSetter)CCAnimationComponentSetPosition },
    { .name = CC_STRING("playing:"), .serializedType = CCExpressionValueTypeUnspecified, .setterType = CCComponentExpressionArgumentTypeBool, .setter = (CCComponentExpressionSetter)CCAnimationComponentSetPlaying }
};

void CCAnimationComponentDeserializer(CCComponent Component, CCExpression Arg, _Bool Deferred)
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
                if (CCStringEqual(Name, CC_STRING("loop:")))
                {
                    if (ArgCount == 2)
                    {
                        CCExpression LoopExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 1);
                        if (CCExpressionGetType(LoopExpr) == CCExpressionValueTypeAtom)
                        {
                            CCString LoopType = CCExpressionGetAtom(LoopExpr);
                            if (CCStringEqual(LoopType, CC_STRING(":once"))) CCAnimationComponentSetLoop(Component, CCAnimationLoopOnce);
                            if (CCStringEqual(LoopType, CC_STRING(":roll"))) CCAnimationComponentSetLoop(Component, CCAnimationLoopRepeatRoll);
                            if (CCStringEqual(LoopType, CC_STRING(":flip"))) CCAnimationComponentSetLoop(Component, CCAnimationLoopRepeatFlip);
                            else if (!CCComponentExpressionDeserializeDeferredArgument(Component, Arg, Deferred)) CC_LOG_ERROR_CUSTOM("Value (:%S) for argument (loop:) is not a valid atom", LoopType);
                        }
                        
                        else if (!CCComponentExpressionDeserializeDeferredArgument(Component, Arg, Deferred)) CC_LOG_ERROR("Expect value for argument (loop:) to be an atom");
                    }
                    
                    else if (!CCComponentExpressionDeserializeDeferredArgument(Component, Arg, Deferred)) CC_LOG_ERROR("Expect value for argument (loop:) to be an atom");
                    
                    return;
                }
            }
        }
    }
    
    CCComponentExpressionDeserializeArgument(Component, Arg, Arguments, sizeof(Arguments) / sizeof(typeof(*Arguments)), Deferred);
}
