/*
 *  Copyright (c) 2025, Stefan Johnson
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


#include <CommonC/Template.h>

#define CCAnimationInterpolateGetInterpolatedData_T(t) CC_TEMPLATE_REF(CCAnimationInterpolateGetInterpolatedData, PRIVATE(Tdata, data), const PTYPE(t *), CCPlaybackTime)
#define CCAnimationInterpolateGetReverseInterpolatedData_T(t) CC_TEMPLATE_REF(CCAnimationInterpolateGetReverseInterpolatedData, PRIVATE(Tdata, data), const PTYPE(t *), CCPlaybackTime)
#define CCAnimationInterpolateGetDirectionalInterpolatedData_T(t) CC_TEMPLATE_REF(CCAnimationInterpolateGetDirectionalInterpolatedData, PRIVATE(Tdata, data), const PTYPE(t *), CCPlaybackTime, CCPlaybackDirection)

#undef CCAnimationInterpolator
#undef CCAnimationInterpolate


CC_TEMPLATE(static CC_FORCE_INLINE PRIVATE(Tdata, data), CCAnimationInterpolateGetInterpolatedData, (const PTYPE(T *) Animation, CCPlaybackTime Time));
CC_TEMPLATE(static CC_FORCE_INLINE PRIVATE(Tdata, data), CCAnimationInterpolateGetReverseInterpolatedData, (const PTYPE(T *) Animation, CCPlaybackTime Time));
CC_TEMPLATE(static CC_FORCE_INLINE PRIVATE(Tdata, data), CCAnimationInterpolateGetDirectionalInterpolatedData, (const PTYPE(T *) Animation, CCPlaybackTime Time, CCPlaybackDirection Direction));

#pragma mark -

CC_TEMPLATE(static CC_FORCE_INLINE PRIVATE(Tdata, data), CCAnimationInterpolateGetInterpolatedData, (const PTYPE(T *) Animation, CCPlaybackTime Time))
{
    CCAssertLog(Animation, "Animation must not be null");
    
    size_t Index = CCAnimationKeyframeGetFrame(&Animation->keyframe, Time, NULL);
    
    const CCPlaybackTime PrevTime = Index ? Animation->keyframe.frames[Index - 1] : 0;
    const CCPlaybackTime EndTime = Animation->keyframe.frames[Index];
    const CCPlaybackTime RelativeEndTime = EndTime - PrevTime;
    CCPlaybackTime Remainder = Time - PrevTime;
    
    size_t NextIndex = Index + 1;
    
    if (NextIndex == Animation->keyframe.count)
    {
        NextIndex = Index;
        Remainder = CCMin(Remainder, RelativeEndTime);
    }
    
    Tdata Data;
    Animation->interpolator(&Animation->data[Index], &Animation->data[NextIndex], Remainder, RelativeEndTime, &Data);
    
    return Data;
}

CC_TEMPLATE(static CC_FORCE_INLINE PRIVATE(Tdata, data), CCAnimationInterpolateGetReverseInterpolatedData, (const PTYPE(T *) Animation, CCPlaybackTime Time))
{
    CCAssertLog(Animation, "Animation must not be null");
    
    size_t Index = CCAnimationKeyframeGetFrame(&Animation->keyframe, Time, NULL);
    
    CCPlaybackTime Remainder = Time;
    
    if (Index) Remainder -= Animation->keyframe.frames[Index - 1];
    
    const CCPlaybackTime EndTime = Animation->keyframe.frames[Index];
    
    size_t PrevIndex = Index + 1;
    
    if (PrevIndex == Animation->keyframe.count)
    {
        PrevIndex = Index;
        Remainder = CCMin(Remainder, EndTime);
    }
    
    Tdata Data;
    Animation->interpolator(&Animation->data[PrevIndex], &Animation->data[Index], EndTime - Remainder, EndTime, &Data);
    
    return Data;
}

CC_TEMPLATE(static CC_FORCE_INLINE PRIVATE(Tdata, data), CCAnimationInterpolateGetDirectionalInterpolatedData, (const PTYPE(T *) Animation, CCPlaybackTime Time, CCPlaybackDirection Direction))
{
    CCAssertLog(Animation, "Animation must not be null");
    
    if (Direction == CCPlaybackDirectionForward) return CCAnimationInterpolateGetInterpolatedData_T(T)(Animation, Time);
    else return CCAnimationInterpolateGetReverseInterpolatedData_T(T)(Animation, Time);
}

#define CCAnimationInterpolator(type) CC_ANIMATION_INTERPOLATOR(type)

#define CCAnimationInterpolate(type) CC_ANIMATION_INTERPOLATE(type)
