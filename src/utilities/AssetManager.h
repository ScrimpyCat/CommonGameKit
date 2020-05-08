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

#ifndef CommonGameKit_AssetManager_h
#define CommonGameKit_AssetManager_h

#include <CommonGameKit/GFX.h>
#include <CommonGameKit/Font.h>

/*!
 * @brief A callback to handle custom destruction of asset source reference.
 * @param Reference The source reference to be destroyed.
 */
typedef void (*CCAssetManagerSourceDestructor)(void *Reference);

/*!
 * @brief A callback to handle custom asset creation from an assert source reference.
 * @param Reference The source reference to create the asset from. May be NULL.
 * @return The asset. Ownership should be passed to the caller.
 */
typedef CC_NEW void *(*CCAssetManagerSourceAsset)(void *Reference);


typedef struct {
    CCAssetManagerSourceAsset asset;
    CCAssetManagerSourceDestructor destructor;
} CCAssetManagerSourceCallbacks;

typedef struct {
    struct {
        const CCDictionaryElementDestructor *destructor;
        const CCDictionaryKeyHasher *hasher;
        const CCComparator *comparator;
        size_t size;
    } identifier;
} CCAssetManagerInterface;

typedef struct {
    const CCAssetManagerInterface *interface;
    CCDictionary assets;
    atomic_flag lock;
} CCAssetManager;

#define CC_ASSET_MANAGER_INIT(interface_) \
{ \
    .interface = interface_, \
    .assets = NULL, \
    .lock = ATOMIC_FLAG_INIT \
}

/*!
 * @brief Generic interface for assets to be identified by string names.
 */
extern const CCAssetManagerInterface CCAssetManagerNamedInterface;

/*!
 * @brief Registers the asset with the asset manager.
 * @note This should usually be wrapped by a simpler interface.
 * @param Manager The asset manager the asset should be registered with.
 * @param Identifier The pointer to the lookup identifier for the asset entry.
 * @param Asset The asset to be managed. Retains a reference to the asset.
 * @param Reference Optional asset source reference. May be NULL.
 * @param Callbacks Optional callbacks for the asset source. May be NULL.
 */
void CCAssetManagerRegister(CCAssetManager *Manager, const void *Identifier, void *CC_RETAIN(Asset), void *Reference, const CCAssetManagerSourceCallbacks *Callbacks);

/*!
 * @brief Deregisters the asset from the asset manager.
 * @note This should usually be wrapped by a simpler interface.
 * @param Manager The asset manager the asset should be deregistered from.
 * @param Identifier The pointer to the lookup identifier for the asset entry.
 */
void CCAssetManagerDeregister(CCAssetManager *Manager, const void *Identifier);

/*!
 * @brief Deregisters all assets from the asset manager.
 * @note This should usually be wrapped by a simpler interface.
 * @param Manager The asset manager the assets should be deregistered from.
 */
void CCAssetManagerDeregisterAll(CCAssetManager *Manager);

/*!
 * @brief Creates the registered asset.
 * @note This should usually be wrapped by a simpler interface.
 * @param Manager The asset manager the asset should be created from.
 * @param Identifier The pointer to the lookup identifier for the asset entry.
 * @return The asset. Must be destroyed.
 */
CC_NEW void *CCAssetManagerCreate(CCAssetManager *Manager, const void *Identifier);

#pragma mark -

/*!
 * @brief Registers the shader library with the asset manager.
 * @param Name The lookup name for the shader library asset entry.
 * @param Shader The shader library to be managed. Retains a reference to the 
 *        shader library.
 */
void CCAssetManagerRegisterShaderLibrary(CCString CC_COPY(Name), GFXShaderLibrary CC_RETAIN(Library));

/*!
 * @brief Deregisters the shader library from the asset manager.
 * @param Name The lookup name for the shader library asset entry.
 */
void CCAssetManagerDeregisterShaderLibrary(CCString Name);

