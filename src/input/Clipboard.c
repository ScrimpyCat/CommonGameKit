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

#include "Clipboard.h"
#include <stdatomic.h>

#ifndef CC_CLIPBOARD_MAX
#define CC_CLIPBOARD_MAX 64
#endif

static inline CCClipboardClipping CCClipboardClippingShare(CCClipboardClipping Clipping)
{
    if (Clipping.type == &CC_REFLECT(CCString))
    {
        if (Clipping.data) Clipping.data = (void*)CCStringCopy((CCString)Clipping.data);
    }
    
    else
    {
        void *Copy;
        CCReflectCopy(Clipping.type, &Copy, &Clipping.data, CCMemoryZoneLocal(), CC_STD_ALLOCATOR, CCReflectAssignmentShare, FALSE);
        
        Clipping.data = Copy;
    }
    
    return Clipping;
}

static inline void CCClipboardClippingDestructor(CCClipboardClipping Clipping)
{
    if (Clipping.type == &CC_REFLECT(CCString)) CCStringDestroy((CCString)Clipping.data);
    else CCFree(Clipping.data);
}

#define Titem CCClipboardClipping
#define Tmax CC_CLIPBOARD_MAX
#define Tshare CCClipboardClippingShare
#define Tdestructor CCClipboardClippingDestructor
#include <CommonC/ConcurrentSharedResource.h>

static CCConcurrentSharedResource(CCClipboardClipping, CC_CLIPBOARD_MAX, CCClipboardClippingShare, CCClipboardClippingDestructor, TRUE) LocalClipboard = CC_CONCURRENT_SHARED_RESOURCE_INIT;

static _Atomic(size_t) ClipboardClippingCount;

void CCClipboardSet(CCClipboardClipping Clipping)
{
    CCConcurrentSharedResourceSet(&LocalClipboard, Clipping);
    
    atomic_signal_fence(memory_order_acquire);
    
    atomic_fetch_add_explicit(&ClipboardClippingCount, 1, memory_order_relaxed);
}

CCClipboardClipping CCClipboardGet(void)
{
    return CCConcurrentSharedResourceGet(&LocalClipboard);
}

size_t CCClipboardCounter(void)
{
    return atomic_load_explicit(&ClipboardClippingCount, memory_order_relaxed);
}
