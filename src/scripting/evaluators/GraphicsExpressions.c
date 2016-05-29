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

typedef struct {
    CCVector2D position;
    CCColourRGBA colour;
    CCVector2D coord;
} CCGraphicsExpressionRectVertData;


static void CCGraphicsExpressionDrawValueDestructor(CCGraphicsExpressionValueDraw *Ptr)
{
    GFXDrawDestroy(Ptr->drawer);
}

CCExpression CCGraphicsExpressionRenderRect(CCExpression Expression)
{
    CCExpression Expr = Expression;
    
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
        GFXDrawSetBlending(Drawer, GFXBlendTransparent);
//        GFXDrawSubmit(Drawer, GFXPrimitiveTypeTriangleStrip, 0, 4);
        
        GFXBufferDestroy(VertBuffer);
        GFXBufferDestroy(RadiusBuffer);
        GFXBufferDestroy(ScaleBuffer);
        GFXShaderDestroy(Shader);
        
        
        CCGraphicsExpressionValueDraw *Drawable;
        CC_SAFE_Malloc(Drawable, sizeof(CCGraphicsExpressionValueDraw),
                       CC_LOG_ERROR("Failed to create CCGraphicsExpressionValueDraw due to allocation failure. Allocation size (%zu)", sizeof(CCGraphicsExpressionValueDraw));
                       GFXDrawDestroy(Drawer);
                       return Expr;
                       );
        
        CCMemorySetDestructor(Drawable, (CCMemoryDestructorCallback)CCGraphicsExpressionDrawValueDestructor);
        
        Drawable->drawer = Drawer;
        Drawable->vertices = 4;
        
        Expr = CCExpressionCreateCustomType(CC_STD_ALLOCATOR, CCGraphicsExpressionValueTypeDraw, Drawable, CCExpressionRetainedValueCopy, CCFree);
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("render-rect", "rect:list colour:list [radius:float]");
    
    return Expr;
}
