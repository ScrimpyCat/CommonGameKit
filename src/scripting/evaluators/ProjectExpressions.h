/*
 *  Copyright (c) 2015, Stefan Johnson
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

#ifndef Blob_Game_ProjectExpressions_h
#define Blob_Game_ProjectExpressions_h

#include "ExpressionEvaluator.h"
#include "AssetExpressions.h"
#include "GFX.h"

/*
 (game "Game" ; title
 (default-resolution 640 480) ; resolution or window size
 (default-fullscreen false) ; fullscreen mode
 
 (dir-fonts "font/") ; font directories
 (dir-levels "logic/levels/") ; level directories
 (dir-rules "logic/rules/") ; rule directories
 (dir-textures "graphics/textures/") ; texture directories
 (dir-shaders "graphics/shaders/") ; shader directories
 (dir-sounds "audio/") ; sound directories
 (dir-layouts "ui/") ; layout directories
 (dir-entities "logic/entities/") ; entity directories
 (dir-logs "logs/") ; log directory
 (dir-tmp "tmp/") ; tmp directory
 )
 */

typedef enum {
    CCProjectExpressionValueTypeGameConfig = CCAssetExpressionValueTypeReservedCount + 1,
    CCProjectExpressionValueTypeShaderLibrary,
    CCProjectExpressionValueTypeReservedCount
} CCProjectExpressionValueType;

typedef struct {
    CCString name;
    GFXShaderLibrary library;
} CCProjectExpressionValueShaderLibrary;


CC_EXPRESSION_EVALUATOR(game) CCExpression CCProjectExpressionGame(CCExpression Expression);
CC_EXPRESSION_EVALUATOR(library) CCExpression CCProjectExpressionLibrary(CCExpression Expression);
CC_EXPRESSION_EVALUATOR(source) CCExpression CCProjectExpressionLibrarySource(CCExpression Expression);
CC_EXPRESSION_EVALUATOR(asset) CCExpression CCProjectExpressionAsset(CCExpression Expression);

#endif
