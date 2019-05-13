/*
 *  Copyright (c) 2019, Stefan Johnson
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

#import "MTLShader.h"

static MTLGFXShader ShaderConstructor(CCAllocatorType Allocator, CFTypeRef Vertex, CFTypeRef Fragment);
static void ShaderDestructor(MTLGFXShader Shader);
static void *ShaderGetInput(MTLGFXShader Shader, CCString Name);


const GFXShaderInterface MTLShaderInterface = {
    .create = (GFXShaderConstructorCallback)ShaderConstructor,
    .destroy = (GFXShaderDestructorCallback)ShaderDestructor,
    .input = (GFXShaderGetInputCallback)ShaderGetInput
};

static void ShaderDestroy(MTLGFXShader Shader)
{
    if (Shader->vert) CFRelease((__bridge CFTypeRef)Shader->vert);
    if (Shader->frag) CFRelease((__bridge CFTypeRef)Shader->frag);
}

static MTLGFXShader ShaderConstructor(CCAllocatorType Allocator, CFTypeRef Vertex, CFTypeRef Fragment)
{
    MTLGFXShader Shader = CCMalloc(Allocator, sizeof(MTLGFXShaderInfo), NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (Shader)
    {
        CCMemorySetDestructor(Shader, (CCMemoryDestructorCallback)ShaderDestroy);
        
        @autoreleasepool {
            Shader->vert = (__bridge id<MTLFunction>)CFRetain(Vertex);
            Shader->frag = (__bridge id<MTLFunction>)CFRetain(Fragment);
        }
    }
    
    return Shader;
}

static void ShaderDestructor(MTLGFXShader Shader)
{
    @autoreleasepool {
        CC_SAFE_Free(Shader);
    }
}

static void *ShaderGetInput(MTLGFXShader Shader, CCString Name)
{
    return NULL;
}
