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
#include "Callbacks.h"

typedef struct {
    void *asset;
    double timestamp;
    void *reference;
    CCAssetManagerSourceCallbacks callbacks;
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
    
    if ((Element->reference) && (Element->callbacks.destructor)) Element->callbacks.destructor(Element->reference);
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

void CCAssetManagerRegister(CCAssetManager *Manager, const void *Identifier, void *Asset, void *Reference, const CCAssetManagerSourceCallbacks *Callbacks)
{
    CCAssetManagerRegisterAsset(Manager, Identifier, &(CCAssetInfo){
        .asset = CCRetain(Asset),
        .timestamp = CCTimestamp(),
        .reference = Reference,
        .callbacks = *Callbacks
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

void CCAssetManagerDeregisterAll(CCAssetManager *Manager)
{
    CCDictionary Assets = NULL;
    
    while (!atomic_flag_test_and_set(&Manager->lock)) CC_SPIN_WAIT();
    Assets = Manager->assets;
    Manager->assets = NULL;
    atomic_flag_clear(&Manager->lock);
    
    if (Assets) CCDictionaryDestroy(Assets);
}

void CCAssetManagerPurge(CCAssetManager *Manager, double Threshold)
{
    while (!atomic_flag_test_and_set(&Manager->lock)) CC_SPIN_WAIT();
    
    const double Timestamp = CCTimestamp() - Threshold;
    CC_DICTIONARY_FOREACH_VALUE_PTR(CCAssetInfo, Info, Manager->assets)
    {
        if (Info->timestamp < Timestamp)
        {
            CCFree(Info->asset);
            Info->asset = NULL;
        }
    }
    
    atomic_flag_clear(&Manager->lock);
}

void *CCAssetManagerCreate(CCAssetManager *Manager, const void *Identifier)
{
    void *Asset = NULL;
    
    while (!atomic_flag_test_and_set(&Manager->lock)) CC_SPIN_WAIT();
    
    if (Manager->assets)
    {
        CCAssetInfo *Info = CCDictionaryGetValue(Manager->assets, Identifier);
        
        if (Info)
        {
            if ((!Info->asset) && (Info->callbacks.asset)) Info->asset = Info->callbacks.asset(Info->reference);
            
            Asset = CCRetain(Info->asset);
            Info->timestamp = CCTimestamp();
        }
    }
    
    atomic_flag_clear(&Manager->lock);
    
    return Asset;
}

#pragma mark - Shader Library
static CCAssetManager ShaderLibraryManager = CC_ASSET_MANAGER_INIT(&CCAssetManagerNamedInterface);

void CCAssetManagerRegisterShaderLibrary(CCString Name, GFXShaderLibrary Library)
{
    CCAssetManagerRegister(&ShaderLibraryManager, &(CCString){ CCStringCopy(Name) }, Library, NULL, NULL);
}

void CCAssetManagerDeregisterShaderLibrary(CCString Name)
{
    CCAssetManagerDeregister(&ShaderLibraryManager, &Name);
}

void CCAssetManagerDeregisterAllShaderLibrary(void)
{
    CCAssetManagerDeregisterAll(&ShaderLibraryManager);
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
    CCAssetManagerRegister(&ShaderManager, &(CCString){ CCStringCopy(Name) }, Shader, NULL, NULL);
}

void CCAssetManagerDeregisterShader(CCString Name)
{
    CCAssetManagerDeregister(&ShaderManager, &Name);
}

void CCAssetManagerDeregisterAllShader(void)
{
    CCAssetManagerDeregisterAll(&ShaderManager);
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
    CCAssetManagerRegister(&TextureManager, &(CCString){ CCStringCopy(Name) }, Texture, NULL, NULL);
}

void CCAssetManagerDeregisterTexture(CCString Name)
{
    CCAssetManagerDeregister(&TextureManager, &Name);
}

void CCAssetManagerDeregisterAllTexture(void)
{
    CCAssetManagerDeregisterAll(&TextureManager);
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
    CCAssetManagerRegister(&TextureStreamManager, &(CCString){ CCStringCopy(Name) }, Stream, NULL, NULL);
}

void CCAssetManagerDeregisterTextureStream(CCString Name)
{
    CCAssetManagerDeregister(&TextureStreamManager, &Name);
}

void CCAssetManagerDeregisterAllTextureStream(void)
{
    CCAssetManagerDeregisterAll(&TextureStreamManager);
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
    CCAssetManagerRegister(&FontManager, &(CCString){ CCStringCopy(Name) }, Font, NULL, NULL);
}

void CCAssetManagerDeregisterFont(CCString Name)
{
    CCAssetManagerDeregister(&FontManager, &Name);
}

void CCAssetManagerDeregisterAllFont(void)
{
    CCAssetManagerDeregisterAll(&FontManager);
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
