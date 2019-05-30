/*
 *  Copyright (c) 2018, Stefan Johnson
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

#ifndef CommonGameKit_ScriptableInterfaceDynamicFieldComponent_h
#define CommonGameKit_ScriptableInterfaceDynamicFieldComponent_h

#include <CommonGameKit/ScriptableInterfaceSystem.h>
#include <CommonGameKit/ComponentBase.h>
#include <CommonGameKit/Expression.h>
#include <CommonC/Common.h>
#include <stdatomic.h>


#define CC_SCRIPTABLE_INTERFACE_DYNAMIC_FIELD_COMPONENT_ID (CCScriptableInterfaceTypeDynamicField | CC_SCRIPTABLE_INTERFACE_COMPONENT_FLAG)

extern const CCString CCScriptableInterfaceDynamicFieldComponentName;

typedef struct {
    CC_COMPONENT_INHERIT(CCComponentClass);
    CCComponent target;
    CCExpression field;
    CCExpression refState;
    CCExpression refRoot;
    _Atomic(CCExpression) state;
} CCScriptableInterfaceDynamicFieldComponentClass, *CCScriptableInterfaceDynamicFieldComponentPrivate;


void CCScriptableInterfaceDynamicFieldComponentRegister(void);
void CCScriptableInterfaceDynamicFieldComponentDeregister(void);
void CCScriptableInterfaceDynamicFieldComponentDeserializer(CCComponent Component, CCExpression Arg, _Bool Deferred);

/*!
 * @brief Initialize the dynamic field component.
 * @param Component The component to be initialized.
 * @param id The component ID.
 */
static inline void CCScriptableInterfaceDynamicFieldComponentInitialize(CCComponent Component, CCComponentID id);

/*!
 * @brief Deallocate the dynamic field component.
 * @param Component The component to be deallocated.
 */
static inline void CCScriptableInterfaceDynamicFieldComponentDeallocate(CCComponent Component);

/*!
 * @brief Get the target component with the dynamic field.
 * @param Component The dynamic field component.
 * @return The target component.
 */
static inline CCComponent CCScriptableInterfaceDynamicFieldComponentGetTarget(CCComponent Component);

/*!
 * @brief Set the target component that will have the dynamic field.
 * @param Component The dynamic field component.
 * @param Target The target component.
 */
static inline void CCScriptableInterfaceDynamicFieldComponentSetTarget(CCComponent Component, CCComponent CC_RETAIN(Target));

/*!
 * @brief Get the field expression of the dynamic field.
 * @param Component The dynamic field component.
 * @return The field expression.
 */
static inline CCExpression CCScriptableInterfaceDynamicFieldComponentGetField(CCComponent Component);

/*!
 * @brief Set the field expression of the dynamic field.
 * @param Component The dynamic field component.
 * @param Field The field expression. Ownership is transferred to the component
 */
static inline void CCScriptableInterfaceDynamicFieldComponentSetField(CCComponent Component, CCExpression CC_OWN(Field));

/*!
 * @brief Get the reference root of the dynamic field.
 * @param Component The dynamic field component.
 * @return The reference root.
 */
static inline CCExpression CCScriptableInterfaceDynamicFieldComponentGetReferenceRoot(CCComponent Component);

/*!
 * @brief Get the reference state of the dynamic field.
 * @param Component The dynamic field component.
 * @return The reference state.
 */
static inline CCExpression CCScriptableInterfaceDynamicFieldComponentGetReferenceState(CCComponent Component);

/*!
 * @brief Set the reference state of the dynamic field.
 * @param Component The dynamic field component.
 * @param State The reference state.
 */
static inline void CCScriptableInterfaceDynamicFieldComponentSetReferenceState(CCComponent Component, CCExpression CC_RETAIN(State));

/*!
 * @brief Get the new state of the dynamic field.
 * @note This operation is atomic.
 * @param Component The dynamic field component.
 * @return The new state. This must be destroyed.
 */
static inline CC_NEW CCExpression CCScriptableInterfaceDynamicFieldComponentGetState(CCComponent Component);

/*!
 * @brief Set the new state of the dynamic field.
 * @note This operation is atomic.
 * @param Component The dynamic field component.
 * @param State The new state.
 */
static inline void CCScriptableInterfaceDynamicFieldComponentSetState(CCComponent Component, CCExpression CC_COPY(State));


#pragma mark -

static inline void CCScriptableInterfaceDynamicFieldComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCComponentInitialize(Component, id);
    ((CCScriptableInterfaceDynamicFieldComponentPrivate)Component)->target = NULL;
    ((CCScriptableInterfaceDynamicFieldComponentPrivate)Component)->field = NULL;
    ((CCScriptableInterfaceDynamicFieldComponentPrivate)Component)->refState = NULL;
    ((CCScriptableInterfaceDynamicFieldComponentPrivate)Component)->refRoot = NULL;
    atomic_init(&((CCScriptableInterfaceDynamicFieldComponentPrivate)Component)->state, NULL);
}

