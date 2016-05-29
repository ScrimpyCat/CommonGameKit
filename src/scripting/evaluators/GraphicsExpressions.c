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

#include "GraphicsExpressions.h"
#include "ExpressionHelpers.h"
#include "AssetManager.h"
#include "AssetExpressions.h"

typedef struct {
    CCVector2D position;
    CCColourRGBA colour;
    CCVector2D coord;
} CCGraphicsExpressionRectVertData;

typedef struct {
    GFXBuffer vertBuffer;
    GFXBuffer scaleBuffer;
    GFXBuffer radiusBuffer;
    CCRect rect;
    CCColourRGBA colour;
    float radius;
} CCGraphicsExpressionRenderRectArgumentState;


static void CCGraphicsExpressionDrawValueDestructor(CCGraphicsExpressionValueDraw *Ptr)
{
    GFXDrawDestroy(Ptr->drawer);
}

static void CCGraphicsExpressionRenderRectArgumentStateValueDestructor(CCGraphicsExpressionRenderRectArgumentState *Ptr)
{
    GFXBufferDestroy(Ptr->vertBuffer);
    GFXBufferDestroy(Ptr->radiusBuffer);
    GFXBufferDestroy(Ptr->scaleBuffer);
}

static CCString StrDrawer = CC_STRING(".drawer"), StrArgs = CC_STRING(".args");

