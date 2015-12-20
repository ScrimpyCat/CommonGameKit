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

#ifndef Blob_Game_GLShader_h
#define Blob_Game_GLShader_h

#include "GLGFX.h"
#include "GLSetup.h"


typedef struct {
    enum {
        GLShaderInputTypeAttribute,
        GLShaderInputTypeUniform,
        GLShaderInputTypeUniformBlock
    } type;
} GLShaderInputInfo;

typedef struct {
    GLShaderInputInfo input;
    char *name; //TODO: implement string type
    //TODO: add cacheName for constant strings
    GLuint location;
    GFXBufferFormat type;
} GLShaderAttributeInfo;

typedef struct {
    GLShaderInputInfo input;
    char *name;
    //cacheName
    GLint location;
    GFXBufferFormat type;
    GLint count;
    _Alignas(double) uint8_t value[sizeof(double) * 4 * 4];
} GLShaderUniformInfo;

typedef struct {
    GLuint program;
    CCCollection attributes;
    CCCollection uniforms;
} GLShaderInfo, *GLShader;

extern const GFXShaderInterface GLShaderInterface;

#endif
