/*
 *  Copyright (c) 2015, 2025, Stefan Johnson
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

#ifndef CommonGameKit_Playback_h
#define CommonGameKit_Playback_h

#include <CommonGameKit/Base.h>

typedef uint64_t CCPlaybackTime;

typedef CC_ENUM(CCPlaybackLoop, uint8_t) {
    CCPlaybackLoopOnce,
    CCPlaybackLoopRepeatRoll,
    CCPlaybackLoopRepeatFlip
};

typedef CC_ENUM(CCPlaybackDirection, int8_t) {
    CCPlaybackDirectionForward = 1,
    CCPlaybackDirectionBackward = -1,
    CCPlaybackDirectionReverse = CCPlaybackDirectionBackward
};

typedef struct {
    CCPlaybackTime position;
    CCPlaybackTime start;
    CCPlaybackTime end;
    CCDecimal speed;
    CCPlaybackLoop loop;
    CCPlaybackDirection direction;
    _Bool playing;
} CCPlayback;

/*!
 * @brief Advance the playback timer.
 * @note All uses of @b CCPlaybackTime are assumed to be in the same units.
 * @param Playback The playback timer to advance.
 * @param Time The amount of time to advance the playback timer by.
 */
static inline void CCPlaybackAdvance(CCPlayback *Playback, CCPlaybackTime Time);

#pragma mark -

static inline void CCPlaybackAdvance(CCPlayback *Playback, CCPlaybackTime Time)
{
    CCAssertLog(Playback, "Playback must not be null");
    CCAssertLog(Playback->start <= Playback->end, "Playback start must not exceed playback end");
    
    if (Playback->playing)
    {
        CCPlaybackTime Position = CCClamp(Playback->position, Playback->start, Playback->end);
        CCPlaybackTime Advance = CCDecimalMultiplyU64(Time, Playback->speed);
        const CCPlaybackTime Length = (Playback->end - Playback->start) + (Playback->loop != CCPlaybackLoopRepeatFlip);
        
        CCPlaybackTime Count = 0;
        
        if (Length)
        {
            Count = Advance / Length;
            
            Advance %= Length;
        }
        
        else Advance = 0;
        
        if (Playback->direction == CCPlaybackDirectionForward)
        {
            const CCPlaybackTime Diff = Playback->end - Position;
            
            if (Diff < Advance)
            {
                switch (Playback->loop)
                {
                    case CCPlaybackLoopOnce:
                        Position = Playback->end;
                        break;
                        
                    case CCPlaybackLoopRepeatRoll:
                        Position = Playback->start + (Advance - Diff) - 1;
                        break;
                        
                    case CCPlaybackLoopRepeatFlip:
                        if ((Count + 1) & 1) Playback->direction *= CCPlaybackDirectionReverse;
                        
                        if (Playback->direction == CCPlaybackDirectionForward) Position = Playback->start + (Advance - Diff);
                        else Position = Playback->end - (Advance - Diff);
                        break;
                }
            }
            
            else if (Count)
            {
                switch (Playback->loop)
                {
                    case CCPlaybackLoopOnce:
                        Position = Playback->end;
                        break;
                        
                    case CCPlaybackLoopRepeatRoll:
                        Position += Advance;
                        break;
                        
                    case CCPlaybackLoopRepeatFlip:
                        if (Count & 1) Playback->direction *= CCPlaybackDirectionReverse;
                        
                        if (Playback->direction == CCPlaybackDirectionForward) Position += Advance;
                        else Position = Playback->end - (Position + (Advance - Playback->start));
                        break;
                }
            }
            
            else Position += Advance;
        }
        
        else
        {
            const CCPlaybackTime Diff = Position - Playback->start;
            
            if (Diff < Advance)
            {
                switch (Playback->loop)
                {
                    case CCPlaybackLoopOnce:
                        Position = Playback->start;
                        break;
                        
                    case CCPlaybackLoopRepeatRoll:
                        Position = Playback->end - (Advance - Diff) + 1;
                        break;
                        
                    case CCPlaybackLoopRepeatFlip:
                        if ((Count + 1) & 1) Playback->direction *= CCPlaybackDirectionReverse;
                        
                        if (Playback->direction == CCPlaybackDirectionForward) Position = Playback->start + (Advance - Diff);
                        else Position = Playback->end - (Advance - Diff);
                        break;
                }
            }
            
            else if (Count)
            {
                switch (Playback->loop)
                {
                    case CCPlaybackLoopOnce:
                        Position = Playback->start;
                        break;
                        
                    case CCPlaybackLoopRepeatRoll:
                        Position -= Advance;
                        break;
                        
                    case CCPlaybackLoopRepeatFlip:
                        if (Count & 1) Playback->direction *= CCPlaybackDirectionReverse;
                        
                        if (Playback->direction == CCPlaybackDirectionForward) Position = Playback->end - (Position - (Advance + Playback->start));
                        else Position -= Advance;
                        break;
                }
            }
            
            else Position -= Advance;
        }
        
        Playback->position = Position;
    }
}

