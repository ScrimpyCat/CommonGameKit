/*
 *  Copyright (c) 2019, Stefan Johnson
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

#ifndef CommonGameKit_GFXCommandBuffer_h
#define CommonGameKit_GFXCommandBuffer_h

#include <CommonGameKit/Base.h>

/*!
 * @brief The graphics command buffer.
 * @description Allows @b CCRetain.
 */
typedef struct GFXCommandBuffer *GFXCommandBuffer;


/*!
 * @brief Create a command buffer to group operations submitted to the GPU.
 * @param Allocator The allocator to be used for the allocations.
 * @return The created command buffer.
 */
CC_NEW GFXCommandBuffer GFXCommandBufferCreate(CCAllocatorType Allocator);

/*!
 * @brief Destroy the command buffer.
 * @param CommandBuffer The command buffer to be destroyed.
 */
void GFXCommandBufferDestroy(GFXCommandBuffer CC_DESTROY(CommandBuffer));

/*!
 * @brief Check whether the command buffer has completed.
 * @param CommandBuffer The command buffer to check for completion.
 */
_Bool GFXCommandBufferIsComplete(GFXCommandBuffer CommandBuffer);

/*!
 * @brief Record all operations submitted on this thread to this command buffer.
 * @param CommandBuffer The command buffer to add the operations to. May be NULL.
 */
void GFXCommandBufferRecord(GFXCommandBuffer CC_RETAIN(CommandBuffer));

/*!
 * @brief Commit the command buffer to the GPU.
 * @description Will clear the local recording command buffer if it is the command
 *              buffer being committed. The command buffer will be destroyed after
 *              it has completed.
 *
 * @param CommandBuffer The command buffer to commit.
 * @param Present Whether the command buffer should be presented.
 */
void GFXCommandBufferCommit(GFXCommandBuffer CC_DESTROY(CommandBuffer), _Bool Present);

/*!
 * @brief Get the a command buffer that is currently recording all submitted
 *        operations on the thread.
 *
 * @return The command buffer that is currently recording. Otherwise NULL if there
 *         is none.
 */
GFXCommandBuffer GFXCommandBufferRecording(void);

#endif
