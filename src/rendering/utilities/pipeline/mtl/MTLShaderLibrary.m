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

#define CC_QUICK_COMPILE
#import "MTLShaderLibrary.h"

static MTLGFXShaderLibrary ShaderLibraryConstructor(CCAllocatorType Allocator);
static MTLGFXShaderLibrary ShaderLibraryPrecompiledConstructor(CCAllocatorType Allocator, CCData Data);
static void ShaderLibraryDestructor(MTLGFXShaderLibrary Library);
static CFTypeRef ShaderLibraryCompile(MTLGFXShaderLibrary Library, GFXShaderSourceType Type, CCString Name, const char *Source);
static CFTypeRef ShaderLibraryGetSource(MTLGFXShaderLibrary Library, CCString Name);


const GFXShaderLibraryInterface MTLShaderLibraryInterface = {
    .create = (GFXShaderLibraryConstructorCallback)ShaderLibraryConstructor,
    .createPrecompiled = (GFXShaderLibraryPrecompiledConstructorCallback)ShaderLibraryPrecompiledConstructor,
    .destroy = (GFXShaderLibraryDestructorCallback)ShaderLibraryDestructor,
    .compile = (GFXShaderLibraryCompileCallback)ShaderLibraryCompile,
    .source = (GFXShaderLibraryGetSourceCallback)ShaderLibraryGetSource
};

static MTLGFXShaderLibrary ShaderLibraryConstructor(CCAllocatorType Allocator)
{
    CCAssertLog(0, "Runtime libraries are unsupported");
}

static void ShaderLibraryDestroy(MTLGFXShaderLibrary Library)
{
    if (Library->library) CFRelease((__bridge CFTypeRef)Library->library);
    if (Library->sources) CCDictionaryDestroy(Library->sources);
}

void ShaderLibrarySourceElementDestructor(CCDictionary Dictionary, CFTypeRef *Element)
{
    CFRelease(*Element);
}

static MTLGFXShaderLibrary ShaderLibraryPrecompiledConstructor(CCAllocatorType Allocator, CCData Data)
{
    MTLGFXShaderLibrary Library = CCMalloc(Allocator, sizeof(MTLGFXShaderLibraryInfo), NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (Library)
    {
        CCMemorySetDestructor(Library, (CCMemoryDestructorCallback)ShaderLibraryDestroy);
        
        @autoreleasepool {
            const size_t Size = CCDataGetSize(Data);
            const void *Buffer = CCDataGetBuffer(Data);
            dispatch_data_t Binary = dispatch_data_create(Buffer, Size, NULL, ^{
                CCDataDestroy(Data);
            });
            
            if (!Buffer)
            {
                for (ptrdiff_t Offset = 0; Offset < Size; )
                {
                    CCBufferMap Map = CCDataMapBuffer(Data, Offset, Size, CCDataHintRead);
                    Offset += Map.size;
                    
                    dispatch_data_t Chunk = dispatch_data_create(Map.ptr, Map.size, NULL, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
                    dispatch_data_t NewBinary = dispatch_data_create_concat(Binary, Chunk);
                    
                    Binary = NewBinary;
                    
                    CCDataUnmapBuffer(Data, Map);
                }
            }
            
            else CCRetain(Data);
            
            NSError *Err;
            Library->library = (__bridge id<MTLLibrary>)((__bridge_retained CFTypeRef)[((MTLInternal*)MTLGFX->internal)->device newLibraryWithData: Binary error: &Err]);
            
            if (Err)
            {
                CC_LOG_ERROR(@"Failed to use precompiled shader library: %@", Err);
                
                ShaderLibraryDestructor(Library);
                
                return NULL;
            }
            
            Library->sources = CCDictionaryCreate(Allocator, CCDictionaryHintSizeMedium | CCDictionaryHintHeavyFinding | CCDictionaryHintConstantLength | CCDictionaryHintConstantElements, sizeof(CCString), sizeof(CFTypeRef), &(CCDictionaryCallbacks){
                .keyDestructor = CCStringDestructorForDictionary,
                .valueDestructor = (CCDictionaryElementDestructor)ShaderLibrarySourceElementDestructor,
                .getHash = CCStringHasherForDictionary,
                .compareKeys = CCStringComparatorForDictionary
            });
            
            for (NSString *Name in Library->library.functionNames)
            {
                CCString Key = CCStringCreate(Allocator, CCStringEncodingUTF8 | CCStringHintCopy, [Name UTF8String]);
                CCDictionarySetValue(Library->sources, &Key, &(CFTypeRef){ (__bridge_retained CFTypeRef)[Library->library newFunctionWithName: Name] });
            }
        }
    }
    
    return Library;
}

static void ShaderLibraryDestructor(MTLGFXShaderLibrary Library)
{
    @autoreleasepool {
        CC_SAFE_Free(Library);
    }
}

static CFTypeRef ShaderLibraryCompile(MTLGFXShaderLibrary Library, GFXShaderSourceType Type, CCString Name, const char *Source)
{
    CCAssertLog(0, "Runtime libraries are unsupported");
}

static CFTypeRef ShaderLibraryGetSource(MTLGFXShaderLibrary Library, CCString Name)
{
    CFTypeRef *Source = CCDictionaryGetValue(Library->sources, &Name);

    return Source ? *Source : NULL;
}
