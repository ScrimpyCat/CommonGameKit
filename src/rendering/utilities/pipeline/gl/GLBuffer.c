/*
 *  Copyright (c) 2015, Stefan Johnson
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

#include "GLBuffer.h"

static GLBuffer GLBufferConstructor(CCAllocatorType Allocator, GFXBufferHint Hint, size_t Size, const void *Data);
static void GLBufferDestructor(GLBuffer Buffer);
static GFXBufferHint GLBufferGetHint(GLBuffer Buffer);
static size_t GLBufferGetSize(GLBuffer Buffer);
static _Bool GLBufferResize(GLBuffer Buffer, size_t Size);
static size_t GLBufferReadBuffer(GLBuffer Buffer, ptrdiff_t Offset, size_t Size, void *Data);
static size_t GLBufferWriteBuffer(GLBuffer Buffer, ptrdiff_t Offset, size_t Size, const void *Data);
static void GLBufferInvalidate(GLBuffer Buffer);


const GFXBufferInterface GLBufferInterface = {
    .create = (GFXBufferConstructorCallback)GLBufferConstructor,
    .destroy = (GFXBufferDestructorCallback)GLBufferDestructor,
    .hints = (GFXBufferGetHintCallback)GLBufferGetHint,
    .size = (GFXBufferGetSizeCallback)GLBufferGetSize,
    .resize = (GFXBufferResizeCallback)GLBufferResize,
    .read = (GFXBufferReadBufferCallback)GLBufferReadBuffer,
    .write = (GFXBufferWriteBufferCallback)GLBufferWriteBuffer,
    .optional = {
        .invalidate = (GFXBufferInvalidateCallback)GLBufferInvalidate,
        //TODO: Add copy .copy = (GFXBufferCopyBufferCallback)GLBufferCopyBuffer,
        //TODO: Add fill .fill = (GFXBufferFillBufferCallback)GLBufferFillBuffer
    }
};


static inline CC_CONSTANT_FUNCTION GLenum GLBufferTarget(GFXBufferHint Hint)
{
    return (GLenum[4]){
        0,
        GL_ARRAY_BUFFER,
        GL_ELEMENT_ARRAY_BUFFER,
        GL_UNIFORM_BUFFER
    }[(Hint & GFXBufferHintDataMask)];
}

static inline CC_CONSTANT_FUNCTION GLenum GLBufferUsage(GFXBufferHint Hint)
{
    /*
     Nature:
     DRAW : CPUWrite : GPURead
     COPY : GPUWrite : GPURead
     READ : GPUWrite : CPURead
     
     GPUWrite = 2
     CPUWrite = 1
     GPURead = 1
     CPURead = 0
     
     Write - Read
     
     GPUWrite - GPURead = 2 - 1 = 1 : COPY
     GPUWrite - CPURead = 2 - 0 = 2 : READ
     CPUWrite - GPURead = 1 - 1 = 0 : DRAW
     CPUWrite - CPURead = 1 - 0 = 1 : COPY //incorrect though theoretically true
     
     (_Bool)CPUWrite + GPUWrite //0, 1, 2, 3
     GPURead - (_Bool)CPURead  //-1, 0, 1, 2
     
     
     Frequency:
     STREAM : WriteNone, WriteOnce : ReadNone, ReadOnce
     STATIC : WriteNone, WriteOnce : ReadMany
     DYNAMIC : WriteMany : ReadMany (leftovers ReadNone, ReadOnce)
     
     STREAM : 0, 1 : 0, 1
     STATIC : 0, 1 : 2
     DYNAMIC : 2 : 2
     
     STREAM : 0 : 0
     STATIC : 0 : 1
     DYNAMIC : 1 : 1
     DYNAMIC : 1 : 0
     
     STREAM : 0 : 0 = 0
     STATIC : 0 : 1 = 1
     DYNAMIC : 2 : 1 = 3
     DYNAMIC : 2 : 0 = 2
     */
    static const GLenum Usage[3][3] = {
        { GL_STREAM_DRAW, GL_STREAM_COPY, GL_STREAM_READ },
        { GL_STATIC_DRAW, GL_STATIC_COPY, GL_STATIC_READ },
        { GL_DYNAMIC_DRAW, GL_DYNAMIC_COPY, GL_DYNAMIC_READ }
    };
    
    const int CPUWrite = (Hint >> GFXBufferHintCPUWrite) & GFXBufferHintAccessMask;
    const int GPUWrite = (Hint >> GFXBufferHintGPUWrite) & GFXBufferHintAccessMask;
    const int CPURead = (Hint >> GFXBufferHintCPURead) & GFXBufferHintAccessMask;
    const int GPURead = (Hint >> GFXBufferHintGPURead) & GFXBufferHintAccessMask;
    
    const int Frequency = (((CPUWrite == GFXBufferHintAccessMany) | (GPUWrite == GFXBufferHintAccessMany)) << 1) + ((CPURead == GFXBufferHintAccessMany) | (GPURead == GFXBufferHintAccessMany));
    const int Nature = CCClampi(((_Bool)CPUWrite + ((_Bool)GPUWrite * 2)) - (_Bool)GPURead, 0, 2);
    
    return Usage[Frequency > 2 ? 2 : Frequency][Nature];
}

