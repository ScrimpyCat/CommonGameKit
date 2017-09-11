/*
 *  Copyright (c) 2016, Stefan Johnson
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

#ifndef CommonGameKit_CommonGameKit_h
#define CommonGameKit_CommonGameKit_h

#include <CommonC/Common.h>

#include <CommonGameKit/Callbacks.h>
#include <CommonGameKit/Window.h>

#include <CommonGameKit/AnimationComponent.h>
#include <CommonGameKit/AnimationInterpolateComponent.h>
#include <CommonGameKit/AnimationKeyframeComponent.h>
#include <CommonGameKit/AnimationSystem.h>

#include <CommonGameKit/Component.h>
#include <CommonGameKit/ComponentBase.h>
#include <CommonGameKit/ComponentSystem.h>
#include <CommonGameKit/Entity.h>
#include <CommonGameKit/EntityManager.h>
#include <CommonGameKit/Message.h>

#include <CommonGameKit/GUIExpression.h>
#include <CommonGameKit/GUIManager.h>
#include <CommonGameKit/GUIObject.h>

#include <CommonGameKit/Controller.h>
#include <CommonGameKit/Keyboard.h>
#include <CommonGameKit/Mouse.h>
#include <CommonGameKit/InputMapComponent.h>
#include <CommonGameKit/InputMapGroupComponent.h>
#include <CommonGameKit/InputMapControllerAxesComponent.h>
#include <CommonGameKit/InputMapControllerButtonComponent.h>
#include <CommonGameKit/InputMapControllerComponent.h>
#include <CommonGameKit/InputMapKeyboardComponent.h>
#include <CommonGameKit/InputMapMouseButtonComponent.h>
#include <CommonGameKit/InputMapMouseDropComponent.h>
#include <CommonGameKit/InputMapMousePositionComponent.h>
#include <CommonGameKit/InputMapMouseScrollComponent.h>
#include <CommonGameKit/InputSystem.h>

#include <CommonGameKit/RenderComponent.h>
#include <CommonGameKit/RenderSystem.h>

#include <CommonGameKit/RelationParentComponent.h>

#include <CommonGameKit/SpatialTransformComponent.h>

#include <CommonGameKit/Colour.h>
#include <CommonGameKit/ColourComponent.h>
#include <CommonGameKit/ColourFormat.h>

#include <CommonGameKit/PixelData.h>
#include <CommonGameKit/PixelDataFile.h>
#include <CommonGameKit/PixelDataGenerator.h>
#include <CommonGameKit/PixelDataStatic.h>

#include <CommonGameKit/GFX.h>
#include <CommonGameKit/GLGFX.h>

#include <CommonGameKit/Expression.h>
#include <CommonGameKit/ExpressionEvaluator.h>
#include <CommonGameKit/ExpressionHelpers.h>

//TODO: generate header for these
#include <CommonGameKit/AssetExpressions.h>
#include <CommonGameKit/BitwiseExpressions.h>
#include <CommonGameKit/ColourExpressions.h>
#include <CommonGameKit/ComponentExpressions.h>
#include <CommonGameKit/ControlFlowExpressions.h>
#include <CommonGameKit/DebugExpressions.h>
#include <CommonGameKit/EntityExpressions.h>
#include <CommonGameKit/EqualityExpressions.h>
#include <CommonGameKit/GraphicsExpressions.h>
#include <CommonGameKit/IOExpressions.h>
#include <CommonGameKit/ListExpressions.h>
#include <CommonGameKit/MacroExpressions.h>
#include <CommonGameKit/MathExpressions.h>
#include <CommonGameKit/MessageExpressions.h>
#include <CommonGameKit/StateExpressions.h>
#include <CommonGameKit/StringExpressions.h>
#include <CommonGameKit/TextExpressions.h>
#include <CommonGameKit/TypeCastExpressions.h>
#include <CommonGameKit/WindowExpressions.h>

#include <CommonGameKit/AssetManager.h>

#include <CommonGameKit/Font.h>
#include <CommonGameKit/Text.h>
#include <CommonGameKit/TextAttribute.h>

#endif