#pragma - Template Types

#define CC_TYPE_CCPlaybackTime(...) CCPlaybackTime
#define CC_TYPE_0_CCPlaybackTime CC_TYPE_CCPlaybackTime,
#define CC_TYPE_1_CCPlaybackTime CC_TYPE_CCPlaybackTime,
#define CC_TYPE_2_CCPlaybackTime CC_TYPE_CCPlaybackTime,
#define CC_TYPE_3_CCPlaybackTime CC_TYPE_CCPlaybackTime,
#define CC_TYPE_4_CCPlaybackTime CC_TYPE_CCPlaybackTime,
#define CC_TYPE_5_CCPlaybackTime CC_TYPE_CCPlaybackTime,
#define CC_TYPE_6_CCPlaybackTime CC_TYPE_CCPlaybackTime,
#define CC_TYPE_7_CCPlaybackTime CC_TYPE_CCPlaybackTime,
#define CC_TYPE_8_CCPlaybackTime CC_TYPE_CCPlaybackTime,
#define CC_TYPE_9_CCPlaybackTime CC_TYPE_CCPlaybackTime,

#define CC_PRESERVE_CC_TYPE_CCPlaybackTime CC_TYPE_CCPlaybackTime

#define CC_TYPE_DECL_CCPlaybackTime(...) CCPlaybackTime, __VA_ARGS__
#define CC_TYPE_DECL_0_CCPlaybackTime CC_TYPE_DECL_CCPlaybackTime,
#define CC_TYPE_DECL_1_CCPlaybackTime CC_TYPE_DECL_CCPlaybackTime,
#define CC_TYPE_DECL_2_CCPlaybackTime CC_TYPE_DECL_CCPlaybackTime,
#define CC_TYPE_DECL_3_CCPlaybackTime CC_TYPE_DECL_CCPlaybackTime,
#define CC_TYPE_DECL_4_CCPlaybackTime CC_TYPE_DECL_CCPlaybackTime,
#define CC_TYPE_DECL_5_CCPlaybackTime CC_TYPE_DECL_CCPlaybackTime,
#define CC_TYPE_DECL_6_CCPlaybackTime CC_TYPE_DECL_CCPlaybackTime,
#define CC_TYPE_DECL_7_CCPlaybackTime CC_TYPE_DECL_CCPlaybackTime,
#define CC_TYPE_DECL_8_CCPlaybackTime CC_TYPE_DECL_CCPlaybackTime,
#define CC_TYPE_DECL_9_CCPlaybackTime CC_TYPE_DECL_CCPlaybackTime,

