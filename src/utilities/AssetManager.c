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
    CCAssetTypeShader
} CCAssetType;

typedef struct {
    CCAssetType type;
    CCString name;
    void *asset;
    //FSPath reference
} CCAssetInfo;

static CCCollection ShaderAssets = NULL;

static void CCAssetElementDestructor(CCCollection Collection, CCAssetInfo *Element)
{
    CCStringDestroy(Element->name);
    
    switch (Element->type)
    {
        case CCAssetTypeShader:
            GFXShaderDestroy(Element->asset);
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
    ShaderAssets = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintHeavyFinding, sizeof(CCAssetInfo), (CCCollectionElementDestructor)CCAssetElementDestructor);
}

void CCAssetManagerDestroy(void)
{
    CCCollectionDestroy(ShaderAssets);
}

void CCAssetManagerRegisterShader(CCString Name, GFXShader Shader)
{
    CCCollectionInsertElement(ShaderAssets, &(CCAssetInfo){
        .type = CCAssetTypeShader,
        .name = CCStringCopy(Name),
        .asset = CCRetain(Shader)
    });
}

void CCAssetManagerDeregisterShader(CCString Name)
{
    CCCollectionRemoveElement(ShaderAssets, CCCollectionFindElement(ShaderAssets, &(CCAssetInfo){ .name = Name }, (CCComparator)CCAssetElementNameComparator));
}

GFXShader CCAssetManagerCreateShader(CCString Name)
{
    CCAssetInfo *Asset = CCCollectionGetElement(ShaderAssets, CCCollectionFindElement(ShaderAssets, &(CCAssetInfo){ .name = Name }, (CCComparator)CCAssetElementNameComparator));
    
    if (!Asset)
    {
        CC_LOG_ERROR_CUSTOM("No shader asset available with name: %S", Name);
        return NULL;
    }
    
    return CCRetain(Asset->asset);
}