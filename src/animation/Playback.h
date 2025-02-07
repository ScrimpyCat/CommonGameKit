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

#endif
