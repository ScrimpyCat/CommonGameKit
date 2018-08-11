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

#ifndef CommonGameKit_AnimationComponent_h
#define CommonGameKit_AnimationComponent_h

#include <CommonGameKit/AnimationSystem.h>
#include <CommonGameKit/ComponentBase.h>
#include <CommonC/Common.h>
#include <CommonGameKit/Expression.h>


#define CC_ANIMATION_COMPONENT_ID (CCAnimationTypeNone | CC_ANIMATION_COMPONENT_FLAG)

typedef struct {
    CC_COMPONENT_INHERIT(CCComponentClass);
    CCString name;
    CCAnimationLoop loop;
    struct {
        float speed;
        double position;
        _Bool playing;
    } control;
} CCAnimationComponentClass, *CCAnimationComponentPrivate;


void CCAnimationComponentDeserializer(CCComponent Component, CCExpression Arg, _Bool Deferred);

/*!
 * @brief Initialize the animation component.
 * @param Component The component to be initialized.
 * @param id The component ID.
 */
static inline void CCAnimationComponentInitialize(CCComponent Component, CCComponentID id);

/*!
 * @brief Deallocate the animation component.
 * @param Component The component to be deallocated.
 */
static inline void CCAnimationComponentDeallocate(CCComponent Component);

/*!
 * @brief Get the name of the animation.
 * @param Component The animation component.
 * @return The animation name.
 */
static inline CCString CCAnimationComponentGetName(CCComponent Component);

/*!
 * @brief Set the name of the animation.
 * @param Component The animation component.
 * @param Name The animation name. Ownership is transferred to the component.
 */
static inline void CCAnimationComponentSetName(CCComponent Component, CCString CC_OWN(Name));

/*!
 * @brief Get the repetition type for the animation.
 * @param Component The animation component.
 * @return The animation repetition.
 */
static inline CCAnimationLoop CCAnimationComponentGetLoop(CCComponent Component);

/*!
 * @brief Set the repetition type of the animation.
 * @param Component The animation component.
 * @param Loop The animation repetition.
 */
static inline void CCAnimationComponentSetLoop(CCComponent Component, CCAnimationLoop Loop);

/*!
 * @brief Get the speed of the animation.
 * @param Component The animation component.
 * @return The animation speed.
 */
static inline float CCAnimationComponentGetSpeed(CCComponent Component);

/*!
 * @brief Set the speed of the animation.
 * @param Component The animation component.
 * @param Speed The animation speed.
 */
static inline void CCAnimationComponentSetSpeed(CCComponent Component, float Speed);

/*!
 * @brief Get the position of the animation.
 * @param Component The animation component.
 * @return The animation position.
 */
static inline double CCAnimationComponentGetPosition(CCComponent Component);

/*!
 * @brief Set the position of the animation.
 * @param Component The animation component.
 * @param Position The animation position.
 */
static inline void CCAnimationComponentSetPosition(CCComponent Component, double Position);

/*!
 * @brief Get whether the animation is playing.
 * @param Component The animation component.
 * @return Whether the animation is currently playing (TRUE), or not (FALSE).
 */
static inline _Bool CCAnimationComponentGetPlaying(CCComponent Component);

/*!
 * @brief Set whether the animation should be playing.
 * @param Component The animation component.
 * @param Playing Whether the animation should be playing.
 */
static inline void CCAnimationComponentSetPlaying(CCComponent Component, _Bool Playing);


#pragma mark -

static inline void CCAnimationComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCComponentInitialize(Component, id);
    ((CCAnimationComponentPrivate)Component)->loop = CCAnimationLoopOnce;
    ((CCAnimationComponentPrivate)Component)->name = 0;
    ((CCAnimationComponentPrivate)Component)->control.speed = 1.0f;
    ((CCAnimationComponentPrivate)Component)->control.position = 0.0f;
    ((CCAnimationComponentPrivate)Component)->control.playing = FALSE;
}

static inline void CCAnimationComponentDeallocate(CCComponent Component)
{
    if (((CCAnimationComponentPrivate)Component)->name) CCStringDestroy(((CCAnimationComponentPrivate)Component)->name);
    CCComponentDeallocate(Component);
}

static inline CCString CCAnimationComponentGetName(CCComponent Component)
{
    return ((CCAnimationComponentPrivate)Component)->name;
}

static inline void CCAnimationComponentSetName(CCComponent Component, CCString Name)
{
    if (((CCAnimationComponentPrivate)Component)->name) CCStringDestroy(((CCAnimationComponentPrivate)Component)->name);
    
    ((CCAnimationComponentPrivate)Component)->name = Name;
}

static inline CCAnimationLoop CCAnimationComponentGetLoop(CCComponent Component)
{
    return ((CCAnimationComponentPrivate)Component)->loop;
}

static inline void CCAnimationComponentSetLoop(CCComponent Component, CCAnimationLoop Loop)
{
    ((CCAnimationComponentPrivate)Component)->loop = Loop;
}

static inline float CCAnimationComponentGetSpeed(CCComponent Component)
{
    return ((CCAnimationComponentPrivate)Component)->control.speed;
}

static inline void CCAnimationComponentSetSpeed(CCComponent Component, float Speed)
{
    ((CCAnimationComponentPrivate)Component)->control.speed = Speed;
}

static inline double CCAnimationComponentGetPosition(CCComponent Component)
{
    return ((CCAnimationComponentPrivate)Component)->control.position;
}

static inline void CCAnimationComponentSetPosition(CCComponent Component, double Position)
{
    ((CCAnimationComponentPrivate)Component)->control.position = Position;
}

static inline _Bool CCAnimationComponentGetPlaying(CCComponent Component)
{
    return ((CCAnimationComponentPrivate)Component)->control.playing;
}

static inline void CCAnimationComponentSetPlaying(CCComponent Component, _Bool Playing)
{
    ((CCAnimationComponentPrivate)Component)->control.playing = Playing;
}

#endif
