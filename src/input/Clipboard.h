/*
 *  Copyright (c) 2024, Stefan Johnson
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

#ifndef CommonGameKit_Clipboard_h
#define CommonGameKit_Clipboard_h

#include <CommonGameKit/Base.h>

typedef struct {
    CCReflectType type;
    void *data;
} CCClipboardClipping;


/*!
 * @brief Set the local clipboard.
 * @param Clipping The clipping to attach to the local clipboard. Ownership of the clipping data is passed to the clipboard.
 */
void CCClipboardSet(CCClipboardClipping CC_OWN(Clipping));

/*!
 * @brief Get the current clipping from the local clipboard.
 * @return The current clipping. The caller is responsible for destroying its reference to the clipping data.
 */
CC_NEW CCClipboardClipping CCClipboardGet(void);

/*!
 * @brief Get the local clipboard counter.
 * @description This can be used to identify if the clipboard has changed.
 * @return The current clipboard counter.
 */
size_t CCClipboardCounter(void)



#pragma mark - Template Types

#define CC_TYPE_CCClipboardClipping(...) CCClipboardClipping
#define CC_TYPE_0_CCClipboardClipping CC_TYPE_CCClipboardClipping,
#define CC_TYPE_1_CCClipboardClipping CC_TYPE_CCClipboardClipping,
#define CC_TYPE_2_CCClipboardClipping CC_TYPE_CCClipboardClipping,
#define CC_TYPE_3_CCClipboardClipping CC_TYPE_CCClipboardClipping,
#define CC_TYPE_4_CCClipboardClipping CC_TYPE_CCClipboardClipping,
#define CC_TYPE_5_CCClipboardClipping CC_TYPE_CCClipboardClipping,
#define CC_TYPE_6_CCClipboardClipping CC_TYPE_CCClipboardClipping,
#define CC_TYPE_7_CCClipboardClipping CC_TYPE_CCClipboardClipping,
#define CC_TYPE_8_CCClipboardClipping CC_TYPE_CCClipboardClipping,
#define CC_TYPE_9_CCClipboardClipping CC_TYPE_CCClipboardClipping,

#define CC_PRESERVE_CC_TYPE_CCClipboardClipping CC_TYPE_CCClipboardClipping

#define CC_TYPE_DECL_CCClipboardClipping(...) CCClipboardClipping, __VA_ARGS__
#define CC_TYPE_DECL_0_CCClipboardClipping CC_TYPE_DECL_CCClipboardClipping,
#define CC_TYPE_DECL_1_CCClipboardClipping CC_TYPE_DECL_CCClipboardClipping,
#define CC_TYPE_DECL_2_CCClipboardClipping CC_TYPE_DECL_CCClipboardClipping,
#define CC_TYPE_DECL_3_CCClipboardClipping CC_TYPE_DECL_CCClipboardClipping,
#define CC_TYPE_DECL_4_CCClipboardClipping CC_TYPE_DECL_CCClipboardClipping,
#define CC_TYPE_DECL_5_CCClipboardClipping CC_TYPE_DECL_CCClipboardClipping,
#define CC_TYPE_DECL_6_CCClipboardClipping CC_TYPE_DECL_CCClipboardClipping,
#define CC_TYPE_DECL_7_CCClipboardClipping CC_TYPE_DECL_CCClipboardClipping,
#define CC_TYPE_DECL_8_CCClipboardClipping CC_TYPE_DECL_CCClipboardClipping,
#define CC_TYPE_DECL_9_CCClipboardClipping CC_TYPE_DECL_CCClipboardClipping,

#define CC_MANGLE_TYPE_0_CCClipboardClipping CCClipboardClipping
#define CC_MANGLE_TYPE_1_CCClipboardClipping CCClipboardClipping
#define CC_MANGLE_TYPE_2_CCClipboardClipping CCClipboardClipping
#define CC_MANGLE_TYPE_3_CCClipboardClipping CCClipboardClipping
#define CC_MANGLE_TYPE_4_CCClipboardClipping CCClipboardClipping
#define CC_MANGLE_TYPE_5_CCClipboardClipping CCClipboardClipping
#define CC_MANGLE_TYPE_6_CCClipboardClipping CCClipboardClipping
#define CC_MANGLE_TYPE_7_CCClipboardClipping CCClipboardClipping
#define CC_MANGLE_TYPE_8_CCClipboardClipping CCClipboardClipping
#define CC_MANGLE_TYPE_9_CCClipboardClipping CCClipboardClipping

#endif
