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

#ifndef CommonGameKit_ECSMonitor_h
#define CommonGameKit_ECSMonitor_h

#include <CommonGameKit/ECS.h>

/*!
 * @brief Initialize the internal context for the callbacks.
 * @param SharedContext The internal shared context for the callbacks.
 * @param Context The internal context to be initialized.
 * @param ID The component ID.
 */
typedef void (*ECSMonitorInitializer)(void *SharedContext, void *Context, ECSComponentID ID);

/*!
 * @brief Create a diff.
 * @param SharedContext The internal shared context for the callbacks.
 * @param Context The internal context for this data being monitored.
 * @param Zone The zone that should be used to allocate memory for the diff.
 * @param ID The component ID.
 * @param Data The component data to get the diff for. May be NULL if the component was removed.
 * @return Return a pointer to where the diff data is stored.
 */
typedef void *(*ECSMonitorDiffer)(void *SharedContext, void *Context, CCMemoryZone Zone, ECSComponentID ID, const void *Data);

/*!
 * @brief Transform the @b Data pointer using the provided diff.
 * @param SharedContext The internal shared context for the callbacks.
 * @param Context The internal context for this data being monitored.
 * @param Zone The memory zone used when creating the diff.
 * @param Diff The diff to reverse.
 * @param ID The component ID.
 * @param Data A pointer to the component data that should be transformed.
 * @return Return whether the new component data. This can either be @b Data, or a new pointer, or NULL if the component was removed.
 */
typedef void *(*ECSMonitorTransformer)(void *SharedContext, void *Context, CCMemoryZone Zone, const void *Diff, ECSComponentID ID, void *Data);

/*!
 * @brief Destroy the internal callback state.
 * @param Context The internal shared context for the callbacks to be destroyed.
 */
typedef void (*ECSMonitorSharedContextDestructor)(void *SharedContext);

/*!
 * @brief Destroy the internal callback context.
 * @param Context The internal context for the callbacks to be destroyed.
 */
typedef void (*ECSMonitorContextDestructor)(void *Context);

/*!
 * @brief Destroy the internal diff data.
 * @param Diff The internal diff to be destroyed.
 */
typedef void (*ECSMonitorDiffDestructor)(void *Diff);


typedef struct {
    size_t contextSize;
    ECSMonitorInitializer initialize;
    ECSMonitorDiffer diff;
    ECSMonitorTransformer transform;
    struct {
        ECSMonitorDiffDestructor diffDestructor;
        ECSMonitorContextDestructor contextDestructor;
        ECSMonitorSharedContextDestructor sharedContextDestructor;
    } optional;
} ECSMonitorInterface;

typedef struct {
    const ECSMonitorInterface *interface;
    ECSComponentID id;
    void *sharedContext;
    void *context;
    struct {
        size_t index;
        size_t size;
        size_t count;
        void **diffs;
        CCMemoryZone *zones;
    } page;
} ECSMonitor;

typedef struct {
    CCMemoryZone zone;
    size_t highest;
    size_t prevCount;
} ECSMonitorDuplicateContext;

typedef struct {
    size_t count;
    void *diffs[];
} ECSMonitorDuplicateDiff;

/*!
 * @brief A diff that can be used to indicate component removal.
 */
extern uint8_t ECSMonitorRemovedDiff;

/*!
 * @brief Create a component data monitor.
 * @param Allocator The allocator to be used.
 * @param ID The ID of the component to be monitored.
 * @param PageSize The number of recorded changes that will be kept per page. Must be at least 1.
 * @param PageCount The number of pages of recorded changes. As the pages fill up, the oldest page is purged. Must be at least 2.
 * @param ZoneSize The size of the page memory zones to be used by the diffing function.
 * @param Interface The monitor implementation to be used.
 * @param SharedContext The shared context data.
 * @return The component data monitor. This must be destroyed.
 */
static inline CC_NEW ECSMonitor ECSMonitorCreate(CCAllocatorType Allocator, ECSComponentID ID, size_t PageSize, size_t PageCount, size_t ZoneSize, const ECSMonitorInterface *Interface, void *SharedContext);

/*!
 * @brief Record the component data.
 * @note The monitor implementation determines when a record will be kept.
 * @param Monitor The monitor that will record the component data.
 * @param Data The component data. This may be NULL to indicate the component was removed.
 */
void ECSMonitorRecord(ECSMonitor *Monitor, const void *Data);

/*!
 * @brief Revert the component data to a previous recorded revision.
 * @param Monitor The monitor that that contains the earlier records for the component data.
 * @param Data The component data to be transformed to an earlier revision.
 * @param Revisions The number of changes to rollback.
 * @return Whether the component exists (TRUE), or was removed (FALSE).
 */
_Bool ECSMonitorTransform(ECSMonitor *Monitor, void *Data, size_t Revisions);

void ECSMonitorDestroy(ECSMonitor *CC_DESTROY(Monitor));

#pragma mark -

static inline ECSMonitor ECSMonitorCreate(CCAllocatorType Allocator, ECSComponentID ID, size_t PageSize, size_t PageCount, size_t ZoneSize, const ECSMonitorInterface *Interface, void *SharedContext)
{
    CCAssertLog(PageSize, "PageSize must not be 0");
    CCAssertLog(PageCount > 1, "PageCount must be bigger than 1");
    CCAssertLog(Interface, "Interface must not be null");
    
    void *Context;
    if (ID & ECSComponentStorageModifierDuplicate)
    {
        ECSMonitorDuplicateContext *DuplicateContext = CCMalloc(Allocator, sizeof(ECSMonitorDuplicateContext), NULL, CC_DEFAULT_ERROR_CALLBACK);
        
        size_t ContextSize = Interface->contextSize * 16;
        
        if (ContextSize < Interface->contextSize) ContextSize = Interface->contextSize;
        
        DuplicateContext->zone = CCMemoryZoneCreate(Allocator, ContextSize);
        DuplicateContext->highest = 0;
        
        Context = DuplicateContext;
        
        ZoneSize += sizeof(ECSMonitorDuplicateDiff) + (sizeof(void*) * 16);
    }
    
    else
    {
        Context = CCMalloc(Allocator, Interface->contextSize, NULL, CC_DEFAULT_ERROR_CALLBACK);
        
        Interface->initialize(SharedContext, Context, ID);
    }
    
    CCMemoryZone *Zones = CCMalloc(Allocator, sizeof(CCMemoryZone) * PageCount, NULL, CC_DEFAULT_ERROR_CALLBACK);
    for (size_t Loop = 0; Loop < PageCount; Loop++)
    {
        Zones[Loop] = CCMemoryZoneCreate(Allocator, ZoneSize);
    }
    
    void *Diffs = CCMalloc(Allocator, sizeof(void*) * PageSize * PageCount, NULL, CC_DEFAULT_ERROR_CALLBACK);
    memset(Diffs, 0, sizeof(void*) * PageSize * PageCount);
    
    return (ECSMonitor){
        .interface = Interface,
        .id = ID,
        .sharedContext = SharedContext,
        .context = Context,
        .page = {
            .index = 0,
            .size = PageSize,
            .count = PageCount,
            .diffs = Diffs,
            .zones = Zones
        }
    };
}

#endif