#define CC_MANGLE_TYPE_0_CCPlaybackTime CCPlaybackTime
#define CC_MANGLE_TYPE_1_CCPlaybackTime CCPlaybackTime
#define CC_MANGLE_TYPE_2_CCPlaybackTime CCPlaybackTime
#define CC_MANGLE_TYPE_3_CCPlaybackTime CCPlaybackTime
#define CC_MANGLE_TYPE_4_CCPlaybackTime CCPlaybackTime
#define CC_MANGLE_TYPE_5_CCPlaybackTime CCPlaybackTime
#define CC_MANGLE_TYPE_6_CCPlaybackTime CCPlaybackTime
#define CC_MANGLE_TYPE_7_CCPlaybackTime CCPlaybackTime
#define CC_MANGLE_TYPE_8_CCPlaybackTime CCPlaybackTime
#define CC_MANGLE_TYPE_9_CCPlaybackTime CCPlaybackTime


#define CC_TYPE_CCPlaybackDirection(...) CCPlaybackDirection
#define CC_TYPE_0_CCPlaybackDirection CC_TYPE_CCPlaybackDirection,
#define CC_TYPE_1_CCPlaybackDirection CC_TYPE_CCPlaybackDirection,
#define CC_TYPE_2_CCPlaybackDirection CC_TYPE_CCPlaybackDirection,
#define CC_TYPE_3_CCPlaybackDirection CC_TYPE_CCPlaybackDirection,
#define CC_TYPE_4_CCPlaybackDirection CC_TYPE_CCPlaybackDirection,
#define CC_TYPE_5_CCPlaybackDirection CC_TYPE_CCPlaybackDirection,
#define CC_TYPE_6_CCPlaybackDirection CC_TYPE_CCPlaybackDirection,
#define CC_TYPE_7_CCPlaybackDirection CC_TYPE_CCPlaybackDirection,
#define CC_TYPE_8_CCPlaybackDirection CC_TYPE_CCPlaybackDirection,
#define CC_TYPE_9_CCPlaybackDirection CC_TYPE_CCPlaybackDirection,

#define CC_PRESERVE_CC_TYPE_CCPlaybackDirection CC_TYPE_CCPlaybackDirection

#define CC_TYPE_DECL_CCPlaybackDirection(...) CCPlaybackDirection, __VA_ARGS__
#define CC_TYPE_DECL_0_CCPlaybackDirection CC_TYPE_DECL_CCPlaybackDirection,
#define CC_TYPE_DECL_1_CCPlaybackDirection CC_TYPE_DECL_CCPlaybackDirection,
#define CC_TYPE_DECL_2_CCPlaybackDirection CC_TYPE_DECL_CCPlaybackDirection,
#define CC_TYPE_DECL_3_CCPlaybackDirection CC_TYPE_DECL_CCPlaybackDirection,
#define CC_TYPE_DECL_4_CCPlaybackDirection CC_TYPE_DECL_CCPlaybackDirection,
#define CC_TYPE_DECL_5_CCPlaybackDirection CC_TYPE_DECL_CCPlaybackDirection,
#define CC_TYPE_DECL_6_CCPlaybackDirection CC_TYPE_DECL_CCPlaybackDirection,
#define CC_TYPE_DECL_7_CCPlaybackDirection CC_TYPE_DECL_CCPlaybackDirection,
#define CC_TYPE_DECL_8_CCPlaybackDirection CC_TYPE_DECL_CCPlaybackDirection,
#define CC_TYPE_DECL_9_CCPlaybackDirection CC_TYPE_DECL_CCPlaybackDirection,

#define CC_MANGLE_TYPE_0_CCPlaybackDirection CCPlaybackDirection
#define CC_MANGLE_TYPE_1_CCPlaybackDirection CCPlaybackDirection
#define CC_MANGLE_TYPE_2_CCPlaybackDirection CCPlaybackDirection
#define CC_MANGLE_TYPE_3_CCPlaybackDirection CCPlaybackDirection
#define CC_MANGLE_TYPE_4_CCPlaybackDirection CCPlaybackDirection
#define CC_MANGLE_TYPE_5_CCPlaybackDirection CCPlaybackDirection
#define CC_MANGLE_TYPE_6_CCPlaybackDirection CCPlaybackDirection
#define CC_MANGLE_TYPE_7_CCPlaybackDirection CCPlaybackDirection
#define CC_MANGLE_TYPE_8_CCPlaybackDirection CCPlaybackDirection
#define CC_MANGLE_TYPE_9_CCPlaybackDirection CCPlaybackDirection


