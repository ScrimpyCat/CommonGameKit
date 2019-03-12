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

#import "MTLShaderLibrary.h"

static id <MTLLibrary>ShaderLibraryConstructor(CCAllocatorType Allocator);
static id <MTLLibrary>ShaderLibraryPrecompiledConstructor(CCAllocatorType Allocator, CCData Data);
static void ShaderLibraryDestructor(id <MTLLibrary>Library);
static id <MTLFunction>ShaderLibraryCompile(id <MTLLibrary>Library, GFXShaderSourceType Type, CCString Name, const char *Source);
static id <MTLFunction>ShaderLibraryGetSource(id <MTLLibrary>Library, CCString Name);


const GFXShaderLibraryInterface MTLShaderLibraryInterface = {
    .create = (GFXShaderLibraryConstructorCallback)ShaderLibraryConstructor,
    .createPrecompiled = (GFXShaderLibraryPrecompiledConstructorCallback)ShaderLibraryPrecompiledConstructor,
    .destroy = (GFXShaderLibraryDestructorCallback)ShaderLibraryDestructor,
    .compile = (GFXShaderLibraryCompileCallback)ShaderLibraryCompile,
    .source = (GFXShaderLibraryGetSourceCallback)ShaderLibraryGetSource
};

static id <MTLLibrary>ShaderLibraryConstructor(CCAllocatorType Allocator)
{
    CCAssertLog(0, "Runtime libraries are unsupported");
}

static id <MTLLibrary>ShaderLibraryPrecompiledConstructor(CCAllocatorType Allocator, CCData Data)
{
    //TODO: Update CCDataBuffer so you can change ownership of internal memory (and can use DISPATCH_DATA_DESTRUCTOR_FREE destructor)
    const size_t Size = CCDataGetSize(Data);
    const void *Buffer = CCDataGetBuffer(Data);
    dispatch_data_t Binary = dispatch_data_create(Buffer, Size, NULL, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
    if (!Buffer)
    {
        for (ptrdiff_t Offset = 0; Offset < Size; )
        {
            CCBufferMap Map = CCDataMapBuffer(Data, Offset, Size, CCDataHintRead);
            Offset += Map.size;
            
            dispatch_data_t Chunk = dispatch_data_create(Map.ptr, Map.size, NULL, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
            dispatch_data_t NewBinary = dispatch_data_create_concat(Binary, Chunk);
            
            Binary = NewBinary;
        }
    }
    
    CCDataDestroy(Data);
    
    NSError *Err;
    id <MTLLibrary>Library = [((MTLInternal*)MTLGFX->internal)->device newLibraryWithData: Binary error: &Err];
    
    if (Err) CC_LOG_ERROR("Failed to use precompiled shader library: %@", Err); //TODO: Move commonobjc log specifier to commonc
    
    return Library;
}

static void ShaderLibraryDestructor(id <MTLLibrary>Library)
{
    CFRelease((__bridge CFTypeRef)Library);
}

static id <MTLFunction>ShaderLibraryCompile(id <MTLLibrary>Library, GFXShaderSourceType Type, CCString Name, const char *Source)
{
    CCAssertLog(0, "Runtime libraries are unsupported");
}

static id <MTLFunction>ShaderLibraryGetSource(id <MTLLibrary>Library, CCString Name)
{
    id <MTLFunction> Function = nil;
    
    CC_STRING_TEMP_BUFFER(Buffer, Name) Function = [Library newFunctionWithName: [NSString stringWithUTF8String: Buffer]];
    
    return Function;
}