static inline void CCScriptableInterfaceDynamicFieldComponentDeallocate(CCComponent Component)
{
    if (((CCScriptableInterfaceDynamicFieldComponentPrivate)Component)->target) CCComponentDestroy(((CCScriptableInterfaceDynamicFieldComponentPrivate)Component)->target);
    if (((CCScriptableInterfaceDynamicFieldComponentPrivate)Component)->field) CCExpressionDestroy(((CCScriptableInterfaceDynamicFieldComponentPrivate)Component)->field);
    if (((CCScriptableInterfaceDynamicFieldComponentPrivate)Component)->refState) CCExpressionDestroy(((CCScriptableInterfaceDynamicFieldComponentPrivate)Component)->refState);
    if (((CCScriptableInterfaceDynamicFieldComponentPrivate)Component)->refRoot) CCExpressionDestroy(((CCScriptableInterfaceDynamicFieldComponentPrivate)Component)->refRoot);
    
    CCExpression State = atomic_load_explicit(&((CCScriptableInterfaceDynamicFieldComponentPrivate)Component)->state, memory_order_acquire);
    if (State) CCExpressionDestroy(State);
    
    CCComponentDeallocate(Component);
}

static inline CCComponent CCScriptableInterfaceDynamicFieldComponentGetTarget(CCComponent Component)
{
    return ((CCScriptableInterfaceDynamicFieldComponentPrivate)Component)->target;
}

static inline void CCScriptableInterfaceDynamicFieldComponentSetTarget(CCComponent Component, CCComponent Target)
{
    CCAssertLog(!((CCScriptableInterfaceDynamicFieldComponentPrivate)Component)->target, "Target should not be changed once set");
    
    ((CCScriptableInterfaceDynamicFieldComponentPrivate)Component)->target = CCRetain(Target);
}

static inline CCExpression CCScriptableInterfaceDynamicFieldComponentGetField(CCComponent Component)
{
    return ((CCScriptableInterfaceDynamicFieldComponentPrivate)Component)->field;
}

static inline void CCScriptableInterfaceDynamicFieldComponentSetField(CCComponent Component, CCExpression Field)
{
    if (((CCScriptableInterfaceDynamicFieldComponentPrivate)Component)->field) CCExpressionDestroy(((CCScriptableInterfaceDynamicFieldComponentPrivate)Component)->field);
    
    ((CCScriptableInterfaceDynamicFieldComponentPrivate)Component)->field = Field;
}

static inline CCExpression CCScriptableInterfaceDynamicFieldComponentGetReferenceRoot(CCComponent Component)
{
    return ((CCScriptableInterfaceDynamicFieldComponentPrivate)Component)->refRoot;
}

static inline CCExpression CCScriptableInterfaceDynamicFieldComponentGetReferenceState(CCComponent Component)
{
    return ((CCScriptableInterfaceDynamicFieldComponentPrivate)Component)->refState;
}

static inline void CCScriptableInterfaceDynamicFieldComponentSetReferenceState(CCComponent Component, CCExpression State)
{
    if (((CCScriptableInterfaceDynamicFieldComponentPrivate)Component)->refState) CCExpressionDestroy(((CCScriptableInterfaceDynamicFieldComponentPrivate)Component)->refState);
    if (((CCScriptableInterfaceDynamicFieldComponentPrivate)Component)->refRoot) CCExpressionDestroy(((CCScriptableInterfaceDynamicFieldComponentPrivate)Component)->refRoot);
    
    ((CCScriptableInterfaceDynamicFieldComponentPrivate)Component)->refRoot = State ? CCExpressionRetain(CCExpressionStateGetSuperRoot(State)) : NULL;
    ((CCScriptableInterfaceDynamicFieldComponentPrivate)Component)->refState = State ? CCExpressionRetain(State) : NULL;
}

static inline CCExpression CCScriptableInterfaceDynamicFieldComponentGetState(CCComponent Component)
{
    return atomic_exchange_explicit(&((CCScriptableInterfaceDynamicFieldComponentPrivate)Component)->state, NULL, memory_order_relaxed);
}

static inline void CCScriptableInterfaceDynamicFieldComponentSetState(CCComponent Component, CCExpression State)
{
    CCExpression PrevState = atomic_exchange_explicit(&((CCScriptableInterfaceDynamicFieldComponentPrivate)Component)->state, (State ? CCExpressionCopy(State) : NULL), memory_order_acq_rel);
    if (PrevState) CCExpressionDestroy(PrevState);
}

#endif
