/*
 *  Copyright (c) 2019, Stefan Johnson
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

#ifndef CommonGameKit_Base_h
#define CommonGameKit_Base_h

#define CC_CUSTOM_COMMON_GAME_KIT_TYPES(func, ...) \
func(__VA_ARGS__, CCAnimationType); \
func(__VA_ARGS__, CCAnimationLoop); \
func(__VA_ARGS__, CCAnimationKeyframeDirection); \
func(__VA_ARGS__, CCComponentSystemID); \
func(__VA_ARGS__, CCComponentSystemExecutionType); \
func(__VA_ARGS__, CCMessageID); \
func(__VA_ARGS__, CCMessage); \
func(__VA_ARGS__, CCMessageRouter); \
func(__VA_ARGS__, CCComponentID); \
func(__VA_ARGS__, CCComponent); \
func(__VA_ARGS__, CCEntity); \
func(__VA_ARGS__, ECSComponentRefs); \
func(__VA_ARGS__, ECSEntity); \
func(__VA_ARGS__, GUIObject); \
func(__VA_ARGS__, GUIEvent); \
func(__VA_ARGS__, CCInputMapType); \
func(__VA_ARGS__, CCInputState); \
func(__VA_ARGS__, CCControllerEvent); \
func(__VA_ARGS__, CCControllerButtonState); \
func(__VA_ARGS__, CCControllerAxesState); \
func(__VA_ARGS__, CCControllerState); \
func(__VA_ARGS__, CCKeyboardAction); \
func(__VA_ARGS__, CCKeyboardModifier); \
func(__VA_ARGS__, CCKeyboardKeycode); \
func(__VA_ARGS__, CCKeyboardState); \
func(__VA_ARGS__, CCKeyboardMap); \
func(__VA_ARGS__, CCMouseButton); \
func(__VA_ARGS__, CCMouseEvent); \
func(__VA_ARGS__, CCMouseButtonState); \
func(__VA_ARGS__, CCMouseButtonMap); \
func(__VA_ARGS__, CCMouseMoveMap); \
func(__VA_ARGS__, CCMouseScrollMap); \
func(__VA_ARGS__, CCMouseDropItemsMap); \
func(__VA_ARGS__, CCMouseMap); \
func(__VA_ARGS__, CCRelationType); \
func(__VA_ARGS__, CCRenderType); \
func(__VA_ARGS__, CCColour); \
func(__VA_ARGS__, CCColourComponent); \
func(__VA_ARGS__, CCColourFormat); \
func(__VA_ARGS__, CCColourFormatHSV); \
func(__VA_ARGS__, CCColourFormatRGB); \
func(__VA_ARGS__, CCPixelData); \
func(__VA_ARGS__, GFXBlend); \
func(__VA_ARGS__, GFXViewport); \
func(__VA_ARGS__, GFXBlitFramebuffer); \
func(__VA_ARGS__, GFXBlit); \
func(__VA_ARGS__, GFXBufferHint); \
func(__VA_ARGS__, GFXBuffer); \
func(__VA_ARGS__, GFXBufferFormat); \
func(__VA_ARGS__, GFXPrimitiveType); \
func(__VA_ARGS__, GFXDrawInput); \
func(__VA_ARGS__, GFXDrawInputVertexBuffer); \
func(__VA_ARGS__, GFXDrawInputBuffer); \
func(__VA_ARGS__, GFXDrawInputTexture); \
func(__VA_ARGS__, GFXDrawIndexBuffer); \
func(__VA_ARGS__, GFXDrawDestination); \
func(__VA_ARGS__, GFXDraw); \
func(__VA_ARGS__, GFXFramebufferAttachmentAction); \
func(__VA_ARGS__, GFXFramebufferAttachmentType); \
func(__VA_ARGS__, GFXFramebufferAttachment); \
func(__VA_ARGS__, GFXFramebuffer); \
func(__VA_ARGS__, GFXShaderInput); \
func(__VA_ARGS__, GFXShader); \
func(__VA_ARGS__, GFXShaderSourceType); \
func(__VA_ARGS__, GFXShaderLibrary); \
func(__VA_ARGS__, GFXShaderSource); \
func(__VA_ARGS__, GFXTextureHint); \
func(__VA_ARGS__, GFXTexture); \
func(__VA_ARGS__, GFXTextureStream); \
func(__VA_ARGS__, GFXCommandBuffer); \
func(__VA_ARGS__, CCExpressionValueType); \
func(__VA_ARGS__, CCExpressionAtomType); \
func(__VA_ARGS__, CCExpression); \
func(__VA_ARGS__, CCScriptableInterfaceType); \
func(__VA_ARGS__, CCFontStyle); \
func(__VA_ARGS__, CCFontCharMap); \
func(__VA_ARGS__, CCFontGlyph); \
func(__VA_ARGS__, CCFontAttribute); \
func(__VA_ARGS__, CCFont); \
func(__VA_ARGS__, CCTextAlignment); \
func(__VA_ARGS__, CCTextDrawable); \
func(__VA_ARGS__, CCText); \
func(__VA_ARGS__, CCTextVisibility); \
func(__VA_ARGS__, CCTextAttribute); \
func(__VA_ARGS__, CC_CONTAINER(CCOrderedCollection, CCTextAttribute)); \
func(__VA_ARGS__, CC_CONTAINER(CCCollection, CCTextAttribute));

#define CC_CUSTOM_COMMON_GAME_KIT_TYPES_(func, ...) \
func(__VA_ARGS__, CCAnimationType); \
func(__VA_ARGS__, CCAnimationLoop); \
func(__VA_ARGS__, CCAnimationKeyframeDirection); \
func(__VA_ARGS__, CCComponentSystemID); \
func(__VA_ARGS__, CCComponentSystemExecutionType); \
func(__VA_ARGS__, CCMessageID); \
func(__VA_ARGS__, CCMessage); \
func(__VA_ARGS__, CCMessageRouter); \
func(__VA_ARGS__, CCComponentID); \
func(__VA_ARGS__, CCComponent); \
func(__VA_ARGS__, CCEntity); \
func(__VA_ARGS__, ECSComponentRefs); \
func(__VA_ARGS__, ECSEntity); \
func(__VA_ARGS__, GUIObject); \
func(__VA_ARGS__, GUIEvent); \
func(__VA_ARGS__, CCInputMapType); \
func(__VA_ARGS__, CCInputState); \
func(__VA_ARGS__, CCControllerEvent); \
func(__VA_ARGS__, CCControllerButtonState); \
func(__VA_ARGS__, CCControllerAxesState); \
func(__VA_ARGS__, CCControllerState); \
func(__VA_ARGS__, CCKeyboardAction); \
func(__VA_ARGS__, CCKeyboardModifier); \
func(__VA_ARGS__, CCKeyboardKeycode); \
func(__VA_ARGS__, CCKeyboardState); \
func(__VA_ARGS__, CCKeyboardMap); \
func(__VA_ARGS__, CCMouseButton); \
func(__VA_ARGS__, CCMouseEvent); \
func(__VA_ARGS__, CCMouseButtonState); \
func(__VA_ARGS__, CCMouseButtonMap); \
func(__VA_ARGS__, CCMouseMoveMap); \
func(__VA_ARGS__, CCMouseScrollMap); \
func(__VA_ARGS__, CCMouseDropItemsMap); \
func(__VA_ARGS__, CCMouseMap); \
func(__VA_ARGS__, CCRelationType); \
func(__VA_ARGS__, CCRenderType); \
func(__VA_ARGS__, CCColour); \
func(__VA_ARGS__, CCColourComponent); \
func(__VA_ARGS__, CCColourFormat); \
func(__VA_ARGS__, CCColourFormatHSV); \
func(__VA_ARGS__, CCColourFormatRGB); \
func(__VA_ARGS__, CCPixelData); \
func(__VA_ARGS__, GFXBlend); \
func(__VA_ARGS__, GFXViewport); \
func(__VA_ARGS__, GFXBlitFramebuffer); \
func(__VA_ARGS__, GFXBlit); \
func(__VA_ARGS__, GFXBufferHint); \
func(__VA_ARGS__, GFXBuffer); \
func(__VA_ARGS__, GFXBufferFormat); \
func(__VA_ARGS__, GFXPrimitiveType); \
func(__VA_ARGS__, GFXDrawInput); \
func(__VA_ARGS__, GFXDrawInputVertexBuffer); \
func(__VA_ARGS__, GFXDrawInputBuffer); \
func(__VA_ARGS__, GFXDrawInputTexture); \
func(__VA_ARGS__, GFXDrawIndexBuffer); \
func(__VA_ARGS__, GFXDrawDestination); \
func(__VA_ARGS__, GFXDraw); \
func(__VA_ARGS__, GFXFramebufferAttachmentAction); \
func(__VA_ARGS__, GFXFramebufferAttachmentType); \
func(__VA_ARGS__, GFXFramebufferAttachment); \
func(__VA_ARGS__, GFXFramebuffer); \
func(__VA_ARGS__, GFXShaderInput); \
func(__VA_ARGS__, GFXShader); \
func(__VA_ARGS__, GFXShaderSourceType); \
func(__VA_ARGS__, GFXShaderLibrary); \
func(__VA_ARGS__, GFXShaderSource); \
func(__VA_ARGS__, GFXTextureHint); \
func(__VA_ARGS__, GFXTexture); \
func(__VA_ARGS__, GFXTextureStream); \
func(__VA_ARGS__, GFXCommandBuffer); \
func(__VA_ARGS__, CCExpressionValueType); \
func(__VA_ARGS__, CCExpressionAtomType); \
func(__VA_ARGS__, CCExpression); \
func(__VA_ARGS__, CCScriptableInterfaceType); \
func(__VA_ARGS__, CCFontStyle); \
func(__VA_ARGS__, CCFontCharMap); \
func(__VA_ARGS__, CCFontGlyph); \
func(__VA_ARGS__, CCFontAttribute); \
func(__VA_ARGS__, CCFont); \
func(__VA_ARGS__, CCTextAlignment); \
func(__VA_ARGS__, CCTextDrawable); \
func(__VA_ARGS__, CCText); \
func(__VA_ARGS__, CCTextVisibility); \
func(__VA_ARGS__, CCTextAttribute); \
func(__VA_ARGS__, CC_CONTAINER(CCOrderedCollection, CCTextAttribute)); \
func(__VA_ARGS__, CC_CONTAINER(CCCollection, CCTextAttribute));

#ifndef CC_CUSTOM_TYPES
#define CC_CUSTOM_TYPES CC_CUSTOM_COMMON_GAME_KIT_TYPES
#endif

#ifndef CC_CUSTOM_TYPES_
#define CC_CUSTOM_TYPES_ CC_CUSTOM_COMMON_GAME_KIT_TYPES_
#endif

#include <CommonC/Common.h>

#endif
