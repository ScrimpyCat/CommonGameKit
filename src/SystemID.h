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

#ifndef CommonGameKit_SystemID_h
#define CommonGameKit_SystemID_h

#include <CommonGameKit/Component.h>
#include <CommonGameKit/ComponentSystem.h>

enum {
    CCRenderSystemID = 1,
    CCInputSystemID,
    CCAnimationSystemID,
    CCRelationSystemID,
    CCScriptableInterfaceSystemID,
    CCScriptableInterfaceSystemIDMax = CCScriptableInterfaceSystemID + (CCComponentSystemExecutionMax - 1),
    CCSystemIDAvailable
};

#define CC_RENDER_SYSTEM_ID CCRenderSystemID
#define CC_RENDER_COMPONENT_FLAG (CC_RENDER_SYSTEM_ID << CC_COMPONENT_SYSTEM_FLAG_INDEX)

#define CC_INPUT_SYSTEM_ID CCInputSystemID
#define CC_INPUT_COMPONENT_FLAG (CC_INPUT_SYSTEM_ID << CC_COMPONENT_SYSTEM_FLAG_INDEX)

#define CC_ANIMATION_SYSTEM_ID CCAnimationSystemID
#define CC_ANIMATION_COMPONENT_FLAG (CC_ANIMATION_SYSTEM_ID << CC_COMPONENT_SYSTEM_FLAG_INDEX)

#define CC_RELATION_SYSTEM_ID CCRelationSystemID
#define CC_RELATION_COMPONENT_FLAG (CC_RELATION_SYSTEM_ID << CC_COMPONENT_SYSTEM_FLAG_INDEX)

#define CC_SCRIPTABLE_INTERFACE_SYSTEM_ID CCScriptableInterfaceSystemID
#define CC_SCRIPTABLE_INTERFACE_COMPONENT_FLAG (CC_SCRIPTABLE_INTERFACE_SYSTEM_ID << CC_COMPONENT_SYSTEM_FLAG_INDEX)

#endif
