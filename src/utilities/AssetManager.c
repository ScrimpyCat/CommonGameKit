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

#include "AssetManager.h"

typedef enum {
    CCAssetTypeUndefined,
    CCAssetTypeShaderLibrary,
    CCAssetTypeShader,
    CCAssetTypeTexture,
    CCAssetTypeFont,
    CCAssetTypeCount
} CCAssetType;

typedef struct {
    CCAssetType type;
    CCString name;
    void *asset;
    //FSPath reference
} CCAssetInfo;

static CCCollection Assets[CCAssetTypeCount] = {NULL};

static void CCAssetElementDestructor(CCCollection Collection, CCAssetInfo *Element)
{
    CCStringDestroy(Element->name);
    
    switch (Element->type)
    {
        case CCAssetTypeShaderLibrary:
            GFXShaderLibraryDestroy(Element->asset);
            break;
            
        case CCAssetTypeShader:
            GFXShaderDestroy(Element->asset);
            break;
            
        case CCAssetTypeTexture:
            GFXTextureDestroy(Element->asset);
            break;
            
        case CCAssetTypeFont:
            CCFontDestroy(Element->asset);
            break;
            
        default:
            break;
    }
}

static CCComparisonResult CCAssetElementNameComparator(const CCAssetInfo *left, const CCAssetInfo *right)
{
    return CCStringEqual(left->name, right->name) ? CCComparisonResultEqual : CCComparisonResultInvalid;
}

void CCAssetManagerCreate(void)
{
    for (size_t Loop = 0; Loop < CCAssetTypeCount; Loop++)
    {
        Assets[Loop] = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintHeavyFinding, sizeof(CCAssetInfo), (CCCollectionElementDestructor)CCAssetElementDestructor);
    }
}

void CCAssetManagerDestroy(void)
{
    for (size_t Loop = 0; Loop < CCAssetTypeCount; Loop++)
    {
        CCCollectionDestroy(Assets[Loop]);
    }
}

#pragma mark - Shader Library
void CCAssetManagerRegisterShaderLibrary(CCString Name, GFXShaderLibrary Library)
{
    CCCollectionInsertElement(Assets[CCAssetTypeShaderLibrary], &(CCAssetInfo){
        .type = CCAssetTypeShader,
        .name = CCStringCopy(Name),
        .asset = CCRetain(Library)
    });
}

void CCAssetManagerDeregisterShaderLibrary(CCString Name)
{
    CCCollectionRemoveElement(Assets[CCAssetTypeShaderLibrary], CCCollectionFindElement(Assets[CCAssetTypeShaderLibrary], &(CCAssetInfo){ .name = Name }, (CCComparator)CCAssetElementNameComparator));
}

GFXShaderLibrary CCAssetManagerCreateShaderLibrary(CCString Name)
{
    CCAssetInfo *Asset = CCCollectionGetElement(Assets[CCAssetTypeShaderLibrary], CCCollectionFindElement(Assets[CCAssetTypeShaderLibrary], &(CCAssetInfo){ .name = Name }, (CCComparator)CCAssetElementNameComparator));
    
    if (!Asset)
    {
        CC_LOG_ERROR_CUSTOM("No shader library asset available with name: %S", Name);
        return NULL;
    }
    
    return CCRetain(Asset->asset);
}

#pragma mark - Shader
void CCAssetManagerRegisterShader(CCString Name, GFXShader Shader)
{
    CCCollectionInsertElement(Assets[CCAssetTypeShader], &(CCAssetInfo){
        .type = CCAssetTypeShader,
        .name = CCStringCopy(Name),
        .asset = CCRetain(Shader)
    });
}

void CCAssetManagerDeregisterShader(CCString Name)
{
    CCCollectionRemoveElement(Assets[CCAssetTypeShader], CCCollectionFindElement(Assets[CCAssetTypeShader], &(CCAssetInfo){ .name = Name }, (CCComparator)CCAssetElementNameComparator));
}

GFXShader CCAssetManagerCreateShader(CCString Name)
{
    CCAssetInfo *Asset = CCCollectionGetElement(Assets[CCAssetTypeShader], CCCollectionFindElement(Assets[CCAssetTypeShader], &(CCAssetInfo){ .name = Name }, (CCComparator)CCAssetElementNameComparator));
    
    if (!Asset)
    {
        CC_LOG_ERROR_CUSTOM("No shader asset available with name: %S", Name);
        return NULL;
    }
    
    return CCRetain(Asset->asset);
}

#pragma mark - Texture
void CCAssetManagerRegisterTexture(CCString Name, GFXTexture Texture)
{
    CCCollectionInsertElement(Assets[CCAssetTypeTexture], &(CCAssetInfo){
        .type = CCAssetTypeTexture,
        .name = CCStringCopy(Name),
        .asset = CCRetain(Texture)
    });
}

void CCAssetManagerDeregisterTexture(CCString Name)
{
    CCCollectionRemoveElement(Assets[CCAssetTypeTexture], CCCollectionFindElement(Assets[CCAssetTypeTexture], &(CCAssetInfo){ .name = Name }, (CCComparator)CCAssetElementNameComparator));
}

GFXTexture CCAssetManagerCreateTexture(CCString Name)
{
    CCAssetInfo *Asset = CCCollectionGetElement(Assets[CCAssetTypeTexture], CCCollectionFindElement(Assets[CCAssetTypeTexture], &(CCAssetInfo){ .name = Name }, (CCComparator)CCAssetElementNameComparator));
    
    if (!Asset)
    {
        CC_LOG_ERROR_CUSTOM("No texture asset available with name: %S", Name);
        return NULL;
    }
    
    return CCRetain(Asset->asset);
}

#pragma mark - Font
//TODO: Fix so it groups them into families (so we can have two arial fonts but they may differ by style or size)
void CCAssetManagerRegisterFont(CCString Name, CCFont Font)
{
    CCCollectionInsertElement(Assets[CCAssetTypeFont], &(CCAssetInfo){
        .type = CCAssetTypeFont,
        .name = CCStringCopy(Name),
        .asset = CCRetain(Font)
    });
}

void CCAssetManagerDeregisterFont(CCString Name)
{
    CCCollectionRemoveElement(Assets[CCAssetTypeFont], CCCollectionFindElement(Assets[CCAssetTypeFont], &(CCAssetInfo){ .name = Name }, (CCComparator)CCAssetElementNameComparator));
}

CCFont CCAssetManagerCreateFont(CCString Name)
{
    CCAssetInfo *Asset = CCCollectionGetElement(Assets[CCAssetTypeFont], CCCollectionFindElement(Assets[CCAssetTypeFont], &(CCAssetInfo){ .name = Name }, (CCComparator)CCAssetElementNameComparator));
    
    if (!Asset)
    {
        CC_LOG_ERROR_CUSTOM("No font asset available with name: %S", Name);
        return NULL;
    }
    
    return CCRetain(Asset->asset);
}
