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

#include "GFX.h"
#include "Font.h"

/*!
 * @brief Creates the global asset manager.
 */
void CCAssetManagerCreate(void);

/*!
 * @brief Destroys the global asset manager.
 */
void CCAssetManagerDestroy(void);

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
 * @brief Creates the registered texture.
 * @param Name The lookup name for the texture asset entry.
 * @return The texture. Must be destroyed.
 */
CC_NEW GFXTexture CCAssetManagerCreateTexture(CCString Name);

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
 * @brief Creates the registered font.
 * @param Name The lookup name for the font asset entry.
 * @return The font. Must be destroyed.
 */
CC_NEW CCFont CCAssetManagerCreateFont(CCString Name);

#endif
