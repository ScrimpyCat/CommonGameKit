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
#include "Keyboard.h"
#include "Callbacks.h"


const CCReflectInteger CC_REFLECT(CCKeyboardModifier) = CC_REFLECT_SIGNED_INTEGER(CCKeyboardModifier, CCReflectEndianNative);

const CCReflectInteger CC_REFLECT(CCKeyboardKeycode) = CC_REFLECT_SIGNED_INTEGER(CCKeyboardKeycode, CCReflectEndianNative);


typedef struct {
    CCChar character : 21;
    CCKeyboardModifier flags : 4;
    _Bool down : 1, repeat : 1;
    uint64_t timestamp : 37;
} CC_PACKED CCKeyboardStatus;

_Static_assert(sizeof(CCKeyboardStatus) == 8, "Exceeds 8 bytes");

static inline uint64_t CCKeyboardRealTimestamp(CCKeyboardStatus Key)
{
    const uint64_t Timestamp = CCTimestamp() / 1000;
    const uint64_t BaseTimestamp = Timestamp & ~0x1fffffffff;
    const uint64_t CurrentRelative = Timestamp & 0x1fffffffff;
    
    const uint64_t AdjustedTimestamp = CurrentRelative >= Key.timestamp ? (BaseTimestamp | Key.timestamp) : ((BaseTimestamp - 0x2000000000) | Key.timestamp);
    
    return AdjustedTimestamp * 1000;
}

static inline CCKeyboardState CCKeyboardStateFromStatus(CCKeyboardStatus Key, uint64_t Timestamp)
{
    return (CCKeyboardState){
        .timestamp = Timestamp,
        .down = Key.down,
        .repeat = Key.repeat
    };
}

static inline CCKeyboardEvent CCKeyboardEventFromStatus(CCKeyboardStatus Key, CCKeyboardKeycode Keycode, uint64_t Timestamp)
{
    return (CCKeyboardEvent){
        .keycode = Keycode,
        .character = Key.character,
        .flags = Key.flags,
        .state = CCKeyboardStateFromStatus(Key, Timestamp)
    };
}

static _Atomic(CCKeyboardStatus) KeyList[CCKeyboardKeycodeCount];

CCConcurrentCircularBuffer(CCKeyboardEvent, CC_KEYBOARD_EVENT_MAX) CCKeyboardInputEventBuffer = CC_CONCURRENT_CIRCULAR_BUFFER_INIT;
CCConcurrentCircularBuffer(CCKeyboardEvent, CC_KEYBOARD_EVENT_MAX) CCKeyboardCharInputEventBuffer = CC_CONCURRENT_CIRCULAR_BUFFER_INIT;

/*
 TODO: Unsure if we can make this guarantee across all platforms or with future minor verson of GLFW.
 It looks like it should be ok on OS X (is), X11, Wayland, Mir; but not sure about Windows. Can we guarantee that the
 input sequence of events will always be keycode, followed by character (or at least character, followed by keycode).
 Must make sure it won't possibly send in another keycode inbetween.
 */
static CCKeyboardKeycode TempKeycode;
static CCKeyboardStatus TempKey;

void CCKeyboardInput(CCKeyboardKeycode Keycode, CCKeyboardAction Action, CCKeyboardModifier Mods)
{
    if ((TempKeycode = Keycode) != CCKeyboardKeycodeUnknown)
    {
        CCAssertLog(Keycode >= 0 && Keycode < CCKeyboardKeycodeCount, "Keycode is not within bounds");
        
        TempKey.flags = Mods;
        
        if (Action == CCKeyboardActionRepeat)
        {
            CCKeyboardStatus Prev = atomic_load_explicit(&KeyList[Keycode], memory_order_relaxed);
            TempKey.timestamp = Prev.timestamp;
            TempKey.down = TRUE;
            TempKey.repeat = TRUE;
        }
        
        else
        {
            TempKey.timestamp = (CCTimestamp() / 1000) & 0x1fffffffff;
            TempKey.down = Action == CCKeyboardActionPress;
            TempKey.repeat = FALSE;
        }
        
        if ((!TempKey.down) || (Keycode & CCKeyboardKeycodeFunctionMask))
        {
            TempKey.character = 0;
            atomic_store_explicit(&KeyList[Keycode], TempKey, memory_order_relaxed);
            
            CCKeyboardEvent Event = CCKeyboardEventFromStatus(TempKey, Keycode, CCTimestamp());
            CCConcurrentCircularBufferAddItem(&CCKeyboardInputEventBuffer, &Event);
        }
    }
}

void CCKeyboardCharInput(CCChar Codepoint, CCKeyboardModifier Mods)
{
    if (TempKeycode != CCKeyboardKeycodeUnknown)
    {
        TempKey.character = Codepoint;
        atomic_store_explicit(&KeyList[TempKeycode], TempKey, memory_order_relaxed);
        
        CCKeyboardEvent Event = CCKeyboardEventFromStatus(TempKey, TempKeycode, CCTimestamp());
        CCConcurrentCircularBufferAddItem(&CCKeyboardCharInputEventBuffer, &Event);
    }
}

void CCKeyboardStateReset(void)
{
    for (size_t Loop = 0; Loop < sizeof(KeyList) / sizeof(typeof(*KeyList)); Loop++)
    {
        atomic_store_explicit(&KeyList[Loop], ((CCKeyboardStatus){ .timestamp = 0, .down = FALSE, .repeat = FALSE }), memory_order_relaxed);
    }
}

CCKeyboardState CCKeyboardGetStateForKeycode(CCKeyboardKeycode Keycode, CCChar MatchCharacter, CCKeyboardModifier Modifier, _Bool IgnoreModifier, _Bool Repeats)
{
    if (Keycode != CCKeyboardKeycodeUnknown)
    {
        CCKeyboardStatus Key = atomic_load_explicit(&KeyList[Keycode], memory_order_relaxed);
        
        if (((MatchCharacter == 0) || (MatchCharacter == Key.character)) &&
            ((IgnoreModifier) || (Modifier == Key.flags)) &&
            ((Repeats) || (!Key.repeat)))
        {
            return CCKeyboardStateFromStatus(Key, CCKeyboardRealTimestamp(Key));
        }
    }
    
    return (CCKeyboardState){ .timestamp = 0, .down = FALSE, .repeat = FALSE };
}

CCKeyboardState CCKeyboardGetStateForCharacter(CCChar Character, CCKeyboardModifier Modifier, _Bool IgnoreModifier, _Bool Repeats)
{
    for (size_t Loop = 0; Loop < sizeof(KeyList) / sizeof(typeof(*KeyList)); Loop++)
    {
        CCKeyboardStatus Key = atomic_load_explicit(&KeyList[Loop], memory_order_relaxed);
        
        if (Character == Key.character)
        {
            if (((IgnoreModifier) || (Modifier == Key.flags)) &&
                ((Repeats) || (!Key.repeat)))
            {
                return CCKeyboardStateFromStatus(Key, CCKeyboardRealTimestamp(Key));
            }
        }
    }
    
    return (CCKeyboardState){ .timestamp = 0, .down = FALSE, .repeat = FALSE };
}
