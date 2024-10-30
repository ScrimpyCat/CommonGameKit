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
#include "Callbacks.h"

typedef struct {
    CCKeyboardModifier flags : 4;
    _Bool down : 1;
    uint64_t timestamp : 59;
} CC_PACKED CCMouseButtonStatus;

_Static_assert(sizeof(CCMouseButtonStatus) == 8, "Exceeds 8 bytes");

static inline uint64_t CCMouseButtonRealTimestamp(CCMouseButtonStatus Mouse)
{
    const uint64_t Timestamp = CCTimestamp();
    const uint64_t BaseTimestamp = Timestamp & ~0x7ffffffffffffff;
    const uint64_t CurrentRelative = Timestamp & 0x7ffffffffffffff;
    
    const uint64_t AdjustedTimestamp = CurrentRelative >= Mouse.timestamp ? (BaseTimestamp | Mouse.timestamp) : ((BaseTimestamp - 0x800000000000000) | Mouse.timestamp);
    
    return AdjustedTimestamp * 1000;
}

static inline CCMouseButtonState CCMouseButtonStateFromStatus(CCMouseButtonStatus Mouse, uint64_t Timestamp)
{
    return (CCMouseButtonState){
        .timestamp = Timestamp,
        .down = Mouse.down
    };
}

static struct {
    _Atomic(CCVector2D) position;
    _Atomic(CCMouseButtonStatus) buttons[CCMouseButtonCount];
} Mouse;

CCConcurrentCircularBuffer(CCMouseEvent, CC_MOUSE_EVENT_MAX) CCMouseInputEventBuffer = CC_CONCURRENT_CIRCULAR_BUFFER_INIT;

void CCMouseDropInput(CCCollection(CCString) Files)
{
    CCAssertLog(Files, "Files must not be null");
        
    const CCMouseEvent Event = { .type = CCMouseEventTypeDrop, .timestamp = CCTimestamp(), .position = atomic_load_explicit(&Mouse.position, memory_order_relaxed), .drop = { .files = CCRetain(Files) } };
    if (!CCConcurrentCircularBufferAddItem(&CCMouseInputEventBuffer, &Event)) CCCollectionDestroy(Files);
}

void CCMouseScrollInput(CCVector2D ScrollDelta)
{
    const CCMouseEvent Event = { .type = CCMouseEventTypeScroll, .timestamp = CCTimestamp(), .position = atomic_load_explicit(&Mouse.position, memory_order_relaxed), .scroll = { .delta = ScrollDelta } };
    CCConcurrentCircularBufferAddItem(&CCMouseInputEventBuffer, &Event);
}

void CCMouseButtonInput(CCMouseButton Button, CCKeyboardAction Action, CCKeyboardModifier Mods)
{
    if (Button != CCMouseButtonUnknown)
    {
        const CCMouseButtonEvent ButtonEvent = { .button = Button, .flags = Mods, .state = { .timestamp = CCTimestamp(), .down = Action == CCKeyboardActionPress } };
        
        atomic_store_explicit(&Mouse.buttons[Button], (CCMouseButtonStatus){ .flags = Mods, .down = ButtonEvent.state.down, .timestamp = ButtonEvent.state.timestamp & 0x7ffffffffffffff }, memory_order_relaxed);
        
        const CCMouseEvent Event = { .type = CCMouseEventTypeButton, .timestamp = ButtonEvent.state.timestamp, .position = atomic_load_explicit(&Mouse.position, memory_order_relaxed), .button = ButtonEvent };
        CCConcurrentCircularBufferAddItem(&CCMouseInputEventBuffer, &Event);
    }
}

void CCMousePositionInput(CCVector2D Position)
{
    CCVector2D PreviousPosition = atomic_load_explicit(&Mouse.position, memory_order_relaxed);
    CCVector2D Delta = CCVector2Sub(Position, PreviousPosition);
    
    atomic_store_explicit(&Mouse.position, Position, memory_order_relaxed);
    
    const CCMouseEvent Event = { .type = CCMouseEventTypeMove, .timestamp = CCTimestamp(), .position = atomic_load_explicit(&Mouse.position, memory_order_relaxed), .move = { .delta = Delta } };
    CCConcurrentCircularBufferAddItem(&CCMouseInputEventBuffer, &Event);
}

void CCMouseEnterInput(_Bool Entered)
{
    const CCMouseEvent Event = { .type = CCMouseEventTypeEntered, .timestamp = CCTimestamp(), .position = atomic_load_explicit(&Mouse.position, memory_order_relaxed), .entered = Entered };
    CCConcurrentCircularBufferAddItem(&CCMouseInputEventBuffer, &Event);
}

void CCMouseStateReset(void)
{
    for (size_t Loop = 0; Loop < sizeof(Mouse.buttons) / sizeof(typeof(*Mouse.buttons)); Loop++)
    {
        atomic_store_explicit(&Mouse.buttons[Loop], ((CCMouseButtonStatus){ .timestamp = 0, .down = FALSE }), memory_order_relaxed);
    }
}

CCMouseButtonState CCMouseButtonGetStateForButton(CCMouseButton Button, CCKeyboardModifier Modifier, _Bool IgnoreModifier)
{
    if (Button != CCKeyboardKeycodeUnknown)
    {
        CCMouseButtonStatus Status = atomic_load_explicit(&Mouse.buttons[Button], memory_order_relaxed);
        
        if ((IgnoreModifier) || (Modifier == Status.flags))
        {
            return CCMouseButtonStateFromStatus(Status, CCMouseButtonRealTimestamp(Status));
        }
    }
    
    return (CCMouseButtonState){ .timestamp = 0, .down = FALSE };
}
