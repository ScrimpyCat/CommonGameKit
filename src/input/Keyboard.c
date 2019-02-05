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

#include "Keyboard.h"
#include "Window.h"
#include "Callbacks.h"
#include <stdatomic.h>
#include "InputSystem.h"
#include "InputMapKeyboardComponent.h"
#include <math.h>


static _Atomic(CCKeyboardMap) KeyList[CCKeyboardKeycodeCount];

/*
 TODO: Unsure if we can make this guarantee across all platforms or with future minor verson of GLFW.
 It looks like it should be ok on OS X (is), X11, Wayland, Mir; but not sure about Windows. Can we guarantee that the
 input sequence of events will always be keycode, followed by character (or at least character, followed by keycode).
 Must make sure it won't possibly send in another keycode inbetween.
 */
static CCKeyboardMap TempKey;
void CCKeyboardInput(CCKeyboardKeycode Keycode, CCKeyboardAction Action, CCKeyboardModifier Mods)
{
    if ((TempKey.keycode = Keycode) != CCKeyboardKeycodeUnknown)
    {
        CCAssertLog(Keycode >= 0 && Keycode < CCKeyboardKeycodeCount, "Keycode is not within bounds");
        
        TempKey.flags = Mods;
        
        if (Action == CCKeyboardActionRepeat)
        {
            CCKeyboardMap Prev = atomic_load(&KeyList[Keycode]);
            TempKey.state.timestamp = Prev.state.timestamp;
            TempKey.state.down = TRUE;
            TempKey.state.repeat = TRUE;
        }
        
        else
        {
            TempKey.state.timestamp = CCTimestamp();
            TempKey.state.down = Action == CCKeyboardActionPress;
            TempKey.state.repeat = FALSE;
        }
        
        if ((!TempKey.state.down) || (TempKey.keycode & CCKeyboardKeycodeFunctionMask))
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
                    if ((CCInputMapKeyboardComponentGetIsKeycode(*Input) && ((CCInputMapKeyboardComponentGetKeycode(*Input) == Keycode) || (CCInputMapKeyboardComponentGetKeycode(*Input) == CCInputMapKeyboardComponentKeycodeAny))) &&
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

void CCKeyboardCharInput(CCChar Codepoint, CCKeyboardModifier Mods)
{
    if (TempKey.keycode != CCKeyboardKeycodeUnknown)
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
                if (((CCInputMapKeyboardComponentGetIsKeycode(*Input) && ((CCInputMapKeyboardComponentGetKeycode(*Input) == TempKey.keycode) || (CCInputMapKeyboardComponentGetKeycode(*Input) == CCInputMapKeyboardComponentKeycodeAny))) ||
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
        atomic_store(&KeyList[Loop], ((CCKeyboardMap){ .keycode = (CCKeyboardKeycode)Loop, .state = { .timestamp = -INFINITY, .down = FALSE, .repeat = FALSE } }));
    }
}

CCKeyboardState CCKeyboardGetStateForComponent(CCComponent Component)
{
    CCAssertLog(CCComponentGetID(Component) == CC_INPUT_MAP_KEYBOARD_COMPONENT_ID, "Must be a input map keyboard component");
    
    if (CCInputMapKeyboardComponentGetKeycode(Component) != CCInputMapKeyboardComponentKeycodeAny)
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
        
        const CCChar Character = CCInputMapKeyboardComponentGetCharacter(Component);
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
