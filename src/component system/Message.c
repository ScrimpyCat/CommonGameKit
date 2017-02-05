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
    CCMessage *Message = CCMalloc(Allocator, sizeof(CCMessage) + Size, NULL, CC_DEFAULT_ERROR_CALLBACK);
    
    if (Message)
    {
        Message->id = id;
        Message->router = Router;
        if (Data) memcpy(((CCMessageData*)Message)->data, Data, Size);
    }
    
    //Post to mailboxes for routes
    
//    CCConcurrentQueueNode *Node = CCConcurrentQueueCreateNode(Allocator, sizeof(CCMessage*), &Message);
//    CCMemorySetDestructor(Node, CCFree);
}

#pragma mark - Component Belonging To Entity

typedef struct {
    CCEntity entity;
    CCComponentID componentID;
} CCMessageRouteComponentEntity;

static void CCMessageRouteComponentEntityPoster(CCMessageRouter *Router, CCMessage *Message)
{
    CCComponentSystemID SystemID = CCComponentSystemHandlesComponentID(((CCMessageRouteComponentEntity*)CCMessageRouterGetData(Router))->componentID);
    CCConcurrentQueue Mailbox = CCComponentSystemGetMailbox(SystemID);
    
    CCConcurrentQueueNode *Node = CCConcurrentQueueCreateNode(CC_STD_ALLOCATOR, sizeof(CCMessage*), &Message);
    CCMemorySetDestructor(Node, CCFree);
    
    CCConcurrentQueuePush(Mailbox, Node);
}

static CCComparisonResult CCMessageRouteComponentEntityFindComponent(const CCComponent *Component, const CCMessageRouteComponentEntity *Data)
{
    if ((CCComponentGetID(*Component) == Data->componentID) && (CCComponentGetEntity(*Component) == Data->entity)) return CCComparisonResultEqual;
    
    return CCComparisonResultInvalid;
}

static void CCMessageRouteComponentEntityDeliverer(CCMessage *Message, CCComponentSystemID SystemID)
{
    CCCollection Components = CCComponentSystemGetComponentsForSystem(SystemID); //Or can we safely CCEntityGetComponents?
    CCComponent *Component = CCCollectionGetElement(Components, CCCollectionFindElement(Components, CCMessageRouterGetData(Message->router), (CCComparator)CCMessageRouteComponentEntityFindComponent));
    
    if (Component)
    {
//        CCComponentHandleMessage(Component, Message);
    }
}

CCMessageRouter *CCMessageDeliverToComponentOfTypeBelongingToEntity(CCComponentID ComponentID, CCEntity Entity)
{
    return CCMessageRouterCreate(CC_STD_ALLOCATOR, CCMessageRouteComponentEntityPoster, CCMessageRouteComponentEntityDeliverer, sizeof(CCMessageRouteComponentEntity), &(CCMessageRouteComponentEntity){
        .componentID = ComponentID,
        .entity = Entity
    });
}