static GLBuffer GLBufferConstructor(CCAllocatorType Allocator, GFXBufferHint Hint, size_t Size, const void *Data)
{
    GLBuffer Buffer = CCMalloc(Allocator, sizeof(GLBufferInfo), NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (Buffer)
    {
        Buffer->hint = Hint;
        switch (Hint & GFXBufferHintDataMask)
        {
            case GFXBufferHintData: //cpu store
                Buffer->data = CCDataBufferCreate(CC_STD_ALLOCATOR, CCDataBufferHintCopy | CCDataHintReadWrite, Size, Data, NULL, NULL);
                break;
                
            case GFXBufferHintDataVertex:
            case GFXBufferHintDataIndex:
            case GFXBufferHintDataUniform:
            {
                const GLenum Target = GLBufferTarget(Hint);
                
                glGenBuffers(1, &Buffer->gl.buffer); CC_GL_CHECK();
                CC_GL_BIND_BUFFER_TARGET(Target, Buffer->gl.buffer);
                glBufferData(Target, (Buffer->gl.size = Size), Data, GLBufferUsage(Hint)); CC_GL_CHECK();
                break;
            }
        }
    }
    
    return Buffer;
}

static void GLBufferDestructor(GLBuffer Buffer)
{
    CCAssertLog(Buffer, "Buffer must not be null");
    
    switch (Buffer->hint & GFXBufferHintDataMask)
    {
        case GFXBufferHintData: //cpu store
            CCDataDestroy(Buffer->data);
            break;
            
        case GFXBufferHintDataVertex:
        case GFXBufferHintDataIndex:
        case GFXBufferHintDataUniform:
            glDeleteBuffers(1, &Buffer->gl.buffer); CC_GL_CHECK();
            break;
    }
    
    CC_SAFE_Free(Buffer);
}

static GFXBufferHint GLBufferGetHint(GLBuffer Buffer)
{
    return Buffer->hint;
}

static size_t GLBufferGetSize(GLBuffer Buffer)
{
    switch (Buffer->hint & GFXBufferHintDataMask)
    {
        case GFXBufferHintData: //cpu store
            return CCDataGetSize(Buffer->data);
            
        case GFXBufferHintDataVertex:
        case GFXBufferHintDataIndex:
        case GFXBufferHintDataUniform:
            return Buffer->gl.size;
    }
    
    return 0;
}

static _Bool GLBufferResize(GLBuffer Buffer, size_t Size)
{
    switch (Buffer->hint & GFXBufferHintDataMask)
    {
        case GFXBufferHintData: //cpu store
            return CCDataSetSize(Buffer->data, Size);
            
        case GFXBufferHintDataVertex:
        case GFXBufferHintDataIndex:
        case GFXBufferHintDataUniform:
        {
            const GLenum Target = GLBufferTarget(Buffer->hint);
            
            CC_GL_BIND_BUFFER_TARGET(Target, Buffer->gl.buffer);
            glBufferData(Target, (Buffer->gl.size = Size), NULL, GLBufferUsage(Buffer->hint));
            
            return CC_GL_CHECK() == GL_NO_ERROR;
        }
    }
    
    return FALSE;
}

static size_t GLBufferReadBuffer(GLBuffer Buffer, ptrdiff_t Offset, size_t Size, void *Data)
{
    switch (Buffer->hint & GFXBufferHintDataMask)
    {
        case GFXBufferHintData: //cpu store
            return CCDataReadBuffer(Buffer->data, Offset, Size, Data);
            
        case GFXBufferHintDataVertex:
        case GFXBufferHintDataIndex:
        case GFXBufferHintDataUniform:
        {
            const GLenum Target = GLBufferTarget(Buffer->hint);
            
            CC_GL_BIND_BUFFER_TARGET(Target, Buffer->gl.buffer);
            glGetBufferSubData(Target, Offset, Size, Data);
            if (CC_GL_CHECK() == GL_NO_ERROR) return Size;
        }
    }
    
    return 0;
}

static size_t GLBufferWriteBuffer(GLBuffer Buffer, ptrdiff_t Offset, size_t Size, const void *Data)
{
    switch (Buffer->hint & GFXBufferHintDataMask)
    {
        case GFXBufferHintData: //cpu store
            return CCDataWriteBuffer(Buffer->data, Offset, Size, Data);
            
        case GFXBufferHintDataVertex:
        case GFXBufferHintDataIndex:
        case GFXBufferHintDataUniform:
        {
            const GLenum Target = GLBufferTarget(Buffer->hint);
            
            CC_GL_BIND_BUFFER_TARGET(Target, Buffer->gl.buffer);
            glBufferSubData(Target, Offset, Size, Data);
            if (CC_GL_CHECK() == GL_NO_ERROR) return Size;
        }
    }
    
    return 0;
}

static void GLBufferInvalidate(GLBuffer Buffer)
{
    switch (Buffer->hint & GFXBufferHintDataMask)
    {
        case GFXBufferHintData: //cpu store
            CCDataInvalidate(Buffer->data);
            break;
            
        case GFXBufferHintDataVertex:
        case GFXBufferHintDataIndex:
        case GFXBufferHintDataUniform:
        {
            const GLenum Target = GLBufferTarget(Buffer->hint);
            
            CC_GL_BIND_BUFFER_TARGET(Target, Buffer->gl.buffer);
            glBufferData(Target, Buffer->gl.size, NULL, GLBufferUsage(Buffer->hint)); CC_GL_CHECK();
        }
    }
}
