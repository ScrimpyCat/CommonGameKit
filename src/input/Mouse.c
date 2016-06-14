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

#include "Mouse.h"
#include "Window.h"
#include "stdatomic.h"
#include "InputSystem.h"
#include "InputMapMousePositionComponent.h"
#include "InputMapMouseButtonComponent.h"
#include "InputMapMouseScrollComponent.h"
#include "InputMapMouseDropComponent.h"

static struct {
    _Atomic(CCVector2D) position;
    _Atomic(CCMouseButtonMap) buttons[GLFW_MOUSE_BUTTON_LAST + 1];
} Mouse;

void CCMouseDropInput(GLFWwindow *Window, int Count, const char **Files)
{
    CCOrderedCollection FileList = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintHeavyEnumerating | CCCollectionHintConstantLength | CCCollectionHintConstantElements | CCCollectionHintSizeSmall, sizeof(CCString), CCStringDestructorForCollection);
    
    for (int Loop = 0; Loop < Count; Loop++) CCOrderedCollectionAppendElement(FileList, &(CCString){ CCStringCreate(CC_STD_ALLOCATOR, (CCStringHint)CCStringEncodingASCII, Files[Loop]) });
    
    
    CCMouseMap State = { .position = atomic_load(&Mouse.position), .drop = { .files = FileList } };
    
    CCCollection InputFileDrops = CCInputSystemGetComponents(CCInputMapTypeMouseDrop);
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(InputFileDrops, &Enumerator);
    
    for (CCComponent *Input = CCCollectionEnumeratorGetCurrent(&Enumerator); Input; Input = CCCollectionEnumeratorNext(&Enumerator))
    {
        CCInputMapMouseCallback Callback = CCInputMapComponentGetCallback(*Input);
        if (Callback) Callback(*Input, CCMouseEventDrop, State);
    }
    
    CCCollectionDestroy(InputFileDrops);
    
    
    CCCollectionDestroy(FileList);
}

void CCMouseScrollInput(GLFWwindow *Window, double x, double y)
{
    CCVector2D ScrollDelta = CCVector2DMake(x, y);
    
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

void CCMouseButtonInput(GLFWwindow *Window, int Button, int Action, int Mods)
{
    CCMouseButtonMap ButtonMap = { .button = Button, .flags = Mods, .state = { .timestamp = glfwGetTime(), .down = Action == GLFW_PRESS } };
    
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

void CCMousePositionInput(GLFWwindow *Window, double x, double y)
{
    int Height;
    glfwGetWindowSize(Window, NULL, &Height);
    
    CCVector2D PreviousPosition = atomic_load(&Mouse.position), CurrentPosition = CCVector2DMake(x, Height - y);
    CCVector2D Delta = CCVector2DMake(CurrentPosition.x - PreviousPosition.x, CurrentPosition.y - PreviousPosition.y);
    
    atomic_store(&Mouse.position, CurrentPosition);
    
    
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

void CCMouseEnterInput(GLFWwindow *Window, int Entered)
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
