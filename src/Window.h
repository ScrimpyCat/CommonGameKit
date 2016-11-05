/*
 *  Copyright (c) 2016, Stefan Johnson
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

#ifndef CommonGameKit_Window_h
#define CommonGameKit_Window_h

#include <CommonC/Common.h>

/*!
 * @brief Increment the window frame.
 * @description Call this from your game's render loop.
 * @return The previous frame ID. Note: This will rollover.
 */
uint32_t CCWindowFrameStep(void);

/*!
 * @brief Get the current frame ID.
 * @return The current frame ID.
 */
uint32_t CCWindowGetFrameID(void);

/*!
 * @brief Set the window frame size.
 * @description Call this whenever your game's window is resized. Note:
 *              While this function is threadsafe, this may result in
 *              flickering when resizing. If that is the case, some steps
 *              to avoid that can be imposing an explicit lock around this
 *              function call and your render loop, or only calling this
 *              when the window has finished being resized.
 */
void CCWindowSetFrameSize(CCVector2Di Size);

/*!
 * @brief Get the window frame size.
 * @return The current frame size.
 */
CCVector2Di CCWindowGetFrameSize(void);

#endif
