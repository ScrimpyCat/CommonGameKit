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

#include "Window.h"
#include <stdatomic.h>

static _Atomic(uint32_t) CCWindowFrameID = ATOMIC_VAR_INIT(0);

uint32_t CCWindowFrameStep(void)
{
    return atomic_fetch_add_explicit(&CCWindowFrameID, 1, memory_order_relaxed);
}

uint32_t CCWindowGetFrameID(void)
{
    return atomic_load_explicit(&CCWindowFrameID, memory_order_relaxed);
}

static _Atomic(CCVector2Di) CCWindowFrameSize = ATOMIC_VAR_INIT(((CCVector2Di){ 0, 0 }));

void CCWindowSetFrameSize(CCVector2Di Size)
{
    atomic_store_explicit(&CCWindowFrameSize, Size, memory_order_relaxed);
}

CCVector2Di CCWindowGetFrameSize(void)
{
    return atomic_load_explicit(&CCWindowFrameSize, memory_order_relaxed);
}

