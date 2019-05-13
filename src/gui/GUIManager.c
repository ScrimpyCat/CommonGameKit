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

#include "GUIManager.h"
#include "Window.h"
#include "Entity.h"
#include "EntityManager.h"

#include "InputSystem.h"
#include "InputMapKeyboardComponent.h"
#include "InputMapMousePositionComponent.h"
#include "InputMapMouseButtonComponent.h"
#include "InputMapMouseScrollComponent.h"
#include "InputMapMouseDropComponent.h"

#include <threads.h>
#include <stdatomic.h>


#define GUI_MANAGER_ENTITY_ID CC_STRING("gui")

typedef struct {
    CCEntity entity;
    CCCollection active, added, removed;
    atomic_flag addedLock, removedLock;
} GUIManager;

static GUIManager ObjectManager = {
    .addedLock = ATOMIC_FLAG_INIT,
    .removedLock = ATOMIC_FLAG_INIT
};

static void GUIManagerEventMouseCallback(CCComponent Component, CCMouseEvent Event, CCMouseMap State)
{
    GUIManagerLock();
    GUIManagerHandleEvent(&(GUIEventInfo){
        .type = GUIEventTypeMouse,
        .mouse = { .event = Event, .state = State }
    });
    GUIManagerUnlock();
}

static void GUIManagerEventKeyboardCallback(CCComponent Component, CCKeyboardMap State)
{
    GUIManagerLock();
    GUIManagerHandleEvent(&(GUIEventInfo){
        .type = GUIEventTypeKey,
        .key = { .state = State }
    });
    GUIManagerUnlock();
}

static void GUIObjectDestructor(CCCollection Collection, GUIObject *Object)
{
    GUIObjectDestroy(*Object);
}

static mtx_t Lock;
void GUIManagerCreate(void)
{
    int err;
    if ((err = mtx_init(&Lock, mtx_plain | mtx_recursive)) != thrd_success)
    {
        CC_LOG_ERROR("Failed to create GUI manager lock (%d)", err);
        exit(EXIT_FAILURE); //TODO: How should we handle this?
    }
    
    ObjectManager = (GUIManager){
        .entity = CCEntityCreate(GUI_MANAGER_ENTITY_ID, CC_STD_ALLOCATOR),
        .active = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintSizeMedium | CCCollectionHintHeavyEnumerating, sizeof(GUIObject), NULL),
        .added = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintSizeSmall | CCCollectionHintHeavyInserting | CCCollectionHintHeavyDeleting, sizeof(GUIObject), NULL),
        .removed = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintSizeSmall | CCCollectionHintHeavyInserting | CCCollectionHintHeavyDeleting, sizeof(GUIObject), (CCCollectionElementDestructor)GUIObjectDestructor),
        .addedLock = ATOMIC_FLAG_INIT,
        .removedLock = ATOMIC_FLAG_INIT
    };
    
    
    //TODO: This will get quite awkward, need to separate input from the ECS.
    CCComponent Input = CCComponentCreate(CC_INPUT_MAP_MOUSE_POSITION_COMPONENT_ID);
    CCInputMapComponentSetCallback(Input, GUIManagerEventMouseCallback);
    CCEntityAttachComponent(ObjectManager.entity, Input);
    CCComponentSystemAddComponent(Input);
    
    Input = CCComponentCreate(CC_INPUT_MAP_MOUSE_BUTTON_COMPONENT_ID);
    CCInputMapComponentSetCallback(Input, GUIManagerEventMouseCallback);
    CCInputMapMouseButtonComponentSetButton(Input, CCMouseButtonLeft);
    CCInputMapMouseButtonComponentSetIgnoreModifier(Input, TRUE);
    CCEntityAttachComponent(ObjectManager.entity, Input);
    CCComponentSystemAddComponent(Input);
    
    Input = CCComponentCreate(CC_INPUT_MAP_MOUSE_BUTTON_COMPONENT_ID);
    CCInputMapComponentSetCallback(Input, GUIManagerEventMouseCallback);
    CCInputMapMouseButtonComponentSetButton(Input, CCMouseButtonRight);
    CCInputMapMouseButtonComponentSetIgnoreModifier(Input, TRUE);
    CCEntityAttachComponent(ObjectManager.entity, Input);
    CCComponentSystemAddComponent(Input);
    
    Input = CCComponentCreate(CC_INPUT_MAP_MOUSE_BUTTON_COMPONENT_ID);
    CCInputMapComponentSetCallback(Input, GUIManagerEventMouseCallback);
    CCInputMapMouseButtonComponentSetButton(Input, CCMouseButtonMiddle);
    CCInputMapMouseButtonComponentSetIgnoreModifier(Input, TRUE);
    CCEntityAttachComponent(ObjectManager.entity, Input);
    CCComponentSystemAddComponent(Input);
    
    Input = CCComponentCreate(CC_INPUT_MAP_MOUSE_SCROLL_COMPONENT_ID);
    CCInputMapComponentSetCallback(Input, GUIManagerEventMouseCallback);
    CCEntityAttachComponent(ObjectManager.entity, Input);
    CCComponentSystemAddComponent(Input);
    
    Input = CCComponentCreate(CC_INPUT_MAP_MOUSE_DROP_COMPONENT_ID);
    CCInputMapComponentSetCallback(Input, GUIManagerEventMouseCallback);
    CCEntityAttachComponent(ObjectManager.entity, Input);
    CCComponentSystemAddComponent(Input);
    
    Input = CCComponentCreate(CC_INPUT_MAP_KEYBOARD_COMPONENT_ID);
    CCInputMapComponentSetCallback(Input, GUIManagerEventKeyboardCallback);
    CCInputMapKeyboardComponentSetKeycode(Input, CCInputMapKeyboardComponentKeycodeAny);
    CCInputMapKeyboardComponentSetIsKeycode(Input, TRUE);
    CCInputMapKeyboardComponentSetRepeats(Input, TRUE);
    CCInputMapKeyboardComponentSetIgnoreModifier(Input, TRUE);
    CCEntityAttachComponent(ObjectManager.entity, Input);
    CCComponentSystemAddComponent(Input);
    
    CCEntityManagerAddEntity(ObjectManager.entity);
}