CCExpression CCGraphicsExpressionRenderRect(CCExpression Expression)
{
    CCExpression Result = Expression;
    
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) >= 3)
    {
        CCEnumerator Enumerator;
        CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
        
        CCExpression *ArgRect = CCCollectionEnumeratorNext(&Enumerator);
        CCExpression *ArgColour = CCCollectionEnumeratorNext(&Enumerator);
        CCExpression *ArgRadius = CCCollectionEnumeratorNext(&Enumerator);
        
        const CCRect Rect = CCExpressionGetRect(CCExpressionEvaluate(*ArgRect));
        const CCColourRGBA Colour = CCExpressionGetColour(CCExpressionEvaluate(*ArgColour));
        const float Radius = ArgRadius ? CCExpressionGetFloat(CCExpressionEvaluate(*ArgRadius)) : 0.0f;
        
        
        CCExpression PreservedDraw = CCExpressionGetStateStrict(Expression, StrDrawer);
        if (PreservedDraw)
        {
            Result = CCExpressionCopy(PreservedDraw);
            
            _Bool UpdateScale = TRUE;
            CCGraphicsExpressionRenderRectArgumentState *Args = CCExpressionGetData(CCExpressionGetStateStrict(Expression, StrArgs));
            if ((Args->rect.position.x != Rect.position.x) ||
                (Args->rect.position.y != Rect.position.y) ||
                (Args->rect.size.x != Rect.size.x) ||
                (Args->rect.size.y != Rect.size.y) ||
                ((UpdateScale = FALSE)) ||
                (Args->colour.r != Colour.r) ||
                (Args->colour.g != Colour.g) ||
                (Args->colour.b != Colour.b) ||
                (Args->colour.a != Colour.a))
            {
                Args->rect = Rect;
                Args->colour = Colour;
                
                GFXBufferWriteBuffer(Args->vertBuffer, 0, sizeof(CCGraphicsExpressionRectVertData) * 4, (CCGraphicsExpressionRectVertData[4]){
                    { .position = Rect.position, .colour = Colour, .coord = CCVector2DMake(0.0f, 0.0f) },
                    { .position = CCVector2Add(Rect.position, CCVector2DMake(Rect.size.x, 0.0f)), .colour = Colour, .coord = CCVector2DMake(1.0f, 0.0f) },
                    { .position = CCVector2Add(Rect.position, CCVector2DMake(0.0f, Rect.size.y)), .colour = Colour, .coord = CCVector2DMake(0.0f, 1.0f) },
                    { .position = CCVector2Add(Rect.position, Rect.size), .colour = Colour, .coord = CCVector2DMake(1.0f, 1.0f) }
                });
                
                if (UpdateScale) GFXBufferWriteBuffer(Args->scaleBuffer, 0, sizeof(CCVector2D), (Rect.size.x < Rect.size.y ? &CCVector2DMake(Rect.size.x / Rect.size.y, 1.0f) : &CCVector2DMake(1.0f, Rect.size.y / Rect.size.x)));
            }
            
            if (Args->radius != Radius)
            {
                Args->radius = Radius;
                
                GFXBufferWriteBuffer(Args->radiusBuffer, 0, sizeof(float), &Radius);
                
                GFXDrawSetBlending(((CCGraphicsExpressionValueDraw*)CCExpressionGetData(PreservedDraw))->drawer, Radius != 0.0f ? GFXBlendTransparent : GFXBlendOpaque);
            }
        }
        
        else
        {
            GFXShader Shader = CCAssetManagerCreateShader(CC_STRING("rounded-rect"));
            
            GFXBuffer ScaleBuffer = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCVector2D), (Rect.size.x < Rect.size.y ? &CCVector2DMake(Rect.size.x / Rect.size.y, 1.0f) : &CCVector2DMake(1.0f, Rect.size.y / Rect.size.x)));
            GFXBuffer RadiusBuffer = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(float), &Radius);
            
            GFXBuffer VertBuffer = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintDataVertex | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCGraphicsExpressionRectVertData) * 4, (CCGraphicsExpressionRectVertData[4]){
                { .position = Rect.position, .colour = Colour, .coord = CCVector2DMake(0.0f, 0.0f) },
                { .position = CCVector2Add(Rect.position, CCVector2DMake(Rect.size.x, 0.0f)), .colour = Colour, .coord = CCVector2DMake(1.0f, 0.0f) },
                { .position = CCVector2Add(Rect.position, CCVector2DMake(0.0f, Rect.size.y)), .colour = Colour, .coord = CCVector2DMake(0.0f, 1.0f) },
                { .position = CCVector2Add(Rect.position, Rect.size), .colour = Colour, .coord = CCVector2DMake(1.0f, 1.0f) }
            });
            
            GFXDraw Drawer = GFXDrawCreate(CC_STD_ALLOCATOR);
            GFXDrawSetShader(Drawer, Shader);
            GFXDrawSetVertexBuffer(Drawer, "vPosition", VertBuffer, GFXBufferFormatFloat32x2, sizeof(CCGraphicsExpressionRectVertData), offsetof(CCGraphicsExpressionRectVertData, position));
            GFXDrawSetVertexBuffer(Drawer, "vColour", VertBuffer, GFXBufferFormatFloat32x3, sizeof(CCGraphicsExpressionRectVertData), offsetof(CCGraphicsExpressionRectVertData, colour));
            GFXDrawSetVertexBuffer(Drawer, "vCoord", VertBuffer, GFXBufferFormatFloat32x2, sizeof(CCGraphicsExpressionRectVertData), offsetof(CCGraphicsExpressionRectVertData, coord));
            GFXDrawSetBuffer(Drawer, "scale", ScaleBuffer);
            GFXDrawSetBuffer(Drawer, "radius", RadiusBuffer);
            GFXDrawSetBlending(Drawer, Radius != 0.0f ? GFXBlendTransparent : GFXBlendOpaque);
            
            GFXShaderDestroy(Shader);
            
            
            CCGraphicsExpressionValueDraw *Drawable;
            CC_SAFE_Malloc(Drawable, sizeof(CCGraphicsExpressionValueDraw),
                           CC_LOG_ERROR("Failed to create CCGraphicsExpressionValueDraw due to allocation failure. Allocation size (%zu)", sizeof(CCGraphicsExpressionValueDraw));
                           GFXDrawDestroy(Drawer);
                           return Result;
                           );
            
            CCMemorySetDestructor(Drawable, (CCMemoryDestructorCallback)CCGraphicsExpressionDrawValueDestructor);
            
            Drawable->drawer = Drawer;
            Drawable->vertices = 4;
            
            Result = CCExpressionCreateCustomType(CC_STD_ALLOCATOR, CCGraphicsExpressionValueTypeDraw, Drawable, CCExpressionRetainedValueCopy, CCFree);
            
            
            CCGraphicsExpressionRenderRectArgumentState *Args;
            CC_SAFE_Malloc(Args, sizeof(CCGraphicsExpressionRenderRectArgumentState),
                           CC_LOG_ERROR("Failed to create CCGraphicsExpressionRenderRectArgumentState due to allocation failure. Allocation size (%zu)", sizeof(CCGraphicsExpressionRenderRectArgumentState));
                           GFXBufferDestroy(VertBuffer);
                           GFXBufferDestroy(RadiusBuffer);
                           GFXBufferDestroy(ScaleBuffer);
                           return Result;
                           );
            
            CCMemorySetDestructor(Args, (CCMemoryDestructorCallback)CCGraphicsExpressionRenderRectArgumentStateValueDestructor);
            
            *Args = (CCGraphicsExpressionRenderRectArgumentState){
                .vertBuffer = VertBuffer,
                .scaleBuffer = ScaleBuffer,
                .radiusBuffer = RadiusBuffer,
                .rect = Rect,
                .colour = Colour,
                .radius = Radius
            };
            
            
            CCExpressionCreateState(Expression, StrDrawer, Result, TRUE);
            CCExpressionCreateState(Expression, StrArgs, CCExpressionCreateCustomType(CC_STD_ALLOCATOR, CCExpressionValueTypeUnspecified, Args, CCExpressionRetainedValueCopy, CCFree), TRUE);
        }
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("render-rect", "rect:list colour:list [radius:float]");
    
    return Result;
}

