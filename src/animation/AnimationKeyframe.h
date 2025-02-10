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

#ifndef CommonGameKit_AnimationKeyframe_h
#define CommonGameKit_AnimationKeyframe_h

#include <CommonGameKit/Playback.h>

typedef struct {
    size_t count;
    const CCPlaybackTime *frames;
    struct {
        CCPlaybackTime time;
        size_t count;
        const size_t *indexes;
    } jump;
} CCAnimationKeyframe;

/*!
 * @brief Get the length for the entire keyframe animation.
 * @param Animation The keyframe animation to get the length of.
 * @return The full length of the animation.
 */
static inline CCPlaybackTime CCAnimationKeyframeGetLength(const CCAnimationKeyframe *Animation);

/*!
 * @brief Get the length for the specified number of keyframes in the animation.
 * @param Animation The keyframe animation to get the frame length of.
 * @param FrameCount The number of frames to get the length of.
 * @return The length of the animation frames.
 */
static inline CCPlaybackTime CCAnimationKeyframeGetFrameLength(const CCAnimationKeyframe *Animation, size_t FrameCount);

/*!
 * @brief Get the frame at the specified time in the keyframe animation.
 * @param Animation The keyframe animation to get the frame of.
 * @param Time The time to get the frame for.
 * @param Finished A pointer to where to store if the animaton has finished (@b Time has reached or passed animation length). May
 *                 be NULL.
 *
 * @return The frame index at the given time in the animation.
 */
static inline size_t CCAnimationKeyframeGetFrame(const CCAnimationKeyframe *Animation, CCPlaybackTime Time, _Bool *Finished);

/*!
 * @brief Get the min/max/avg frane times for the keyframe animation.
 * @param Animation The keyframe animation to get the frame times for.
 * @param Min The minimum amount of time a frame will take. May be null.
 * @param Max The maximum amount of time a frame will take. May be null.
 * @param Avg The average amount of time a frame will take. May be null.
 */
static inline void CCAnimationKeyframeGetFrameTimes(const CCAnimationKeyframe *Animation, CCPlaybackTime *Min, CCPlaybackTime *Max, CCPlaybackTime *Avg);

/*!
 * @brief Get the number of samples in keyframe animation when incrementing at the specified interval.
 * @param Animation The keyframe animation to get the frame times for.
 * @param Interval The amount of time between each sample.
 * @return The number of samples.
 */
static inline size_t CCAnimationKeyframeGetSampleCount(const CCAnimationKeyframe *Animation, CCPlaybackTime Interval);

/*!
 * @brief Generate the jump indexes for the keyframe animation.
 * @param Animation The keyframe animation to generate the jump indexes for.
 * @param Indexes A pointer to an array of @b Count, where the jump indexes will be stored.
 * @param Count The number of jump indexes to generate.
 */
static inline void CCAnimationKeyframeGenerateJumpIndexes(const CCAnimationKeyframe *Animation, size_t *Indexes, size_t Count);

#pragma mark -

static inline CCPlaybackTime CCAnimationKeyframeGetLength(const CCAnimationKeyframe *Animation)
{
    CCAssertLog(Animation, "Animation must not be null");
    
    return CCAnimationKeyframeGetFrameLength(Animation, Animation->count);
}

static inline CCPlaybackTime CCAnimationKeyframeGetFrameLength(const CCAnimationKeyframe *Animation, size_t FrameCount)
{
    CCAssertLog(Animation, "Animation must not be null");
    CCAssertLog(FrameCount <= Animation->count, "FrameIndex must not exceed animation count");
    
    return FrameCount ? Animation->frames[FrameCount - 1] : 0;
}

static inline size_t CCAnimationKeyframeGetFrame(const CCAnimationKeyframe *Animation, CCPlaybackTime Time, _Bool *Finished)
{
    CCAssertLog(Animation, "Animation must not be null");
    CCAssertLog(Animation->jump.count, "Animation jump count must not be 0");
    
    const size_t JumpIndex = CCMin(Time / Animation->jump.time, Animation->jump.count - 1);
    
    size_t FrameIndex = Animation->jump.indexes[JumpIndex];
    
    while (Animation->frames[FrameIndex] <= Time)
    {
        FrameIndex++;
        
        if (FrameIndex >= Animation->count)
        {
            if (Finished) *Finished = TRUE;
            
            return FrameIndex - 1;
        }
    }
    
    if (Finished) *Finished = FALSE;
    
    return FrameIndex;
}

static inline void CCAnimationKeyframeGetFrameTimes(const CCAnimationKeyframe *Animation, CCPlaybackTime *Min, CCPlaybackTime *Max, CCPlaybackTime *Avg)
{
    CCAssertLog(Animation, "Animation must not be null");
    
    CCPlaybackTime Smallest = -1, Largest = 0, Prev = 0;
    
    for (size_t Loop = 0; Loop < Animation->count; Loop++)
    {
        CCPlaybackTime Time = Animation->frames[Loop] - Prev;
        
        if (Time < Smallest) Smallest = Time;
        if (Time > Largest) Largest = Time;
        
        Prev = Animation->frames[Loop];
    }
    
    if (Avg) *Avg = CCAnimationKeyframeGetLength(Animation) / Animation->count;
    if (Min) *Min = Smallest;
    if (Max) *Max = Largest;
}

static inline size_t CCAnimationKeyframeGetSampleCount(const CCAnimationKeyframe *Animation, CCPlaybackTime Interval)
{
    CCAssertLog(Animation, "Animation must not be null");
    CCAssertLog(Interval, "Interval must not be 0");
    
    return CCAnimationKeyframeGetLength(Animation) / Interval;
}

static inline void CCAnimationKeyframeGenerateJumpIndexes(const CCAnimationKeyframe *Animation, size_t *Indexes, size_t Count)
{
    CCAssertLog(Animation, "Animation must not be null");
    CCAssertLog(Indexes, "Indexes must not be null");
    CCAssertLog(Count, "Count must not be 0");
    
    const CCPlaybackTime SampleTime = CCAnimationKeyframeGetLength(Animation) / Count;
    CCPlaybackTime Time = 0;
    
    for (size_t Loop = 0, Index = 0; Loop < Count; Loop++, Time += SampleTime)
    {
        while (Animation->frames[Index] <= Time) Index++;
        
        Indexes[Loop] = Index;
    }
}

#endif
