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
    CCExpression drawer;
    CCExpression args;
} CCGraphicsExpressionCache;

static void CCGraphicsExpressionCacheValueDestructor(CCGraphicsExpressionCache *Ptr)
{
    if (Ptr->drawer) CCExpressionDestroy(Ptr->drawer);
    if (Ptr->args) CCExpressionDestroy(Ptr->args);
}

static CCExpression CCGraphicsExpressionCacheCreate(CCAllocatorType Allocator, CCExpression Drawer, CCExpression Args)
{
    CCGraphicsExpressionCache *Cache;
    CC_SAFE_Malloc(Cache, sizeof(CCGraphicsExpressionCache),
                   CC_LOG_ERROR("Failed to create CCGraphicsExpressionCache due to allocation failure. Allocation size (%zu)", sizeof(CCGraphicsExpressionCache));
                   return NULL;
                   );
    
    CCExpression Result = CCExpressionCreateCustomType(CC_STD_ALLOCATOR, CCExpressionValueTypeUnspecified, Cache, CCExpressionRetainedValueCopy, CCFree);
    if (Result)
    {
        CCMemorySetDestructor(Cache, (CCMemoryDestructorCallback)CCGraphicsExpressionCacheValueDestructor);
        
        *Cache = (CCGraphicsExpressionCache){
            .drawer = Drawer,
            .args = Args
        };
    }
    
    else CCFree(Cache);
    
    return Result;
}

static inline CCExpression CCGraphicsExpressionCacheGetDrawer(CCExpression Cache)
{
    return ((CCGraphicsExpressionCache*)CCExpressionGetData(Cache))->drawer;
}

static inline CCExpression CCGraphicsExpressionCacheGetArgs(CCExpression Cache)
{
    return ((CCGraphicsExpressionCache*)CCExpressionGetData(Cache))->args;
}

typedef struct {
    CCVector2D position;
    CCColourRGBA colour;
    CCColourRGBA outlineColour;
    CCVector2D coord;
} CCGraphicsExpressionRectVertData;

typedef enum {
    CCGraphicsExpressionRenderRectEdgeTypeCurve,
    CCGraphicsExpressionRenderRectEdgeTypeFlat,
    
    CCGraphicsExpressionRenderRectEdgeMask = 1,
    CCGraphicsExpressionRenderRectEdgeLeft = 0,
    CCGraphicsExpressionRenderRectEdgeRight = 1,
    CCGraphicsExpressionRenderRectEdgeBottom = 2,
    CCGraphicsExpressionRenderRectEdgeTop = 3,
    
    CCGraphicsExpressionRenderRectEdgeAllCurved = (CCGraphicsExpressionRenderRectEdgeTypeCurve << CCGraphicsExpressionRenderRectEdgeLeft)
        | (CCGraphicsExpressionRenderRectEdgeTypeCurve << CCGraphicsExpressionRenderRectEdgeRight)
        | (CCGraphicsExpressionRenderRectEdgeTypeCurve << CCGraphicsExpressionRenderRectEdgeBottom)
        | (CCGraphicsExpressionRenderRectEdgeTypeCurve << CCGraphicsExpressionRenderRectEdgeTop),
    CCGraphicsExpressionRenderRectEdgeAllFlat = (CCGraphicsExpressionRenderRectEdgeTypeFlat << CCGraphicsExpressionRenderRectEdgeLeft)
        | (CCGraphicsExpressionRenderRectEdgeTypeFlat << CCGraphicsExpressionRenderRectEdgeRight)
        | (CCGraphicsExpressionRenderRectEdgeTypeFlat << CCGraphicsExpressionRenderRectEdgeBottom)
        | (CCGraphicsExpressionRenderRectEdgeTypeFlat << CCGraphicsExpressionRenderRectEdgeTop),
} CCGraphicsExpressionRenderRectEdge;