static CCTextAttribute CCGraphicsExpressionLoadAttributedString(CCExpression String, CCEnumerator *Enumerator)
{
    CCTextAttribute Attribute = {
        .string = CCStringCopy(CCExpressionGetString(String)),
        .font = NULL,
        .colour = CCVector4DMake(0.309f, 0.247f, 0.239f, 1.0f),
        .scale = CCVector2DFill(-0.77f),
        .space = -0.25f,
        .softness = 0.3f,
        .thickness = 0.45f
    };
    
    for (CCExpression *Arg = CCCollectionEnumeratorNext(Enumerator); Arg; Arg = CCCollectionEnumeratorNext(Enumerator))
    {
        if (CCExpressionGetType(*Arg) == CCExpressionValueTypeList)
        {
            size_t Count = CCCollectionGetCount(CCExpressionGetList(*Arg));
            if (Count > 1)
            {
                CCExpression Type = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(*Arg), 0);
                if (CCExpressionGetType(Type) == CCExpressionValueTypeAtom)
                {
                    //TODO: workout naming conventions to stop collisions
                    if (CCStringEqual(CCExpressionGetString(Type), CC_STRING("colour:"))) Attribute.colour = CCExpressionGetNamedColour(*Arg);
                    else if (CCStringEqual(CCExpressionGetString(Type), CC_STRING("scale:"))) Attribute.scale = CCExpressionGetNamedVector2(*Arg);
                    else if (CCStringEqual(CCExpressionGetString(Type), CC_STRING("offset:"))) Attribute.offset = CCExpressionGetNamedVector2(*Arg);
                    else if (CCStringEqual(CCExpressionGetString(Type), CC_STRING("tilt:"))) Attribute.tilt = CCExpressionGetNamedVector2(*Arg);
                    else if (CCStringEqual(CCExpressionGetString(Type), CC_STRING("space:"))) Attribute.space = CCExpressionGetNamedFloat(*Arg);
                    else if (CCStringEqual(CCExpressionGetString(Type), CC_STRING("softness:"))) Attribute.softness = CCExpressionGetNamedFloat(*Arg);
                    else if (CCStringEqual(CCExpressionGetString(Type), CC_STRING("thickness:"))) Attribute.thickness = CCExpressionGetNamedFloat(*Arg);
                }
            }
        }
        
        else if (CCExpressionGetType(*Arg) == CCAssetExpressionValueTypeFont)
        {
            Attribute.font = CCRetain(CCExpressionGetData(*Arg));
        }
    }
    
    if (!Attribute.font) Attribute.font = CCAssetManagerCreateFont(CC_STRING("Arial"));
    
    return Attribute;
}

