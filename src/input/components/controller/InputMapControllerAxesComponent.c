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

#include "InputMapControllerAxesComponent.h"
#include "ComponentExpressions.h"

const CCString CCInputMapControllerAxesComponentName = CC_STRING("input_map_controller_axes");

static const CCComponentExpressionDescriptor CCInputMapControllerAxesComponentDescriptor = {
    .id = CC_INPUT_MAP_CONTROLLER_AXES_COMPONENT_ID,
    .initialize = NULL,
    .deserialize = CCInputMapControllerAxesComponentDeserializer,
    .serialize = NULL
};

void CCInputMapControllerAxesComponentRegister(void)
{
    CCComponentRegister(CC_INPUT_MAP_CONTROLLER_AXES_COMPONENT_ID, CCInputMapControllerAxesComponentName, CC_STD_ALLOCATOR, sizeof(CCInputMapControllerAxesComponentClass), CCInputMapControllerAxesComponentInitialize, NULL, CCInputMapControllerAxesComponentDeallocate);
    
    CCComponentExpressionRegister(CC_STRING("input-controller-axes"), &CCInputMapControllerAxesComponentDescriptor, TRUE);
}

void CCInputMapControllerAxesComponentDeregister(void)
{
    CCComponentDeregister(CC_INPUT_MAP_CONTROLLER_AXES_COMPONENT_ID);
}

static CCComponentExpressionArgumentDeserializer Arguments[] = {
    { .name = CC_STRING("deadzone:"), .serializedType = CCExpressionValueTypeUnspecified, .setterType = CCComponentExpressionArgumentTypeFloat32, .setter = (CCComponentExpressionSetter)CCInputMapControllerAxesComponentSetDeadzone },
    { .name = CC_STRING("resolution:"), .serializedType = CCExpressionValueTypeUnspecified, .setterType = CCComponentExpressionArgumentTypeUInt8, .setter = (CCComponentExpressionSetter)CCInputMapControllerAxesComponentSetResolution }
};

void CCInputMapControllerAxesComponentDeserializer(CCComponent Component, CCExpression Arg)
{
    if (CCExpressionGetType(Arg) == CCExpressionValueTypeList)
    {
        if (CCCollectionGetCount(CCExpressionGetList(Arg)) >= 2)
        {
            CCExpression NameExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 0);
            if (CCExpressionGetType(NameExpr) == CCExpressionValueTypeAtom)
            {
                static const struct {
                    void (*identifier)(CCComponent, int32_t);
                    void (*oneSided)(CCComponent, _Bool);
                    void (*flip)(CCComponent, _Bool);
                } Axes[3] = {
                    { .identifier = CCInputMapControllerAxesComponentSetX, .oneSided = CCInputMapControllerAxesComponentSetOneSidedX, .flip = CCInputMapControllerAxesComponentSetFlipX },
                    { .identifier = CCInputMapControllerAxesComponentSetY, .oneSided = CCInputMapControllerAxesComponentSetOneSidedY, .flip = CCInputMapControllerAxesComponentSetFlipY },
                    { .identifier = CCInputMapControllerAxesComponentSetZ, .oneSided = CCInputMapControllerAxesComponentSetOneSidedZ, .flip = CCInputMapControllerAxesComponentSetFlipZ }
                };
                
                size_t Axis = SIZE_MAX;
                CCString Name = CCExpressionGetAtom(NameExpr);
                
                if (CCStringEqual(Name, CC_STRING("x:"))) Axis = 0;
                else if (CCStringEqual(Name, CC_STRING("y:"))) Axis = 1;
                else if (CCStringEqual(Name, CC_STRING("z:"))) Axis = 2;
                
                if (Axis != SIZE_MAX)
                {
                    CCEnumerator Enumerator;
                    CCCollectionGetEnumerator(CCExpressionGetList(Arg), &Enumerator);
                    
                    CCExpression *IDExpr = CCCollectionEnumeratorNext(&Enumerator);
                    if ((IDExpr) && (CCExpressionGetType(*IDExpr) == CCExpressionValueTypeInteger))
                    {
                        Axes[Axis].identifier(Component, CCExpressionGetInteger(*IDExpr));
                        
                        for (CCExpression *Expr = CCCollectionEnumeratorNext(&Enumerator); Expr; Expr = CCCollectionEnumeratorNext(&Enumerator))
                        {
                            if (CCExpressionGetType(*Expr) == CCExpressionValueTypeAtom)
                            {
                                CCString Flag = CCExpressionGetString(*Expr);
                                if (CCStringEqual(Flag, CC_STRING("one-sided")))
                                {
                                    Axes[Axis].oneSided(Component, TRUE);
                                }
                                
                                else if (CCStringEqual(Flag, CC_STRING("flip")))
                                {
                                    Axes[Axis].flip(Component, TRUE);
                                }
                                
                                else CC_LOG_ERROR_CUSTOM("Flag (%S) for argument (%S) is not a valid atom", Flag, Name);
                            }
                            
                            else CC_LOG_ERROR_CUSTOM("Flag for argument (%S) is not an atom", Name);
                        }
                    }
                    
                    else CC_LOG_ERROR_CUSTOM("Identifier value for argument (%S) is not an integer", Name);
                    
                    return;
                }
            }
        }
    }
    
    if (!CCComponentExpressionDeserializeArgument(Component, Arg, Arguments, sizeof(Arguments) / sizeof(typeof(*Arguments))))
    {
        CCInputMapControllerComponentDeserializer(Component, Arg);
    }
}
