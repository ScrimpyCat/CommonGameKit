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

#define CC_QUICK_COMPILE
#include "Mouse.h"
#include "Window.h"
#include "Callbacks.h"
#include <stdatomic.h>
#include "InputSystem.h"
#include "InputMapMousePositionComponent.h"
#include "InputMapMouseButtonComponent.h"
#include "InputMapMouseScrollComponent.h"
#include "InputMapMouseDropComponent.h"

static struct {
    _Atomic(CCVector2D) position;
    _Atomic(CCMouseButtonMap) buttons[CCMouseButtonCount];
} Mouse;

void CCMouseDropInput(CCCollection Files)
{
    CCAssertLog(Files, "Files must not be null");
    
    CCMouseMap State = { .position = atomic_load(&Mouse.position), .drop = { .files = Files } };
    
    CCCollection InputFileDrops = CCInputSystemGetComponents(CCInputMapTypeMouseDrop);
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(InputFileDrops, &Enumerator);
    
    for (CCComponent *Input = CCCollectionEnumeratorGetCurrent(&Enumerator); Input; Input = CCCollectionEnumeratorNext(&Enumerator))
    {
        CCInputMapMouseCallback Callback = CCInputMapComponentGetCallback(*Input);
        if (Callback) Callback(*Input, CCMouseEventDrop, State);
    }
    
    CCCollectionDestroy(InputFileDrops);
}

void CCMouseScrollInput(CCVector2D ScrollDelta)
{
    CCMouseMap State = { .position = atomic_load(&Mouse.position), .scroll = { .delta = ScrollDelta } };
    
    CCCollection InputScrolls = CCInputSystemGetComponents(CCInputMapTypeMouseScroll);
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(InputScrolls, &Enumerator);
    
    for (CCComponent *Input = CCCollectionEnumeratorGetCurrent(&Enumerator); Input; Input = CCCollectionEnumeratorNext(&Enumerator))
    {
        CCInputMapMouseCallback Callback = CCInputMapComponentGetCallback(*Input);
        if (Callback) Callback(*Input, CCMouseEventScroll, State);
    }
    
    CCCollectionDestroy(InputScrolls);
}

void CCMouseButtonInput(CCMouseButton Button, CCKeyboardAction Action, CCKeyboardModifier Mods)
{
    if (Button != CCMouseButtonUnknown)
    {
        CCMouseButtonMap ButtonMap = { .button = Button, .flags = Mods, .state = { .timestamp = CCTimestamp(), .down = Action == CCKeyboardActionPress } };
        
        atomic_store(&Mouse.buttons[Button], ButtonMap);
        
        
        CCMouseMap State = { .position = atomic_load(&Mouse.position), .button = ButtonMap };
        
        CCCollection InputButtons = CCInputSystemGetComponents(CCInputMapTypeMouseButton);
        
        CCEnumerator Enumerator;
        CCCollectionGetEnumerator(InputButtons, &Enumerator);
        
        for (CCComponent *Input = CCCollectionEnumeratorGetCurrent(&Enumerator); Input; Input = CCCollectionEnumeratorNext(&Enumerator))
        {
            CCInputMapMouseCallback Callback = CCInputMapComponentGetCallback(*Input);
            if ((CCInputMapMouseButtonComponentGetButton(*Input) == Button) &&
                (CCInputMapMouseButtonComponentGetIgnoreModifier(*Input) || (CCInputMapMouseButtonComponentGetFlags(*Input) == Mods)))
            {
                if (Callback) Callback(*Input, CCMouseEventButton, State);
            }
        }
        
        CCCollectionDestroy(InputButtons);
    }
}

void CCMousePositionInput(CCVector2D Position)
{
    CCVector2D PreviousPosition = atomic_load(&Mouse.position);
    CCVector2D Delta = CCVector2Sub(Position, PreviousPosition);
    
    atomic_store(&Mouse.position, Position);
    
    
    CCMouseMap State = { .position = atomic_load(&Mouse.position), .move = { .delta = Delta } };
    
    CCCollection InputPositions = CCInputSystemGetComponents(CCInputMapTypeMousePosition);
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(InputPositions, &Enumerator);
    
    for (CCComponent *Input = CCCollectionEnumeratorGetCurrent(&Enumerator); Input; Input = CCCollectionEnumeratorNext(&Enumerator))
    {
        CCInputMapMouseCallback Callback = CCInputMapComponentGetCallback(*Input);
        if (Callback) Callback(*Input, CCMouseEventMove, State);
    }
    
    CCCollectionDestroy(InputPositions);
}

void CCMouseEnterInput(_Bool Entered)
{
    //TODO: Do we want to provide a callback here?
}

void CCMouseStateReset(void)
{
    for (size_t Loop = 0; Loop < sizeof(Mouse.buttons) / sizeof(typeof(*Mouse.buttons)); Loop++)
    {
        atomic_store(&Mouse.buttons[Loop], ((CCMouseButtonMap){ .button = (uint32_t)Loop, .state = { .timestamp = -INFINITY, .down = FALSE } }));
    }
}

CCMouseButtonState CCMouseButtonGetStateForComponent(CCComponent Component)
{
    CCAssertLog(CCComponentGetID(Component) == CC_INPUT_MAP_MOUSE_BUTTON_COMPONENT_ID, "Must be a input map mouse button component");
    
    CCMouseButtonMap Button = atomic_load(&Mouse.buttons[CCInputMapMouseButtonComponentGetButton(Component)]);
    if (CCInputMapMouseButtonComponentGetIgnoreModifier(Component) || (CCInputMapMouseButtonComponentGetFlags(Component) == Button.flags))
    {
        return Button.state;
    }
    
    return (CCMouseButtonState){ .timestamp = 0.0, .down = FALSE };
}
