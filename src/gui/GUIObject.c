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

#include "GUIObject.h"
#include "Window.h"
#include "AssetManager.h"

GUIObject GUIObjectCreate(CCAllocatorType Allocator, const GUIObjectInterface *Interface)
{
    CCAssertLog(Interface, "Interface must not be null");
    
    GUIObject Object = CCMalloc(Allocator, sizeof(GUIObjectInfo), NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (Object)
    {
        *Object = (GUIObjectInfo){
            .interface = Interface,
            .allocator = Allocator,
            .parent = NULL,
            .internal = Interface->create(Allocator),
            .cache = {
                .strategy = GUIObjectCacheStrategyNone,
                .rect = (CCRect){ .position = CCVector2DZero, .size = CCVector2DZero },
                .store = NULL,
                .changed = TRUE
            }
        };
        
        if (!Object->internal)
        {
            CC_LOG_ERROR("Failed to create GUI object: Implementation failure (%p)", Interface);
            CC_SAFE_Free(Object);
        }
        
        else if (mtx_init(&Object->lock, mtx_plain | mtx_recursive) != thrd_success)
        {
            CC_LOG_ERROR("Failed to create lock for GUIObject (%p)", Object);
        }
    }
    
    else CC_LOG_ERROR("Failed to create GUI object due to allocation failure. Allocation size (%zu)", sizeof(GUIObjectInfo));
    
    return Object;
}

void GUIObjectDestroy(GUIObject Object)
{
    CCAssertLog(Object, "GUI object must not be null");
    
    GUIObject Parent = GUIObjectGetParent(Object);
    if (Parent)
    {
        GUIObjectRemoveChild(Parent, Object);
    }
    
    mtx_destroy(&Object->lock);
    if (Object->cache.store) GFXFramebufferDestroy(Object->cache.store);
    Object->interface->destroy(Object->internal);
    CC_SAFE_Free(Object);
}

void GUIObjectRender(GUIObject Object, GFXFramebuffer Framebuffer, size_t Index, GFXBuffer Projection)
{
    CCAssertLog(Object, "GUI object must not be null");
    
    mtx_lock(&Object->lock);
    
    if (GUIObjectGetEnabled(Object))
    {
        if ((Object->cache.strategy & GUIObjectCacheStrategyMask) != GUIObjectCacheStrategyNone)
        {
            _Bool Changed = GUIObjectHasChanged(Object);
            const CCRect Rect = Object->cache.rect;
            GFXTexture Texture;
            
            if (Changed)
            {
                if ((Object->cache.store) && (Object->cache.strategy & GUIObjectCacheDirtyResize))
                {
                    GFXFramebufferDestroy(Object->cache.store);
                    Object->cache.store = NULL;
                }
                
                if (!Object->cache.store)
                {
                    //TODO: use frame bounds?
                    //TODO: Use texture streams?
                    Texture = GFXTextureCreate(CC_STD_ALLOCATOR, GFXTextureHintDimension2D | (GFXTextureHintFilterModeNearest << GFXTextureHintFilterMin) | (GFXTextureHintFilterModeNearest << GFXTextureHintFilterMag), CCColourFormatRGB8Unorm, (size_t)Rect.size.x, (size_t)Rect.size.y, 1, NULL);
                    GFXFramebufferAttachment Attachment = GFXFramebufferAttachmentCreateColour(Texture, GFXFramebufferAttachmentActionFlagClearOnce | GFXFramebufferAttachmentActionLoad, GFXFramebufferAttachmentActionStore, CCVector4DFill(0.0f));
                    Object->cache.store = GFXFramebufferCreate(CC_STD_ALLOCATOR, &Attachment, 1);
                }
                
                else
                {
                    GFXFramebufferAttachment *Attachment = GFXFramebufferGetAttachment(Object->cache.store, 0);
                    Attachment->load |= GFXFramebufferAttachmentActionFlagClearOnce;
                    Texture = Attachment->texture;
                }
                
                CCMatrix4 Ortho = CCMatrix4MakeOrtho(0.0f, Rect.size.x, 0.0f, Rect.size.y, 0.0f, 1.0f);
                Ortho = CCMatrix4Translate(Ortho, CCVector3DMake(Rect.position.x, Rect.position.y, 0.0f));
                GFXBuffer CacheProjection = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCMatrix4), &Ortho);
                Object->interface->render(Object, Object->cache.store, 0, CacheProjection);
                Object->cache.strategy &= GUIObjectCacheStrategyMask;
                
                GFXBufferDestroy(CacheProjection);
            }
            
            else Texture = GFXFramebufferGetAttachment(Object->cache.store, 0)->texture;
            
            
            GFXDraw Drawer = GFXDrawCreate(CC_STD_ALLOCATOR);
            
            GFXShader Shader = CCAssetManagerCreateShader(CC_STRING("texture2d"));
            GFXDrawSetShader(Drawer, Shader);
            GFXShaderDestroy(Shader);
            
            struct {
                CCVector2D position;
                CCVector2D coord;
            } VertData[4] = {
                { .position = Rect.position, .coord = CCVector2DMake(0.0f, 0.0f) },
                { .position = CCVector2DMake(Rect.position.x + Rect.size.x, Rect.position.y), .coord = CCVector2DMake(1.0f, 0.0f) },
                { .position = CCVector2DMake(Rect.position.x, Rect.position.y + Rect.size.y), .coord = CCVector2DMake(0.0f, 1.0f) },
                { .position = CCVector2DMake(Rect.position.x + Rect.size.x, Rect.position.y + Rect.size.y), .coord = CCVector2DMake(1.0f, 1.0f) }
            };
            
            GFXBuffer VertBuffer = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintDataVertex | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(VertData), &VertData);
            GFXDrawSetVertexBuffer(Drawer, CC_STRING("vPosition"), VertBuffer, GFXBufferFormatFloat32x2, sizeof(typeof(*VertData)), offsetof(typeof(*VertData), position));
            GFXDrawSetVertexBuffer(Drawer, CC_STRING("vTexCoord"), VertBuffer, GFXBufferFormatFloat32x2, sizeof(typeof(*VertData)), offsetof(typeof(*VertData), coord));
            GFXBufferDestroy(VertBuffer);
            
            GFXDrawSetTexture(Drawer, CC_STRING("tex"), Texture);
            GFXDrawSetBlending(Drawer, GFXBlendTransparentPremultiplied);
            GFXDrawSetFramebuffer(Drawer, Framebuffer, Index);
            GFXDrawSetBuffer(Drawer, CC_STRING("modelViewProjectionMatrix"), Projection);
            GFXDrawSubmit(Drawer, GFXPrimitiveTypeTriangleStrip, 0, 4);
            
            GFXDrawDestroy(Drawer);
        }
        
        else Object->interface->render(Object, Framebuffer, Index, Projection);
    }
    Object->cache.changed = FALSE;
    mtx_unlock(&Object->lock);
}