typedef struct {
    GFXBuffer vertBuffer;
    GFXBuffer scaleBuffer;
    GFXBuffer radiusBuffer;
    GFXBuffer outlineBuffer;
    CCRect rect;
    CCColourRGBA colour;
    CCColourRGBA outlineColour;
    CCVector2D outline;
    float radius;
    CCGraphicsExpressionRenderRectEdge edge;
} CCGraphicsExpressionRenderRectArgumentState;


static void CCGraphicsExpressionDrawValueDestructor(CCGraphicsExpressionValueDraw *Ptr)
{
    GFXDrawDestroy(Ptr->drawer);
}

static void CCGraphicsExpressionRenderRectArgumentStateValueDestructor(CCGraphicsExpressionRenderRectArgumentState *Ptr)
{
    GFXBufferDestroy(Ptr->vertBuffer);
    GFXBufferDestroy(Ptr->outlineBuffer);
    GFXBufferDestroy(Ptr->radiusBuffer);
    GFXBufferDestroy(Ptr->scaleBuffer);
}

static CCGraphicsExpressionRenderRectEdge CCGraphicsExpressionRenderRectGetEdge(CCExpression Edge)
{
    if (CCExpressionGetType(Edge) == CCExpressionValueTypeAtom)
    {
        if (CCStringEqual(CCExpressionGetString(Edge), CC_STRING(":flat"))) return CCGraphicsExpressionRenderRectEdgeTypeFlat;
        else if (CCStringEqual(CCExpressionGetString(Edge), CC_STRING(":round"))) return CCGraphicsExpressionRenderRectEdgeTypeCurve;
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_ERROR("Should be a valid atom: :flat | :round.");
    
    return 0;
}

CCExpression CCGraphicsExpressionRenderRect(CCExpression Expression)
{
    CCExpression Result = Expression;
    
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) >= 3)
    {
        CCEnumerator Enumerator;
        CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
        
        const CCRect Rect = CCExpressionGetRect(CCExpressionEvaluate(*(CCExpression*)CCCollectionEnumeratorNext(&Enumerator)));
        const CCColourRGBA Colour = CCExpressionGetColour(CCExpressionEvaluate(*(CCExpression*)CCCollectionEnumeratorNext(&Enumerator)));
        CCColourRGBA OutlineColour = CCVector4DFill(1.0f);
        CCVector2D Outline = CCVector2DFill(0.0f);
        float Radius = 0.0f;
        CCGraphicsExpressionRenderRectEdge Edge = CCGraphicsExpressionRenderRectEdgeAllCurved;
        
        for (CCExpression *Expr = CCCollectionEnumeratorNext(&Enumerator); Expr; Expr = CCCollectionEnumeratorNext(&Enumerator))
        {
            CCExpression Arg = CCExpressionEvaluate(*Expr);
            if (CCExpressionGetType(Arg) == CCExpressionValueTypeList)
            {
                size_t Count = CCCollectionGetCount(CCExpressionGetList(Arg));
                if (Count > 1)
                {
                    CCExpression Type = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 0);
                    if (CCExpressionGetType(Type) == CCExpressionValueTypeAtom)
                    {
                        if (CCStringEqual(CCExpressionGetString(Type), CC_STRING("radius:"))) Radius = CCExpressionGetNamedFloat(Arg);
                        else if (CCStringEqual(CCExpressionGetString(Type), CC_STRING("outline:")))
                        {
                            if (Count == 3)
                            {
                                float Size = 0.0f;
                                CCExpression OutlineSize = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 1);
                                
                                if (CCExpressionGetType(OutlineSize) == CCExpressionValueTypeFloat) Size = CCExpressionGetFloat(OutlineSize);
                                else if (CCExpressionGetType(OutlineSize) == CCExpressionValueTypeInteger) Size = (float)CCExpressionGetInteger(OutlineSize) / (Rect.size.x < Rect.size.y ? Rect.size.x : Rect.size.y);
                                
                                Outline = Rect.size.x < Rect.size.y ? CCVector2DMake(Size, (Rect.size.x / Rect.size.y) * Size) : CCVector2DMake((Rect.size.y / Rect.size.x) * Size, Size);
                                OutlineColour = CCExpressionGetColour(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 2));
                            }
                            
                            else CC_EXPRESSION_EVALUATOR_LOG_OPTION_ERROR("outline", "size:number colour:list");
                        }
                        
                        else if (CCStringEqual(CCExpressionGetString(Type), CC_STRING("left:"))) Edge |= CCGraphicsExpressionRenderRectGetEdge(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 1)) << CCGraphicsExpressionRenderRectEdgeLeft;
                        else if (CCStringEqual(CCExpressionGetString(Type), CC_STRING("right:"))) Edge |= CCGraphicsExpressionRenderRectGetEdge(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 1)) << CCGraphicsExpressionRenderRectEdgeRight;
                        else if (CCStringEqual(CCExpressionGetString(Type), CC_STRING("bottom:"))) Edge |= CCGraphicsExpressionRenderRectGetEdge(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 1)) << CCGraphicsExpressionRenderRectEdgeBottom;
                        else if (CCStringEqual(CCExpressionGetString(Type), CC_STRING("top:"))) Edge |= CCGraphicsExpressionRenderRectGetEdge(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 1)) << CCGraphicsExpressionRenderRectEdgeTop;
                    }
                }
            }
        }
        
        
        CCExpression Cache = CCExpressionStateGetPrivate(Expression);
        if (Cache)
        {
            Result = CCExpressionRetain(CCGraphicsExpressionCacheGetDrawer(Cache));
            
            _Bool UpdateScale = TRUE;
            CCGraphicsExpressionRenderRectArgumentState *Args = CCExpressionGetData(CCGraphicsExpressionCacheGetArgs(Cache));
            if ((Args->rect.position.x != Rect.position.x) ||
                (Args->rect.position.y != Rect.position.y) ||
                (Args->rect.size.x != Rect.size.x) ||
                (Args->rect.size.y != Rect.size.y) ||
                ((UpdateScale = FALSE)) ||
                (Args->colour.r != Colour.r) ||
                (Args->colour.g != Colour.g) ||
                (Args->colour.b != Colour.b) ||
                (Args->colour.a != Colour.a) ||
                (Args->outlineColour.r != OutlineColour.r) ||
                (Args->outlineColour.g != OutlineColour.g) ||
                (Args->outlineColour.b != OutlineColour.b) ||
                (Args->outlineColour.a != OutlineColour.a) ||
                (Args->edge != Edge))
            {
                Args->rect = Rect;
                Args->colour = Colour;
                Args->outlineColour = OutlineColour;
                Args->edge = Edge;
                
                const float LeftEdge = ((Edge >> CCGraphicsExpressionRenderRectEdgeLeft) & CCGraphicsExpressionRenderRectEdgeMask) == CCGraphicsExpressionRenderRectEdgeTypeCurve ? 0.0f : 0.5f;
                const float RightEdge = ((Edge >> CCGraphicsExpressionRenderRectEdgeRight) & CCGraphicsExpressionRenderRectEdgeMask) == CCGraphicsExpressionRenderRectEdgeTypeCurve ? 1.0f : 0.5f;
                const float BottomEdge = ((Edge >> CCGraphicsExpressionRenderRectEdgeBottom) & CCGraphicsExpressionRenderRectEdgeMask) == CCGraphicsExpressionRenderRectEdgeTypeCurve ? 0.0f : 0.5f;
                const float TopEdge = ((Edge >> CCGraphicsExpressionRenderRectEdgeTop) & CCGraphicsExpressionRenderRectEdgeMask) == CCGraphicsExpressionRenderRectEdgeTypeCurve ? 1.0f : 0.5f;
                
                GFXBufferWriteBuffer(Args->vertBuffer, 0, sizeof(CCGraphicsExpressionRectVertData) * 4, (CCGraphicsExpressionRectVertData[4]){
                    { .position = Rect.position, .colour = Colour, .outlineColour = OutlineColour, .coord = CCVector2DMake(LeftEdge, BottomEdge) },
                    { .position = CCVector2Add(Rect.position, CCVector2DMake(Rect.size.x, 0.0f)), .colour = Colour, .outlineColour = OutlineColour, .coord = CCVector2DMake(RightEdge, BottomEdge) },
                    { .position = CCVector2Add(Rect.position, CCVector2DMake(0.0f, Rect.size.y)), .colour = Colour, .outlineColour = OutlineColour, .coord = CCVector2DMake(LeftEdge, TopEdge) },
                    { .position = CCVector2Add(Rect.position, Rect.size), .colour = Colour, .outlineColour = OutlineColour, .coord = CCVector2DMake(RightEdge, TopEdge) }
                });
                
                if (UpdateScale) GFXBufferWriteBuffer(Args->scaleBuffer, 0, sizeof(CCVector2D), (Rect.size.x < Rect.size.y ? &CCVector2DMake(Rect.size.x / Rect.size.y, 1.0f) : &CCVector2DMake(1.0f, Rect.size.y / Rect.size.x)));
            }
            
            if (Args->radius != Radius)
            {
                Args->radius = Radius;
                
                GFXBufferWriteBuffer(Args->radiusBuffer, 0, sizeof(float), &Radius);
                
                GFXDrawSetBlending(((CCGraphicsExpressionValueDraw*)CCExpressionGetData(Result))->drawer, (Radius != 0.0f) || (Args->colour.a < 1.0f) | (Args->outlineColour.a < 1.0f) ? GFXBlendTransparent : GFXBlendOpaque);
            }
            
            if ((Args->outline.x != Outline.x) || (Args->outline.y != Outline.y))
            {
                Args->outline = Outline;
                
                GFXBufferWriteBuffer(Args->outlineBuffer, 0, sizeof(CCVector2D), &Outline);
            }
        }
        
        else
        {
            /*
             TODO: Use rounded-rect shader when no outline is used.
             
             Can keep the same buffers, just change the drawer.
             */
            GFXShader Shader = CCAssetManagerCreateShader(CC_STRING("outline-rounded-rect"));
            
            GFXBuffer ScaleBuffer = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCVector2D), (Rect.size.x < Rect.size.y ? &CCVector2DMake(Rect.size.x / Rect.size.y, 1.0f) : &CCVector2DMake(1.0f, Rect.size.y / Rect.size.x)));
            GFXBuffer RadiusBuffer = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(float), &Radius);
            GFXBuffer OutlineBuffer = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCVector2D), &Outline);
            
            const float LeftEdge = ((Edge >> CCGraphicsExpressionRenderRectEdgeLeft) & CCGraphicsExpressionRenderRectEdgeMask) == CCGraphicsExpressionRenderRectEdgeTypeCurve ? 0.0f : 0.5f;
            const float RightEdge = ((Edge >> CCGraphicsExpressionRenderRectEdgeRight) & CCGraphicsExpressionRenderRectEdgeMask) == CCGraphicsExpressionRenderRectEdgeTypeCurve ? 1.0f : 0.5f;
            const float BottomEdge = ((Edge >> CCGraphicsExpressionRenderRectEdgeBottom) & CCGraphicsExpressionRenderRectEdgeMask) == CCGraphicsExpressionRenderRectEdgeTypeCurve ? 0.0f : 0.5f;
            const float TopEdge = ((Edge >> CCGraphicsExpressionRenderRectEdgeTop) & CCGraphicsExpressionRenderRectEdgeMask) == CCGraphicsExpressionRenderRectEdgeTypeCurve ? 1.0f : 0.5f;
            
            GFXBuffer VertBuffer = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintDataVertex | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCGraphicsExpressionRectVertData) * 4, (CCGraphicsExpressionRectVertData[4]){
                { .position = Rect.position, .colour = Colour, .outlineColour = OutlineColour, .coord = CCVector2DMake(LeftEdge, BottomEdge) },
                { .position = CCVector2Add(Rect.position, CCVector2DMake(Rect.size.x, 0.0f)), .colour = Colour, .outlineColour = OutlineColour, .coord = CCVector2DMake(RightEdge, BottomEdge) },
                { .position = CCVector2Add(Rect.position, CCVector2DMake(0.0f, Rect.size.y)), .colour = Colour, .outlineColour = OutlineColour, .coord = CCVector2DMake(LeftEdge, TopEdge) },
                { .position = CCVector2Add(Rect.position, Rect.size), .colour = Colour, .outlineColour = OutlineColour, .coord = CCVector2DMake(RightEdge, TopEdge) }
            });
            
            GFXDraw Drawer = GFXDrawCreate(CC_STD_ALLOCATOR);
            GFXDrawSetShader(Drawer, Shader);
            GFXDrawSetVertexBuffer(Drawer, CC_STRING("vPosition"), VertBuffer, GFXBufferFormatFloat32x2, sizeof(CCGraphicsExpressionRectVertData), offsetof(CCGraphicsExpressionRectVertData, position));
            GFXDrawSetVertexBuffer(Drawer, CC_STRING("vColour"), VertBuffer, GFXBufferFormatFloat32x4, sizeof(CCGraphicsExpressionRectVertData), offsetof(CCGraphicsExpressionRectVertData, colour));
            GFXDrawSetVertexBuffer(Drawer, CC_STRING("vColourOutline"), VertBuffer, GFXBufferFormatFloat32x4, sizeof(CCGraphicsExpressionRectVertData), offsetof(CCGraphicsExpressionRectVertData, outlineColour));
            GFXDrawSetVertexBuffer(Drawer, CC_STRING("vCoord"), VertBuffer, GFXBufferFormatFloat32x2, sizeof(CCGraphicsExpressionRectVertData), offsetof(CCGraphicsExpressionRectVertData, coord));
            GFXDrawSetBuffer(Drawer, CC_STRING("scale"), ScaleBuffer);
            GFXDrawSetBuffer(Drawer, CC_STRING("radius"), RadiusBuffer);
            GFXDrawSetBuffer(Drawer, CC_STRING("outline"), OutlineBuffer);
            GFXDrawSetBlending(Drawer, (Radius != 0.0f) || (Colour.a < 1.0f) || (OutlineColour.a < 1.0f) ? GFXBlendTransparent : GFXBlendOpaque);
            
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
                .outlineBuffer = OutlineBuffer,
                .rect = Rect,
                .colour = Colour,
                .outlineColour = OutlineColour,
                .radius = Radius,
                .outline = Outline,
                .edge = Edge
            };
            
            
            CCExpressionStateSetPrivate(Expression, CCGraphicsExpressionCacheCreate(CC_STD_ALLOCATOR, CCExpressionRetain(Result), CCExpressionCreateCustomType(CC_STD_ALLOCATOR, CCExpressionValueTypeUnspecified, Args, CCExpressionRetainedValueCopy, CCFree)));
        }
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("render-rect", "rect:list colour:list [...]");
    
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
        CCExpression Cache = CCExpressionStateGetPrivate(Expression);
        if (Cache)
        {
            Result = CCExpressionRetain(CCGraphicsExpressionCacheGetDrawer(Cache));
            Text = CCExpressionGetData(Result);
        }
        
        else
        {
            Text = CCTextCreate(CC_STD_ALLOCATOR);
            Result = CCExpressionCreateCustomType(CC_STD_ALLOCATOR, CCGraphicsExpressionValueTypeText, Text, CCExpressionRetainedValueCopy, (CCExpressionValueDestructor)CCTextDestroy);
            
            CCExpressionStateSetPrivate(Expression, CCGraphicsExpressionCacheCreate(CC_STD_ALLOCATOR, CCExpressionRetain(Result), NULL));
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