void GUIManagerDestroy(void)
{
    mtx_destroy(&Lock);
    
    if (ObjectManager.active) CCCollectionDestroy(ObjectManager.active);
    if (ObjectManager.added) CCCollectionDestroy(ObjectManager.added);
    if (ObjectManager.removed) CCCollectionDestroy(ObjectManager.removed);
    
    CCEntityDestroy(ObjectManager.entity);
}

void GUIManagerUpdate(void)
{
    //Add to active
    while (!atomic_flag_test_and_set(&ObjectManager.addedLock)) CC_SPIN_WAIT();
    CCCollectionInsertCollection(ObjectManager.active, ObjectManager.added, NULL); //TODO: make a consumed insert or a retained list?
    
    CCCollectionRemoveAllElements(ObjectManager.added);
    
    atomic_flag_clear(&ObjectManager.addedLock);
    
    
    //Remove from active
    while (!atomic_flag_test_and_set(&ObjectManager.removedLock)) CC_SPIN_WAIT();
    CCCollection Entries = CCCollectionFindCollection(ObjectManager.active, ObjectManager.removed, NULL);
    CCCollectionRemoveCollection(ObjectManager.active, Entries);
    CCCollectionDestroy(Entries);
    
    CCCollectionRemoveAllElements(ObjectManager.removed);
    atomic_flag_clear(&ObjectManager.removedLock);
}

void GUIManagerRender(GFXFramebuffer Framebuffer, size_t Index)
{
    static CCVector2Di CachedSize = CCVector2DiZero;
    static GFXBuffer CachedProjection = NULL;
    
    const CCVector2Di Size = CCWindowGetFrameSize();
    if ((Size.x != CachedSize.x) || (Size.y != CachedSize.y) || (!CachedProjection))
    {
        if (CachedProjection) GFXBufferDestroy(CachedProjection);
        
        CCMatrix4 Ortho = CCMatrix4MakeOrtho(0.0f, Size.x, 0.0f, Size.y, -1.0f, 1.0f);
        CachedProjection = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadMany, sizeof(CCMatrix4), &Ortho);
        
        CachedSize = Size;
    }
    
    CC_COLLECTION_FOREACH(GUIObject, Object, ObjectManager.active)
    {
        GUIObjectRender(Object, Framebuffer, Index, CachedProjection);
    }
}

void GUIManagerHandleEvent(GUIEvent Event)
{
    CC_COLLECTION_FOREACH(GUIObject, Object, ObjectManager.active)
    {
        GUIObjectEvent(Object, Event);
    }
}

void GUIManagerAddObject(GUIObject Object)
{
    while (!atomic_flag_test_and_set(&ObjectManager.addedLock)) CC_SPIN_WAIT();
    CCCollectionInsertElement(ObjectManager.added, &Object);
    atomic_flag_clear(&ObjectManager.addedLock);
}

void GUIManagerRemoveObject(GUIObject Object)
{
    while (!atomic_flag_test_and_set(&ObjectManager.removedLock)) CC_SPIN_WAIT();
    CCCollectionInsertElement(ObjectManager.removed, &Object);
    atomic_flag_clear(&ObjectManager.removedLock);
}

CCCollection GUIManagerGetObjects(void)
{
    return ObjectManager.active;
}

GUIObject GUIManagerFindObjectWithNamespace(CCString Namespace)
{
    CC_COLLECTION_FOREACH(GUIObject, UI, ObjectManager.active)
    {
        CCExpression Name = CCExpressionGetStateStrict(GUIObjectGetExpressionState(UI), CC_STRING("@namespace"));
        if ((Name) && (CCExpressionGetType(Name) == CCExpressionValueTypeAtom) && (CCStringEqual(CCExpressionGetAtom(Name), Namespace))) return UI;
    }
    
    return NULL;
}

_Bool GUIManagerTryLock(void)
{
    int err = mtx_trylock(&Lock);
    if ((err != thrd_success) && (err != thrd_busy))
    {
        CC_LOG_ERROR("Failed to lock GUI manager (%d)", err);
    }
    
    return err == thrd_success;
}

void GUIManagerLock(void)
{
    int err;
    if ((err = mtx_lock(&Lock)) != thrd_success)
    {
        CC_LOG_ERROR("Failed to lock GUI manager (%d)", err);
    }
}

void GUIManagerUnlock(void)
{
    int err;
    if ((err = mtx_unlock(&Lock)) != thrd_success)
    {
        CC_LOG_ERROR("Failed to unlock GUI manager (%d)", err);
    }
}
