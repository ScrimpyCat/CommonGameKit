/*
 *  Copyright (c) 2020, Stefan Johnson
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

#ifndef CommonGameKit_TypeCallbacks_h
#define CommonGameKit_TypeCallbacks_h

#include <CommonC/Common.h>

#pragma mark - Collection Callbacks

extern const CCCollectionElementDestructor CCEntityDestructorForCollection;
extern const CCCollectionElementDestructor CCComponentDestructorForCollection;
extern const CCCollectionElementDestructor GUIObjectDestructorForCollection;
extern const CCCollectionElementDestructor CCPixelDataDestructorForCollection;
extern const CCCollectionElementDestructor GFXBlitDestructorForCollection;
extern const CCCollectionElementDestructor GFXBufferDestructorForCollection;
extern const CCCollectionElementDestructor GFXDrawDestructorForCollection;
extern const CCCollectionElementDestructor GFXFramebufferDestructorForCollection;
extern const CCCollectionElementDestructor GFXShaderDestructorForCollection;
extern const CCCollectionElementDestructor GFXShaderLibraryDestructorForCollection;
extern const CCCollectionElementDestructor GFXTextureStreamDestructorForCollection;
extern const CCCollectionElementDestructor GFXTextureDestructorForCollection;
extern const CCCollectionElementDestructor GFXCommandBufferDestructorForCollection;
extern const CCCollectionElementDestructor CCExpressionDestructorForCollection;
extern const CCCollectionElementDestructor CCFontDestructorForCollection;
extern const CCCollectionElementDestructor CCTextDestructorForCollection;

#pragma mark - Dictionary Callbacks

extern const CCDictionaryElementDestructor CCEntityDestructorForDictionary;
extern const CCDictionaryElementDestructor CCComponentDestructorForDictionary;
extern const CCDictionaryElementDestructor GUIObjectDestructorForDictionary;
extern const CCDictionaryElementDestructor CCPixelDataDestructorForDictionary;
extern const CCDictionaryElementDestructor GFXBlitDestructorForDictionary;
extern const CCDictionaryElementDestructor GFXBufferDestructorForDictionary;
extern const CCDictionaryElementDestructor GFXDrawDestructorForDictionary;
extern const CCDictionaryElementDestructor GFXFramebufferDestructorForDictionary;
extern const CCDictionaryElementDestructor GFXShaderDestructorForDictionary;
extern const CCDictionaryElementDestructor GFXShaderLibraryDestructorForDictionary;
extern const CCDictionaryElementDestructor GFXTextureStreamDestructorForDictionary;
extern const CCDictionaryElementDestructor GFXTextureDestructorForDictionary;
extern const CCDictionaryElementDestructor GFXCommandBufferDestructorForDictionary;
extern const CCDictionaryElementDestructor CCExpressionDestructorForDictionary;
extern const CCDictionaryElementDestructor CCFontDestructorForDictionary;
extern const CCDictionaryElementDestructor CCTextDestructorForDictionary;

#endif
