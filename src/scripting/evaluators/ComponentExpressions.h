/*
 *  Copyright (c) 2017, Stefan Johnson
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

#ifndef CommonGameKit_ComponentExpressions_h
#define CommonGameKit_ComponentExpressions_h

#include <CommonGameKit/ExpressionEvaluator.h>
#include <CommonGameKit/Component.h>


typedef enum {
    CCComponentExpressionValueTypeComponent = 'cmpt'
} CCComponentExpressionValueType;

/*!
 * @brief Initialize a component.
 * @param Component The component to be initialized.
 */
typedef void (*CCComponentExpressionInitializer)(CCComponent Component);

/*!
 * @brief Evaluate a component argument.
 * @description The deserializer is responsible for evaluating any arguments of the parent
 *              component as well.
 *
 * @param Component The component whose state is to be set.
 * @param Arg The expression of the argument, which takes the form of an option.
 * @param Deferred Whether resolving value can be deferred or not.
 */
typedef void (*CCComponentExpressionDeserializer)(CCComponent Component, CCExpression Arg, _Bool Deferred);

/*!
 * @brief Convert a component into an expression.
 * @param Allocator The allocator to be used.
 * @param Component The component to be converted into an expression.
 * @return The expression.
 */
typedef CC_NEW CCExpression (*CCComponentExpressionSerializer)(CCAllocatorType Allocator, CCComponent Component);

typedef struct {
    CCComponentID id;
    CCComponentExpressionInitializer initialize;
    CCComponentExpressionDeserializer deserialize;
    CCComponentExpressionSerializer serialize;
} CCComponentExpressionDescriptor;

typedef enum {
    CCComponentExpressionArgumentTypeMask = 0xff,
    CCComponentExpressionArgumentTypeBool = 1,
    CCComponentExpressionArgumentTypeUInt8,
    CCComponentExpressionArgumentTypeUInt16,
    CCComponentExpressionArgumentTypeUInt32,
    CCComponentExpressionArgumentTypeUInt64,
    CCComponentExpressionArgumentTypeInt8,
    CCComponentExpressionArgumentTypeInt16,
    CCComponentExpressionArgumentTypeInt32,
    CCComponentExpressionArgumentTypeInt64,
    CCComponentExpressionArgumentTypeFloat32,
    CCComponentExpressionArgumentTypeFloat64,
    CCComponentExpressionArgumentTypeVector2,
    CCComponentExpressionArgumentTypeVector3,
    CCComponentExpressionArgumentTypeVector4,
    CCComponentExpressionArgumentTypeVector2i,
    CCComponentExpressionArgumentTypeVector3i,
    CCComponentExpressionArgumentTypeVector4i,
    CCComponentExpressionArgumentTypeColour,
    CCComponentExpressionArgumentTypeString,
    CCComponentExpressionArgumentTypeData,
    CCComponentExpressionArgumentTypeExpression,
    CCComponentExpressionArgumentTypeTextAttribute,
    CCComponentExpressionArgumentTypeMax,
    
#if CC_HARDWARE_PTR_64
    CCComponentExpressionArgumentTypeSize = CCComponentExpressionArgumentTypeUInt64,
#elif CC_HARDWARE_PTR_32
    CCComponentExpressionArgumentTypeSize = CCComponentExpressionArgumentTypeUInt32,
#endif
    
    CCComponentExpressionArgumentTypeContainerMask = (3 << 29),
    CCComponentExpressionArgumentTypeContainerComponent = 0,
    CCComponentExpressionArgumentTypeContainerArray = (1 << 29),
    CCComponentExpressionArgumentTypeContainerList = (2 << 29),
    CCComponentExpressionArgumentTypeContainerOrderedCollection = (3 << 29),
    
    CCComponentExpressionArgumentTypeOwnershipMask = (1 << 31),
    CCComponentExpressionArgumentTypeOwnershipTransfer = (0 << 31),
    CCComponentExpressionArgumentTypeOwnershipRetain = (1 << 31)
} CCComponentExpressionArgumentSetterType;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wstrict-prototypes"
/*!
 * @brief The component setter function.
 * @description This should take the form of the @b CCComponentExpressionArgumentSetterType provided.
 */
typedef void (*CCComponentExpressionSetter)();
#pragma clang diagnostic pop

typedef struct {
    /// The name of the argument to be matched.
    CCString name;
    /// Ignored if @b CCExpressionValueTypeUnspecified otherwise is checked and used to inform the conversion
    CCExpressionValueType serializedType;
    /// The type of setter being used.
    CCComponentExpressionArgumentSetterType setterType;
    /// The component setter to be used for the given deserializer.
    CCComponentExpressionSetter setter;
} CCComponentExpressionArgumentDeserializer;

/*!
 * @brief Register component evaluators.
 * @param Name The name of the component expression.
 * @param Descriptor The descriptor to workout how to create the component from the expression.
 * @param Wrapper Whether or not a wrapper function should be created.
 */
void CCComponentExpressionRegister(CCString CC_COPY(Name), const CCComponentExpressionDescriptor *Descriptor, _Bool Wrapper);

/*!
 * @brief Deserialize and set a component argument.
 * @param Component The component to be set.
 * @param Arg The expression argument to be deserialized.
 * @param Deserializer The argument deserializers.
 * @param Count The number of argument deserializers.
 * @param Deferred Whether resolving value can be deferred or not.
 * @return TRUE if the argument was successfully deserialized, otherwise FALSE if it was not.
 */
_Bool CCComponentExpressionDeserializeArgument(CCComponent Component, CCExpression Arg, const CCComponentExpressionArgumentDeserializer *Deserializer, size_t Count, _Bool Deferred);

/*!
 * @brief Deserialize a deferred component argument.
 * @param Component The component to be set.
 * @param Arg The expression argument to be deferred deserialization.
 * @param Deferred Whether resolving value can be deferred or not.
 * @return TRUE if the argument was successfully deferred deserialization, otherwise FALSE if it was not.
 */
_Bool CCComponentExpressionDeserializeDeferredArgument(CCComponent Component, CCExpression Arg, _Bool Deferred);

/*!
 * @brief Get the component expression descriptor for the given name it was registered under.
 * @param Name The name of the component expression.
 * @return The component expression descriptor for the name, or NULL if one was not found.
 */
const CCComponentExpressionDescriptor *CCComponentExpressionDescriptorForName(CCString Name);

/*!
 * @brief Get the component expression descriptor for the given component ID.
 * @param id The ID of the component.
 * @return The component expression descriptor for the name, or NULL if one was not found.
 */
const CCComponentExpressionDescriptor *CCComponentExpressionDescriptorForID(CCComponentID id);

/*!
 * @brief Create a component expression for a given component.
 * @param Allocator The allocator to be used.
 * @param Component The component to be converted into an expression.
 * @return The expression that will evaluate into an equivalent component.
 */
CCExpression CCComponentExpressionCreate(CCAllocatorType Allocator, CCComponent Component);

CC_EXPRESSION_EVALUATOR(component) CCExpression CCComponentExpressionComponent(CCExpression Expression);

#endif