void GUIObjectEvent(GUIObject Object, GUIEvent Event)
{
    CCAssertLog(Object, "GUI object must not be null");
    
    mtx_lock(&Object->lock);
    if (GUIObjectGetEnabled(Object))
    {
        Object->cache.strategy |= GUIObjectCacheDirtyEvent;
        Object->interface->event(Object, Event);
    }
    mtx_unlock(&Object->lock);
}

CCRect GUIObjectGetRect(GUIObject Object)
{
    CCAssertLog(Object, "GUI object must not be null");
    
    mtx_lock(&Object->lock);
    const CCRect Rect = Object->interface->rect.get(Object);
    mtx_unlock(&Object->lock);
    
    return Rect;
}

void GUIObjectSetRect(GUIObject Object, CCRect Rect)
{
    CCAssertLog(Object, "GUI object must not be null");
    
    mtx_lock(&Object->lock);
    Object->interface->rect.set(Object, Rect);
    mtx_unlock(&Object->lock);
}

_Bool GUIObjectGetEnabled(GUIObject Object)
{
    CCAssertLog(Object, "GUI object must not be null");
    
    mtx_lock(&Object->lock);
    const _Bool Enabled = Object->interface->enabled.get(Object);
    mtx_unlock(&Object->lock);
    
    return Enabled;
}

