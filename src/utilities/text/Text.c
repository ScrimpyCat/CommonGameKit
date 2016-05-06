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

#include "Text.h"

typedef struct CCTextInfo {
    CCAllocatorType allocator;
    CCOrderedCollection drawers;
    CCOrderedCollection strings;
    CCTextAlignment alignment;
    CCRect frame;
    struct {
        struct {
            CCTextVisibility options;
            size_t offset;
            size_t length;
        } controls;
        size_t length; //cached visible length
    } visible;
    size_t length; //cached length
} CCTextInfo;

static void CCTextDestructor(CCText Text)
{
    if (Text->drawers) CCCollectionDestroy(Text->drawers);
    if (Text->strings) CCCollectionDestroy(Text->strings);
}

CCText CCTextCreate(CCAllocatorType Allocator)
{
    CCText Text = CCMalloc(Allocator, sizeof(CCTextInfo), NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (Text)
    {
        *Text = (CCTextInfo){
            .allocator = Allocator,
            .drawers = NULL,
            .strings = NULL,
            .alignment = 0,
            .frame = (CCRect){ CCVector2DFill(0.0f), CCVector2DFill(0.0f) },
            .visible = {
                .controls = {
                    .options = CCTextVisibilityMultiLine | CCTextVisibilityWord,
                    .offset = 0,
                    .length = SIZE_MAX
                },
                .length = 0
            },
            .length = 0
        };
        
        CCMemorySetDestructor(Text, (CCMemoryDestructorCallback)CCTextDestructor);
    }
    
    else CC_LOG_ERROR("Failed to create text, due to allocation failure. Allocation size (%zu)", sizeof(CCTextInfo));
    
    return Text;
}

void CCTextDestroy(CCText Text)
{
    CCAssertLog(Text, "Text must not be null");
    CC_SAFE_Free(Text);
}

CCOrderedCollection CCTextGetDrawables(CCText Text)
{
    CCAssertLog(Text, "Text must not be null");
    
    /*
     Return a collection of GFXDraw's, best case it'll just contain one draw
     
     workout what words or characters will be visible
     
     sort those characters into groups of renderables (ones that can be rendered together)
     
     create buffer and draw
     */
    if (Text->strings)
    {
        CCOrderedCollection Selection = CCTextAttributeGetSelection(Text->allocator, Text->strings, Text->visible.controls.offset, Text->visible.controls.length);
        CCOrderedCollection Lines = CCTextAttributeGetLines(Text->allocator, Selection, Text->visible.controls.options, Text->frame.size.x);
    }
    
    return NULL;
}

void CCTextSetString(CCText Text, CCOrderedCollection AttributedStrings)
{
    CCAssertLog(Text, "Text must not be null");
    
    size_t Length = 0;
    CCOrderedCollection Strings = NULL;
    if (AttributedStrings)
    {
        Strings = CCTextAttributeMerge(Text->allocator, AttributedStrings);
        Length = CCTextAttributeGetLength(AttributedStrings);
    }
    
    //TODO: compare difference between new Strings and old Text->strings
    if (Text->strings) CCCollectionDestroy(Text->strings);
    Text->strings = Strings;
    Text->length = Length;
}

void CCTextSetAlignment(CCText Text, CCTextAlignment Alignment)
{
    CCAssertLog(Text, "Text must not be null");
    
    if (Text->alignment != Alignment)
    {
        //TODO: set change in alignment
        Text->alignment = Alignment;
    }
}

void CCTextSetVisibility(CCText Text, CCTextVisibility Visibility)
{
    CCAssertLog(Text, "Text must not be null");
    
    if (Text->visible.controls.options != Visibility)
    {
        //TODO: set change visibility
        Text->visible.controls.options = Visibility;
    }
}

void CCTextSetFrame(CCText Text, CCRect Frame)
{
    CCAssertLog(Text, "Text must not be null");
    
    if ((Text->frame.position.x != Frame.position.x) &&
        (Text->frame.position.y != Frame.position.y) &&
        (Text->frame.size.x != Frame.size.x) &&
        (Text->frame.size.y != Frame.size.y))
    {
        //TODO: set change frame
        Text->frame = Frame;
    }
}

size_t CCTextGetLength(CCText Text)
{
    CCAssertLog(Text, "Text must not be null");
    
    return Text->length;
}

size_t CCTextGetVisibleLength(CCText Text)
{
    CCAssertLog(Text, "Text must not be null");
    //TODO: setup drawables and get visible length
    return Text->visible.length;
}

void CCTextSetVisibleLength(CCText Text, size_t Length)
{
    CCAssertLog(Text, "Text must not be null");
    
    if (Text->visible.controls.length != Length)
    {
        //TODO: set change visible length
        Text->visible.controls.length = Length;
    }
}

size_t CCTextGetOffset(CCText Text)
{
    CCAssertLog(Text, "Text must not be null");
    
    return Text->visible.controls.offset;
}

void CCTextSetOffset(CCText Text, size_t Offset)
{
    CCAssertLog(Text, "Text must not be null");
    
    if (Text->visible.controls.offset != Offset)
    {
        //TODO: set change visible length
        Text->visible.controls.offset = Offset;
    }
}
