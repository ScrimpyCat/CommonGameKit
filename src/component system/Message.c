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
