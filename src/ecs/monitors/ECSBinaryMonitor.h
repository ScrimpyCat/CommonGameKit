/*
 *  Copyright (c) 2023, Stefan Johnson
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

#ifndef CommonGameKit_ECSBinaryMonitor_h
#define CommonGameKit_ECSBinaryMonitor_h

#include <CommonGameKit/ECSMonitor.h>

typedef struct {
    CCAllocatorType allocator;
    struct {
        size_t size;
    } copy;
    struct {
        size_t offset : 3;
        size_t size : (sizeof(size_t) * 8) - 3;
    } diff;
} ECSBinaryMonitorSharedContext;

typedef struct {
    struct {
        void *data;
        _Bool created;
    } copy;
} ECSBinaryMonitorContext;

extern const ECSMonitorInterface * const ECSBinaryMonitor;

/*!
 * @brief Create a binary diffing component data monitor.
 * @param Allocator The allocator to be used.
 * @param ID The ID of the component to be monitored.
 * @param PageSize The number of recorded changes that will be kept per page. Must be at least 1.
 * @param PageCount The number of pages of recorded changes. As the pages fill up, the oldest page is purged. Must be at least 2.
 * @param SequenceSize The size byte sequences to diff.
 * @return The component data monitor. This must be destroyed.
 */
static inline CC_NEW ECSMonitor ECSBinaryMonitorCreate(CCAllocatorType Allocator, ECSComponentID ID, size_t PageSize, size_t PageCount, size_t SequenceSize);

#pragma mark -

static inline ECSMonitor ECSBinaryMonitorCreate(CCAllocatorType Allocator, ECSComponentID ID, size_t PageSize, size_t PageCount, size_t SequenceSize)
{
    CCAssertLog(SequenceSize > 0, "SequenceSize must not be zero");
    
    const size_t Size = ECSComponentSize(ID);
    
    if (SequenceSize > Size) SequenceSize = Size;
    
    const size_t Indexes = Size ? (Size / SequenceSize) : 0;
    const size_t OffsetSize = CCBitCountSet(CCBitMaskForValue(Indexes + 0xfe)) / 8;
    
    ECSBinaryMonitorSharedContext *SharedContext = CCMalloc(Allocator, sizeof(ECSBinaryMonitorSharedContext), NULL, CC_DEFAULT_ERROR_CALLBACK);
    
    SharedContext->allocator = Allocator;
    SharedContext->copy.size = Size;
    SharedContext->diff.offset = OffsetSize - 1;
    SharedContext->diff.size = SequenceSize;
    
    size_t ZoneSize = OffsetSize + SequenceSize;
    
    if (ZoneSize < (ZoneSize * PageSize)) ZoneSize *= PageSize;
    
    return ECSMonitorCreate(Allocator, ID, PageSize, PageCount, ZoneSize, ECSBinaryMonitor, SharedContext);
}

#endif
