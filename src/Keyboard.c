//
//  Keyboard.c
//  Blob Game
//
//  Created by Stefan Johnson on 4/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#include "Keyboard.h"
#include "Window.h"
#include "stdatomic.h"
#include "InputSystem.h"
#include "InputMapKeyboardComponent.h"
#include <math.h>


static _Atomic(CCKeyboardMap) KeyList[GLFW_KEY_LAST + 1];

/*
 TODO: Unsure if we can make this guarantee across all platforms or with future minor verson of GLFW.
 It looks like it should be ok on OS X (is), X11, Wayland, Mir; but not sure about Windows. Can we guarantee that the
 input sequence of events will always be keycode, followed by character (or at least character, followed by keycode).
 Must make sure it won't possibly send in another keycode inbetween.
 */
static CCKeyboardMap TempKey;
void CCKeyboardInput(GLFWwindow *Window, int Keycode, int Scancode, int Action, int Mods)
{
    if ((TempKey.keycode = Keycode) != GLFW_KEY_UNKNOWN)
    {
        CCAssertLog(Keycode >= 0 && Keycode <= GLFW_KEY_LAST, "Keycode is not within bounds");
        
        TempKey.flags = Mods;
        
        if (Action == GLFW_REPEAT)
        {
            CCKeyboardMap Prev = atomic_load(&KeyList[Keycode]);
            TempKey.state.timestamp = Prev.state.timestamp;
            TempKey.state.down = TRUE;
            TempKey.state.repeat = TRUE;
        }
        
        else
        {
            TempKey.state.timestamp = glfwGetTime();
            TempKey.state.down = Action == GLFW_PRESS;
            TempKey.state.repeat = FALSE;
        }
        
        if (!TempKey.state.down)
        {
            TempKey.character = 0;
            atomic_store(&KeyList[TempKey.keycode], TempKey);
            
            CCCollection InputKeys = CCInputSystemGetComponents(CCInputMapTypeKeyboard);
            
            CCEnumerator Enumerator;
            CCCollectionGetEnumerator(InputKeys, &Enumerator);
            
            for (CCComponent *Input = CCCollectionEnumeratorGetCurrent(&Enumerator); Input; Input = CCCollectionEnumeratorNext(&Enumerator))
            {
                CCInputMapKeyboardCallback Callback = CCInputMapComponentGetCallback(*Input);
                if (Callback)
                {
                    if ((CCInputMapKeyboardComponentGetIsKeycode(*Input) && (CCInputMapKeyboardComponentGetKeycode(*Input) == Keycode)) &&
                        (CCInputMapKeyboardComponentGetIgnoreModifier(*Input) || (CCInputMapKeyboardComponentGetFlags(*Input) == Mods)))
                    {
                        Callback(*Input, TempKey);
                    }
                }
            }
            
            CCCollectionDestroy(InputKeys);
        }
    }
}

void CCKeyboardCharInput(GLFWwindow *Window, unsigned int Codepoint, int Mods)
{
    if (TempKey.keycode != GLFW_KEY_UNKNOWN)
    {
        TempKey.character = Codepoint;
        atomic_store(&KeyList[TempKey.keycode], TempKey);
        
        
        CCCollection InputKeys = CCInputSystemGetComponents(CCInputMapTypeKeyboard);
        
        CCEnumerator Enumerator;
        CCCollectionGetEnumerator(InputKeys, &Enumerator);
        
        for (CCComponent *Input = CCCollectionEnumeratorGetCurrent(&Enumerator); Input; Input = CCCollectionEnumeratorNext(&Enumerator))
        {
            CCInputMapKeyboardCallback Callback = CCInputMapComponentGetCallback(*Input);
            if (Callback)
            {
                if (((CCInputMapKeyboardComponentGetIsKeycode(*Input) && (CCInputMapKeyboardComponentGetKeycode(*Input) == TempKey.keycode)) ||
                     (CCInputMapKeyboardComponentGetCharacter(*Input) == Codepoint)) &&
                    (CCInputMapKeyboardComponentGetIgnoreModifier(*Input) || (CCInputMapKeyboardComponentGetFlags(*Input) == Mods)))
                {
                    Callback(*Input, TempKey);
                }
            }
        }
        
        CCCollectionDestroy(InputKeys);
    }
}

void CCKeyboardStateReset(void)
{
    for (size_t Loop = 0; Loop < sizeof(KeyList) / sizeof(typeof(*KeyList)); Loop++)
    {
        atomic_store(&KeyList[Loop], ((CCKeyboardMap){ .keycode = (uint32_t)Loop, .state.timestamp = -INFINITY }));
    }
}

CCKeyboardState CCKeyboardGetStateForComponent(CCComponent Component)
{
    CCAssertLog(CCComponentGetID(Component) == CC_INPUT_MAP_KEYBOARD_COMPONENT_ID, "Must be a input map keyboard component");
    
    if (CCInputMapKeyboardComponentGetKeycode(Component) != GLFW_KEY_UNKNOWN)
    {
        CCKeyboardMap Key = atomic_load(&KeyList[CCInputMapKeyboardComponentGetKeycode(Component)]);
        if (((CCInputMapKeyboardComponentGetIsKeycode(Component)) ||
             (CCInputMapKeyboardComponentGetCharacter(Component) == Key.character)) &&
            (CCInputMapKeyboardComponentGetIgnoreModifier(Component) || (CCInputMapKeyboardComponentGetFlags(Component) == Key.flags)) &&
            (CCInputMapKeyboardComponentGetRepeats(Component) || (!Key.state.repeat)))
        {
            return Key.state;
        }
    }
    
    else
    {
        CCAssertLog(!CCInputMapKeyboardComponentGetIsKeycode(Component), "Must be a character match");
        
        const uint32_t Character = CCInputMapKeyboardComponentGetCharacter(Component);
        for (size_t Loop = 0; Loop < sizeof(KeyList) / sizeof(typeof(*KeyList)); Loop++)
        {
            CCKeyboardMap Key = atomic_load(&KeyList[Loop]);
            if (Key.character == Character)
            {
                if ((CCInputMapKeyboardComponentGetIgnoreModifier(Component) || (CCInputMapKeyboardComponentGetFlags(Component) == Key.flags)) &&
                    (CCInputMapKeyboardComponentGetRepeats(Component) || (!Key.state.repeat)))
                {
                    return Key.state;
                }
            }
        }
    }
    
    return (CCKeyboardState){ .timestamp = 0.0, .down = FALSE, .repeat = FALSE };
}
