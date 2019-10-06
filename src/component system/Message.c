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

#define CC_QUICK_COMPILE
#include "Message.h"
#include "ComponentBase.h"


CCMessageRouter *CCMessageRouterCreate(CCAllocatorType Allocator, CCMessageRouterPost Post, CCMessageRouterDeliver Deliver, size_t Size, const void *Data)
{
    CCMessageRouter *Router = CCMalloc(Allocator, sizeof(CCMessageRouter) + Size, NULL, CC_DEFAULT_ERROR_CALLBACK);
    
    if (Router)
    {
        Router->post = Post;
        Router->deliver = Deliver;
        if (Data) memcpy(((CCMessageRouterData*)Router)->data, Data, Size);
    }
    
    return Router;
}

void CCMessageRouterDestroy(CCMessageRouter *Router)
{
    CCAssertLog(Router, "Router must not be null");
    
    CCFree(Router);
}

void CCMessageDestroy(CCMessage *Message)
{
    CCAssertLog(Message, "Message must not be null");
    
    CCFree(Message);
}

void CCMessageDestructor(CCMessage *Message)
{
    CCFree(Message->router);
}

void CCMessagePost(CCAllocatorType Allocator, CCMessageID id, CCMessageRouter *Router, size_t Size, const void *Data)
{
    CCAssertLog(Router, "Router must not be null");
    
    CCMessage *Message = CCMalloc(Allocator, sizeof(CCMessage) + Size, NULL, CC_DEFAULT_ERROR_CALLBACK);
    
    if (Message)
    {
        if (Data) memcpy(((CCMessageData*)Message)->data, Data, Size);
        
        Message->id = id;
        Message->router = Router;
        
        Router->post(Router, Message);
        CCMessageDestroy(Message);
    }
}

static void CCMessageNodeDataDestructor(CCConcurrentQueueNode *Node)
{
    CCMessageDestroy(*(CCMessage**)CCConcurrentQueueGetNodeData(Node));
}

#pragma mark - Component

typedef struct {
    CCComponentID componentID;
} CCMessageRouteComponent;

static void CCMessageRouteComponentPoster(CCMessageRouter *Router, CCMessage *Message)
{
    CCComponentSystemHandle *System = CCComponentSystemHandlesComponentID(((CCMessageRouteComponent*)CCMessageRouterGetData(Router))->componentID);
    if (System)
    {
#define CC_CONTAINER_TYPE_DISABLE
        CCConcurrentQueue(CCMessage *) Mailbox = CCComponentSystemGetMailbox(System->id); //TODO: update handle to include mailbox
#undef CC_CONTAINER_TYPE_DISABLE
        
        CCConcurrentQueueNode *Node = CCConcurrentQueueCreateNode(CC_STD_ALLOCATOR, sizeof(CCMessage*), &(CCMessage*){ CCRetain(Message) });
        CCMemorySetDestructor(Node, (CCMemoryDestructorCallback)CCMessageNodeDataDestructor);
        
        CCConcurrentQueuePush(Mailbox, Node);
    }
}

static void CCMessageRouteComponentDeliverer(CCMessage *Message, CCComponentSystemID SystemID)
{
    CCCollection(CCComponent) Components = CCComponentSystemGetComponentsForSystem(SystemID);
    CC_COLLECTION_FOREACH(CCComponent, Component, Components)
    {
        if (CCComponentGetID(Component) == ((CCMessageRouteComponent*)CCMessageRouterGetData(Message->router))->componentID)
        {
            CCComponentHandleMessage(Component, Message);
        }
    }
}

CCMessageRouter *CCMessageDeliverToComponent(CCComponentID ComponentID)
{
    return CCMessageRouterCreate(CC_STD_ALLOCATOR, CCMessageRouteComponentPoster, CCMessageRouteComponentDeliverer, sizeof(CCMessageRouteComponent), &(CCMessageRouteComponent){
        .componentID = ComponentID
    });
}

#pragma mark - Component Belonging To Entity

typedef struct {
    CCEntity entity;
    CCComponentID componentID;
} CCMessageRouteComponentEntity;

static void CCMessageRouteComponentEntityPoster(CCMessageRouter *Router, CCMessage *Message)
{
    CCComponentSystemHandle *System = CCComponentSystemHandlesComponentID(((CCMessageRouteComponentEntity*)CCMessageRouterGetData(Router))->componentID);
    if (System)
    {
#define CC_CONTAINER_TYPE_DISABLE
        CCConcurrentQueue(CCMessage *) Mailbox = CCComponentSystemGetMailbox(System->id); //TODO: update handle to include mailbox
#undef CC_CONTAINER_TYPE_DISABLE
        
        CCConcurrentQueueNode *Node = CCConcurrentQueueCreateNode(CC_STD_ALLOCATOR, sizeof(CCMessage*), &(CCMessage*){ CCRetain(Message) });
        CCMemorySetDestructor(Node, (CCMemoryDestructorCallback)CCMessageNodeDataDestructor);
        
        CCConcurrentQueuePush(Mailbox, Node);
    }
}

static CCComparisonResult CCMessageRouteComponentEntityFindComponent(const CCComponent *Component, const CCMessageRouteComponentEntity *Data)
{
    if ((CCComponentGetID(*Component) == Data->componentID) && (CCComponentGetEntity(*Component) == Data->entity)) return CCComparisonResultEqual;
    
    return CCComparisonResultInvalid;
}

static void CCMessageRouteComponentEntityDeliverer(CCMessage *Message, CCComponentSystemID SystemID)
{
    CCCollection(CCComponent) Components = CCComponentSystemGetComponentsForSystem(SystemID); //Or can we safely CCEntityGetComponents?
    CCComponent *Component = CCCollectionGetElement(Components, CCCollectionFindElement(Components, CCMessageRouterGetData(Message->router), (CCComparator)CCMessageRouteComponentEntityFindComponent));
    
    if (Component)
    {
        CCComponentHandleMessage(*Component, Message);
    }
}

CCMessageRouter *CCMessageDeliverToComponentBelongingToEntity(CCComponentID ComponentID, CCEntity Entity)
{
    return CCMessageRouterCreate(CC_STD_ALLOCATOR, CCMessageRouteComponentEntityPoster, CCMessageRouteComponentEntityDeliverer, sizeof(CCMessageRouteComponentEntity), &(CCMessageRouteComponentEntity){
        .componentID = ComponentID,
        .entity = Entity
    });
}