#define CC_TYPE_CCPlaybackLoop(...) CCPlaybackLoop
#define CC_TYPE_0_CCPlaybackLoop CC_TYPE_CCPlaybackLoop,
#define CC_TYPE_1_CCPlaybackLoop CC_TYPE_CCPlaybackLoop,
#define CC_TYPE_2_CCPlaybackLoop CC_TYPE_CCPlaybackLoop,
#define CC_TYPE_3_CCPlaybackLoop CC_TYPE_CCPlaybackLoop,
#define CC_TYPE_4_CCPlaybackLoop CC_TYPE_CCPlaybackLoop,
#define CC_TYPE_5_CCPlaybackLoop CC_TYPE_CCPlaybackLoop,
#define CC_TYPE_6_CCPlaybackLoop CC_TYPE_CCPlaybackLoop,
#define CC_TYPE_7_CCPlaybackLoop CC_TYPE_CCPlaybackLoop,
#define CC_TYPE_8_CCPlaybackLoop CC_TYPE_CCPlaybackLoop,
#define CC_TYPE_9_CCPlaybackLoop CC_TYPE_CCPlaybackLoop,

#define CC_PRESERVE_CC_TYPE_CCPlaybackLoop CC_TYPE_CCPlaybackLoop

#define CC_TYPE_DECL_CCPlaybackLoop(...) CCPlaybackLoop, __VA_ARGS__
#define CC_TYPE_DECL_0_CCPlaybackLoop CC_TYPE_DECL_CCPlaybackLoop,
#define CC_TYPE_DECL_1_CCPlaybackLoop CC_TYPE_DECL_CCPlaybackLoop,
#define CC_TYPE_DECL_2_CCPlaybackLoop CC_TYPE_DECL_CCPlaybackLoop,
#define CC_TYPE_DECL_3_CCPlaybackLoop CC_TYPE_DECL_CCPlaybackLoop,
#define CC_TYPE_DECL_4_CCPlaybackLoop CC_TYPE_DECL_CCPlaybackLoop,
#define CC_TYPE_DECL_5_CCPlaybackLoop CC_TYPE_DECL_CCPlaybackLoop,
#define CC_TYPE_DECL_6_CCPlaybackLoop CC_TYPE_DECL_CCPlaybackLoop,
#define CC_TYPE_DECL_7_CCPlaybackLoop CC_TYPE_DECL_CCPlaybackLoop,
#define CC_TYPE_DECL_8_CCPlaybackLoop CC_TYPE_DECL_CCPlaybackLoop,
#define CC_TYPE_DECL_9_CCPlaybackLoop CC_TYPE_DECL_CCPlaybackLoop,

#define CC_MANGLE_TYPE_0_CCPlaybackLoop CCPlaybackLoop
#define CC_MANGLE_TYPE_1_CCPlaybackLoop CCPlaybackLoop
#define CC_MANGLE_TYPE_2_CCPlaybackLoop CCPlaybackLoop
#define CC_MANGLE_TYPE_3_CCPlaybackLoop CCPlaybackLoop
#define CC_MANGLE_TYPE_4_CCPlaybackLoop CCPlaybackLoop
#define CC_MANGLE_TYPE_5_CCPlaybackLoop CCPlaybackLoop
#define CC_MANGLE_TYPE_6_CCPlaybackLoop CCPlaybackLoop
#define CC_MANGLE_TYPE_7_CCPlaybackLoop CCPlaybackLoop
#define CC_MANGLE_TYPE_8_CCPlaybackLoop CCPlaybackLoop
#define CC_MANGLE_TYPE_9_CCPlaybackLoop CCPlaybackLoop

#endif
