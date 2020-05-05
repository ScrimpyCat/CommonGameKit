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

#define CC_QUICK_COMPILE
#include "AssetManager.h"

typedef struct {
    void *asset;
    FSPath reference;
} CCAssetInfo;

const CCAssetManagerInterface CCAssetManagerNamedInterface = {
    .identifier = {
        .destructor = &CCStringDestructorForDictionary,
        .hasher = &CCStringHasherForDictionary,
        .comparator = &CCStringComparatorForDictionary,
        .size = sizeof(CCString)
    }
};

static void CCAssetElementDestructor(CCDictionary Dictionary, CCAssetInfo *Element)
{
    CCFree(Element->asset);
    
    if (Element->reference) FSPathDestroy(Element->reference);
}

static void CCAssetManagerRegisterAsset(CCAssetManager *Manager, const void *Identifier, CCAssetInfo *Asset)
{
    while (!atomic_flag_test_and_set(&Manager->lock)) CC_SPIN_WAIT();
    
    if (!Manager->assets)
    {
        Manager->assets = CCDictionaryCreate(CC_STD_ALLOCATOR, CCDictionaryHintHeavyFinding, Manager->interface->identifier.size, sizeof(CCAssetInfo), &(CCDictionaryCallbacks){
            .getHash = *Manager->interface->identifier.hasher,
            .compareKeys = *Manager->interface->identifier.comparator,
            .keyDestructor = *Manager->interface->identifier.destructor,
            .valueDestructor = (CCDictionaryElementDestructor)CCAssetElementDestructor
        });
    }
    
    CCDictionarySetValue(Manager->assets, Identifier, Asset);
    
    atomic_flag_clear(&Manager->lock);
}

void CCAssetManagerRegister(CCAssetManager *Manager, const void *Identifier, void *Asset)
{
    CCAssetManagerRegisterAsset(Manager, Identifier, &(CCAssetInfo){
        .asset = CCRetain(Asset),
        .reference = NULL
    });
}

void CCAssetManagerDeregister(CCAssetManager *Manager, const void *Identifier)
{
    while (!atomic_flag_test_and_set(&Manager->lock)) CC_SPIN_WAIT();
    
    if (Manager->assets)
    {
        CCDictionaryRemoveValue(Manager->assets, Identifier);
    }
    
    atomic_flag_clear(&Manager->lock);
}

void *CCAssetManagerCreate(CCAssetManager *Manager, const void *Identifier)
{
    while (!atomic_flag_test_and_set(&Manager->lock)) CC_SPIN_WAIT();
    CCAssetInfo *Asset = Manager->assets ? CCDictionaryGetValue(Manager->assets, Identifier) : NULL;
    atomic_flag_clear(&Manager->lock);
    
    return Asset ? CCRetain(Asset->asset) : NULL;
}

#pragma mark - Shader Library
static CCAssetManager ShaderLibraryManager = CC_ASSET_MANAGER_INIT(&CCAssetManagerNamedInterface);

void CCAssetManagerRegisterShaderLibrary(CCString Name, GFXShaderLibrary Library)
{
    CCAssetManagerRegister(&ShaderLibraryManager, &(CCString){ CCStringCopy(Name) }, Library);
}

void CCAssetManagerDeregisterShaderLibrary(CCString Name)
{
    CCAssetManagerDeregister(&ShaderLibraryManager, &Name);
}

GFXShaderLibrary CCAssetManagerCreateShaderLibrary(CCString Name)
{
    void *Asset = CCAssetManagerCreate(&ShaderLibraryManager, &Name);
    
    if (!Asset)
    {
        CC_LOG_ERROR_CUSTOM("No shader library asset available with name: %S", Name);
        return NULL;
    }
    
    return Asset;
}

#pragma mark - Shader
static CCAssetManager ShaderManager = CC_ASSET_MANAGER_INIT(&CCAssetManagerNamedInterface);

void CCAssetManagerRegisterShader(CCString Name, GFXShader Shader)
{
    CCAssetManagerRegister(&ShaderManager, &(CCString){ CCStringCopy(Name) }, Shader);
}

void CCAssetManagerDeregisterShader(CCString Name)
{
    CCAssetManagerDeregister(&ShaderManager, &Name);
}

GFXShader CCAssetManagerCreateShader(CCString Name)
{
    void *Asset = CCAssetManagerCreate(&ShaderManager, &Name);
    
    if (!Asset)
    {
        CC_LOG_ERROR_CUSTOM("No shader asset available with name: %S", Name);
        return NULL;
    }
    
    return Asset;
}

#pragma mark - Texture
static CCAssetManager TextureManager = CC_ASSET_MANAGER_INIT(&CCAssetManagerNamedInterface);

void CCAssetManagerRegisterTexture(CCString Name, GFXTexture Texture)
{
    CCAssetManagerRegister(&TextureManager, &(CCString){ CCStringCopy(Name) }, Texture);
}

void CCAssetManagerDeregisterTexture(CCString Name)
{
    CCAssetManagerDeregister(&TextureManager, &Name);
}

GFXTexture CCAssetManagerCreateTexture(CCString Name)
{
    void *Asset = CCAssetManagerCreate(&TextureManager, &Name);
    
    if (!Asset)
    {
        CC_LOG_ERROR_CUSTOM("No texture asset available with name: %S", Name);
        return NULL;
    }
    
    return Asset;
}

#pragma mark - Texture Stream
static CCAssetManager TextureStreamManager = CC_ASSET_MANAGER_INIT(&CCAssetManagerNamedInterface);

void CCAssetManagerRegisterTextureStream(CCString Name, GFXTextureStream Stream)
{
    CCAssetManagerRegister(&TextureStreamManager, &(CCString){ CCStringCopy(Name) }, Stream);
}

void CCAssetManagerDeregisterTextureStream(CCString Name)
{
    CCAssetManagerDeregister(&TextureStreamManager, &Name);
}

GFXTextureStream CCAssetManagerCreateTextureStream(CCString Name)
{
    void *Asset = CCAssetManagerCreate(&TextureStreamManager, &Name);
    
    if (!Asset)
    {
        CC_LOG_ERROR_CUSTOM("No texture stream asset available with name: %S", Name);
        return NULL;
    }
    
    return Asset;
}

#pragma mark - Font
static CCAssetManager FontManager = CC_ASSET_MANAGER_INIT(&CCAssetManagerNamedInterface);
//TODO: Fix so it groups them into families (so we can have two arial fonts but they may differ by style or size)
void CCAssetManagerRegisterFont(CCString Name, CCFont Font)
{
    CCAssetManagerRegister(&FontManager, &(CCString){ CCStringCopy(Name) }, Font);
}

void CCAssetManagerDeregisterFont(CCString Name)
{
    CCAssetManagerDeregister(&FontManager, &Name);
}

CCFont CCAssetManagerCreateFont(CCString Name)
{
    void *Asset = CCAssetManagerCreate(&FontManager, &Name);
    
    if (!Asset)
    {
        CC_LOG_ERROR_CUSTOM("No font asset available with name: %S", Name);
        return NULL;
    }
    
    return Asset;
}