void GUIObjectSetEnabled(GUIObject Object, _Bool Enabled)
{
    CCAssertLog(Object, "GUI object must not be null");
    
    mtx_lock(&Object->lock);
    Object->interface->enabled.set(Object, Enabled);
    mtx_unlock(&Object->lock);
}

GUIObject GUIObjectGetParent(GUIObject Object)
{
    CCAssertLog(Object, "GUI object must not be null");
    
    return Object->parent;
}

GUIObjectCache GUIObjectGetCacheStrategy(GUIObject Object)
{
    CCAssertLog(Object, "GUI object must not be null");
    
    mtx_lock(&Object->lock);
    const GUIObjectCache Cache = Object->cache.strategy & GUIObjectCacheStrategyMask;
    mtx_unlock(&Object->lock);
    
    return Cache;
}

void GUIObjectSetCacheStrategy(GUIObject Object, GUIObjectCache Strategy)
{
    CCAssertLog(Object, "GUI object must not be null");
    
    mtx_lock(&Object->lock);
    Object->cache.strategy = Strategy & GUIObjectCacheStrategyMask;
    Object->cache.changed = TRUE;
    
    if ((Object->cache.store) && ((Strategy & GUIObjectCacheStrategyMask) == GUIObjectCacheStrategyNone))
    {
        GFXFramebufferDestroy(Object->cache.store);
        Object->cache.store = NULL;
    }
    mtx_unlock(&Object->lock);
}

void GUIObjectAddChild(GUIObject Object, GUIObject Child)
{
    CCAssertLog(Object, "GUI object must not be null");
    CCAssertLog(Child, "Child GUI object must not be null");
    
    mtx_lock(&Object->lock);
    Child->parent = Object;
    Object->interface->child.add(Object, Child);
    Object->cache.changed = TRUE;
    Child->cache.changed = TRUE;
    mtx_unlock(&Object->lock);
}

void GUIObjectRemoveChild(GUIObject Object, GUIObject Child)
{
    CCAssertLog(Object, "GUI object must not be null");
    CCAssertLog(Child, "Child GUI object must not be null");
    CCAssertLog(Object == Child->parent, "The GUI object must be the parent of the child GUI object");
    
    mtx_lock(&Object->lock);
    Child->parent = NULL;
    Object->interface->child.remove(Object, Child);
    Object->cache.changed = TRUE;
    Child->cache.changed = TRUE;
    mtx_unlock(&Object->lock);
}

_Bool GUIObjectHasChanged(GUIObject Object)
{
    CCAssertLog(Object, "GUI object must not be null");
    
    mtx_lock(&Object->lock);
    if (Object->cache.strategy != GUIObjectCacheStrategyNone)
    {
        const CCRect Rect = GUIObjectGetRect(Object);
        
        Object->cache.strategy |= (CCVector2Equal(Rect.position, Object->cache.rect.position) ? 0 : GUIObjectCacheDirtyMove) | (CCVector2Equal(Rect.size, Object->cache.rect.size) ? 0 : GUIObjectCacheDirtyResize);
        
        Object->cache.rect = Rect;
        
        if ((Object->cache.strategy >> GUIObjectCacheDirtyIndex) & (Object->cache.strategy & GUIObjectCacheStrategyMask)) Object->cache.changed = TRUE;
    }
    
    _Bool Changed = Object->cache.changed || Object->interface->changed(Object);
    mtx_unlock(&Object->lock);
    
    return Changed;
}

CCExpression GUIObjectEvaluateExpression(GUIObject Object, CCExpression Expression)
{
    CCAssertLog(Object, "GUI object must not be null");
    
    mtx_lock(&Object->lock);
    CCExpression Expr = CCExpressionRetain(Object->interface->evaluate(Object, Expression));
    mtx_unlock(&Object->lock);
    
    CCExpressionStateSetResult(Expression, Expr);
    return Expr;
}

CCExpression GUIObjectGetExpressionState(GUIObject Object)
{
    CCAssertLog(Object, "GUI object must not be null");
    
    mtx_lock(&Object->lock);
    CCExpression Expr = Object->interface->state(Object);
    mtx_unlock(&Object->lock);
    
    return Expr;
}