CCExpression CCGraphicsExpressionRenderText(CCExpression Expression)
{
    CCExpression Result = Expression;
    
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) >= 2)
    {
        CCEnumerator Enumerator;
        CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
        
        CCExpression *ArgRect = CCCollectionEnumeratorNext(&Enumerator);
        
        size_t Offset = 0, Length = SIZE_MAX;
        CCTextAlignment Alignment = CCTextAlignmentLeft;
        CCTextVisibility Visibility = CCTextVisibilityMultiLine | CCTextVisibilityWord;
        CCOrderedCollection AttributedStrings = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintOrdered | CCCollectionHintHeavyEnumerating, sizeof(CCTextAttribute), CCTextAttributeDestructorForCollection);
        
        for (CCExpression *Arg = CCCollectionEnumeratorNext(&Enumerator); Arg; Arg = CCCollectionEnumeratorNext(&Enumerator))
        {
            CCExpression ArgText = CCExpressionEvaluate(*Arg);
            if (CCExpressionGetType(ArgText) == CCExpressionValueTypeList)
            {
                if (CCCollectionGetCount(CCExpressionGetList(ArgText)) >= 1)
                {
                    CCEnumerator Enumerator;
                    CCCollectionGetEnumerator(CCExpressionGetList(ArgText), &Enumerator);
                    
                    CCExpression *ArgString = CCCollectionEnumeratorGetCurrent(&Enumerator);
                    if (CCExpressionGetType(*ArgString) == CCExpressionValueTypeList)
                    {
                        for (CCExpression *Text = ArgString; Text; Text = CCCollectionEnumeratorNext(&Enumerator))
                        {
                            if (CCExpressionGetType(*Text) == CCExpressionValueTypeList)
                            {
                                CCEnumerator StringEnumerator;
                                CCCollectionGetEnumerator(CCExpressionGetList(*Text), &StringEnumerator);
                                
                                CCExpression *Option = CCCollectionEnumeratorGetCurrent(&StringEnumerator);
                                if ((Option) && (CCExpressionGetType(*Option) == CCExpressionValueTypeAtom) && (CCStringEqual(CCExpressionGetAtom(*Option), CC_STRING("text:"))))
                                {
                                    CCExpression *String = CCCollectionEnumeratorNext(&StringEnumerator);
                                    if (CCExpressionGetType(*String) == CCExpressionValueTypeString)
                                    {
                                        CCTextAttribute Attribute = CCGraphicsExpressionLoadAttributedString(*String, &StringEnumerator);
                                        CCOrderedCollectionAppendElement(AttributedStrings, &Attribute);
                                    }
                                }
                            }
                        }
                    }
                    
                    else if (CCExpressionGetType(*ArgString) == CCExpressionValueTypeAtom)
                    {
                        if (CCStringEqual(CCExpressionGetAtom(*ArgString), CC_STRING("text:")))
                        {
                            ArgString = CCCollectionEnumeratorNext(&Enumerator);
                            
                            if (CCExpressionGetType(*ArgString) == CCExpressionValueTypeString)
                            {
                                CCTextAttribute Attribute = CCGraphicsExpressionLoadAttributedString(*ArgString, &Enumerator);
                                CCOrderedCollectionAppendElement(AttributedStrings, &Attribute);
                            }
                        }
                        
                        else if (CCStringEqual(CCExpressionGetAtom(*ArgString), CC_STRING("wrap:")))
                        {
                            Visibility = 0;
                            
                            while ((ArgString = CCCollectionEnumeratorNext(&Enumerator)))
                            {
                                if (CCExpressionGetType(*ArgString) == CCExpressionValueTypeAtom)
                                {
                                    if (CCStringEqual(CCExpressionGetAtom(*ArgString), CC_STRING(":char"))) Visibility |= CCTextVisibilityCharacter;
                                    else if (CCStringEqual(CCExpressionGetAtom(*ArgString), CC_STRING(":word"))) Visibility |= CCTextVisibilityWord;
                                    else if (CCStringEqual(CCExpressionGetAtom(*ArgString), CC_STRING(":single"))) Visibility |= CCTextVisibilitySingleLine;
                                    else if (CCStringEqual(CCExpressionGetAtom(*ArgString), CC_STRING(":multi"))) Visibility |= CCTextVisibilityMultiLine;
                                }
                            }
                        }
                        
                        else if (CCStringEqual(CCExpressionGetAtom(*ArgString), CC_STRING("align:")))
                        {
                            ArgString = CCCollectionEnumeratorNext(&Enumerator);
                            
                            if (CCExpressionGetType(*ArgString) == CCExpressionValueTypeAtom)
                            {
                                if (CCStringEqual(CCExpressionGetAtom(*ArgString), CC_STRING(":left"))) Alignment = CCTextAlignmentLeft;
                                else if (CCStringEqual(CCExpressionGetAtom(*ArgString), CC_STRING(":center"))) Alignment = CCTextAlignmentCenter;
                                else if (CCStringEqual(CCExpressionGetAtom(*ArgString), CC_STRING(":right"))) Alignment = CCTextAlignmentRight;
                            }
                        }
                        
                        else if (CCStringEqual(CCExpressionGetAtom(*ArgString), CC_STRING("offset:")))
                        {
                            ArgString = CCCollectionEnumeratorNext(&Enumerator);
                            
                            if (CCExpressionGetType(*ArgString) == CCExpressionValueTypeInteger)
                            {
                                Offset = CCExpressionGetInteger(*ArgString);
                            }
                        }
                        
                        else if (CCStringEqual(CCExpressionGetAtom(*ArgString), CC_STRING("length:")))
                        {
                            ArgString = CCCollectionEnumeratorNext(&Enumerator);
                            
                            if (CCExpressionGetType(*ArgString) == CCExpressionValueTypeInteger)
                            {
                                Length = CCExpressionGetInteger(*ArgString);
                            }
                            
                            else if ((CCExpressionGetType(*ArgString) == CCExpressionValueTypeAtom) && (CCStringEqual(CCExpressionGetAtom(*ArgString), CC_STRING(":max"))))
                            {
                                Length = SIZE_MAX;
                            }
                        }
                    }
                }
            }
        }
        
        CCText Text;
        CCExpression PreservedDraw = CCExpressionGetStateStrict(Expression, StrDrawer);
        if (PreservedDraw)
        {
            Text = CCExpressionGetData(PreservedDraw);
            Result = CCExpressionCopy(PreservedDraw);
        }
        
        else
        {
            Text = CCTextCreate(CC_STD_ALLOCATOR);
            Result = CCExpressionCreateCustomType(CC_STD_ALLOCATOR, CCGraphicsExpressionValueTypeText, Text, CCExpressionRetainedValueCopy, (CCExpressionValueDestructor)CCTextDestroy);
            
            CCExpressionCreateState(Expression, StrDrawer, Result, TRUE);
        }
        
        CCTextSetOffset(Text, Offset);
        CCTextSetVisibleLength(Text, Length);
        CCTextSetFrame(Text, CCExpressionGetRect(CCExpressionEvaluate(*ArgRect)));
        CCTextSetAlignment(Text, Alignment);
        CCTextSetVisibility(Text, Visibility);
        CCTextSetString(Text, AttributedStrings);
        CCCollectionDestroy(AttributedStrings);
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("render-text", "rect:list [...]");
    
    return Result;
}