/*!
 * @brief Deregisters all shader libraries from the asset manager.
 */
void CCAssetManagerDeregisterAllShaderLibrary(void);

/*!
 * @brief Creates the registered shader library.
 * @param Name The lookup name for the shader library asset entry.
 * @return The shader library. Must be destroyed.
 */
CC_NEW GFXShaderLibrary CCAssetManagerCreateShaderLibrary(CCString Name);

/*!
 * @brief Registers the shader with the asset manager.
 * @param Name The lookup name for the shader asset entry.
 * @param Shader The shader to be managed. Retains a reference to the shader.
 */
void CCAssetManagerRegisterShader(CCString CC_COPY(Name), GFXShader CC_RETAIN(Shader));

/*!
 * @brief Deregisters the shader from the asset manager.
 * @param Name The lookup name for the shader asset entry.
 */
void CCAssetManagerDeregisterShader(CCString Name);

/*!
 * @brief Deregisters all shaders from the asset manager.
 */
void CCAssetManagerDeregisterAllShader(void);

/*!
 * @brief Creates the registered shader.
 * @param Name The lookup name for the shader asset entry.
 * @return The shader. Must be destroyed.
 */
CC_NEW GFXShader CCAssetManagerCreateShader(CCString Name);

/*!
 * @brief Registers the texture with the asset manager.
 * @param Name The lookup name for the texture asset entry.
 * @param Texture The texture to be managed. Retains a reference to the texture.
 */
void CCAssetManagerRegisterTexture(CCString CC_COPY(Name), GFXTexture CC_RETAIN(Texture));

/*!
 * @brief Deregisters the texture from the asset manager.
 * @param Name The lookup name for the texture asset entry.
 */
void CCAssetManagerDeregisterTexture(CCString Name);

/*!
 * @brief Deregisters all textures from the asset manager.
 */
void CCAssetManagerDeregisterAllTexture(void);

/*!
 * @brief Creates the registered texture.
 * @param Name The lookup name for the texture asset entry.
 * @return The texture. Must be destroyed.
 */
CC_NEW GFXTexture CCAssetManagerCreateTexture(CCString Name);

/*!
 * @brief Registers the texture stream with the asset manager.
 * @param Name The lookup name for the texture stream asset entry.
 * @param Stream The texture stream to be managed. Retains a reference to the texture stream.
 */
void CCAssetManagerRegisterTextureStream(CCString CC_COPY(Name), GFXTextureStream CC_RETAIN(Stream));

/*!
 * @brief Deregisters the texture stream from the asset manager.
 * @param Name The lookup name for the texture stream asset entry.
 */
void CCAssetManagerDeregisterTextureStream(CCString Name);

/*!
 * @brief Deregisters all texture streams from the asset manager.
 */
void CCAssetManagerDeregisterAllTextureStream(void);

/*!
 * @brief Creates the registered texture stream.
 * @param Name The lookup name for the texture stream asset entry.
 * @return The texture stream. Must be destroyed.
 */
CC_NEW GFXTextureStream CCAssetManagerCreateTextureStream(CCString Name);

/*!
 * @brief Registers the font with the asset manager.
 * @param Name The lookup name for the font asset entry.
 * @param Font The font to be managed. Retains a reference to the font.
 */
void CCAssetManagerRegisterFont(CCString CC_COPY(Name), CCFont CC_RETAIN(Font));

/*!
 * @brief Deregisters the font from the asset manager.
 * @param Name The lookup name for the font asset entry.
 */
void CCAssetManagerDeregisterFont(CCString Name);

/*!
 * @brief Deregisters all fonts from the asset manager.
 */
void CCAssetManagerDeregisterAllFont(void);

/*!
 * @brief Creates the registered font.
 * @param Name The lookup name for the font asset entry.
 * @return The font. Must be destroyed.
 */
CC_NEW CCFont CCAssetManagerCreateFont(CCString Name);

#endif
