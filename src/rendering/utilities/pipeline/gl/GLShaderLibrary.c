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

#include "GLShaderLibrary.h"

static GLShaderLibrary GLShaderLibraryConstructor(CCAllocatorType Allocator);
static void GLShaderLibraryDestructor(GLShaderLibrary Library);
static const GLShaderSource GLShaderLibraryCompile(GLShaderLibrary Library, GFXShaderSourceType Type, CCString Name, const char *Source);
static const GLShaderSource GLShaderLibraryGetSource(GLShaderLibrary Library, CCString Name);

typedef struct {
    GFXShaderSourceType type;
    union {
        GLuint shader;
        char *header;
    } source;
} GLShaderLibrarySource;


const GFXShaderLibraryInterface GLShaderLibraryInterface = {
    .create = (GFXShaderLibraryConstructorCallback)GLShaderLibraryConstructor,
    .destroy = (GFXShaderLibraryDestructorCallback)GLShaderLibraryDestructor,
    .compile = (GFXShaderLibraryCompileCallback)GLShaderLibraryCompile,
    .source = (GFXShaderLibraryGetSourceCallback)GLShaderLibraryGetSource
};


static void GLShaderLibraryElementDestructor(CCDictionary Dictionary, GLShaderLibrarySource *Element)
{
    if (Element->type == GFXShaderSourceTypeHeader) CCFree(Element->source.header);
    else
    {
        glDeleteShader(Element->source.shader); CC_GL_CHECK();
    }
}

static GLShaderLibrary GLShaderLibraryConstructor(CCAllocatorType Allocator)
{
    return CCDictionaryCreate(Allocator, CCDictionaryHintHeavyFinding, sizeof(CCString), sizeof(GLShaderLibrarySource), &(CCDictionaryCallbacks){
        .getHash = CCStringHasherForDictionary,
        .compareKeys = CCStringComparatorForDictionary,
        .keyDestructor = CCStringDestructorForDictionary,
        .valueDestructor = (CCDictionaryElementDestructor)GLShaderLibraryElementDestructor
    });
}

static void GLShaderLibraryDestructor(GLShaderLibrary Library)
{
    CCDictionaryDestroy(Library);
}

static inline GLenum GLShaderType(GFXShaderSourceType Type)
{
    switch (Type)
    {
        case GFXShaderSourceTypeVertex:
            return GL_VERTEX_SHADER;
            
        case GFXShaderSourceTypeFragment:
            return GL_FRAGMENT_SHADER;
            
        default:
            break;
    }
    
    CCAssertLog(0, "Invalid shader type");
}

static inline const char *GLShaderTypeString(GFXShaderSourceType Type)
{
    switch (Type)
    {
        case GFXShaderSourceTypeVertex:
            return "vertex";
            
        case GFXShaderSourceTypeFragment:
            return "fragment";
            
        default:
            break;
    }
    
    return NULL;
}

static const GLShaderSource GLShaderLibraryCompile(GLShaderLibrary Library, GFXShaderSourceType Type, CCString Name, const char *Source)
{
    if (Type == GFXShaderSourceTypeHeader)
    {
        GLShaderLibrarySource Shader = { .type = Type };
        CC_SAFE_Malloc(Shader.source.header, sizeof(char) * (strlen(Source) + 1),
                       CC_LOG_ERROR_CUSTOM("Failed to store shader (%S). Due to allocation failure.", Name);
                       return 0;
                       );
        strcpy(Shader.source.header, Source);
        
        return 0;
    }
    
    CC_GL_PUSH_GROUP_MARKER("Compile Shader");
    
    GLuint Shader = glCreateShader(GLShaderType(Type)); CC_GL_CHECK();
    glShaderSource(Shader, 1, &Source, NULL); CC_GL_CHECK();
    glCompileShader(Shader); CC_GL_CHECK();
    
    GLint Completed;
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &Completed); CC_GL_CHECK();
    
    if (!Completed)
    {
        GLsizei Length;
        glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &Length); CC_GL_CHECK();
        
        char *ErrorLog = NULL;
        CC_TEMP_Malloc(ErrorLog, Length,
                       CC_LOG_ERROR_CUSTOM("Failed to compile %s shader (%S). Unable to get reason.", GLShaderTypeString(Type), Name);
                       CC_GL_POP_GROUP_MARKER();
                       return 0;
                       );
        
        glGetShaderInfoLog(Shader, Length, NULL, ErrorLog); CC_GL_CHECK();
        CC_LOG_ERROR_CUSTOM("Failed to compile %s shader (%S). Reason:\n%s", GLShaderTypeString(Type), Name, ErrorLog);
        
        glDeleteShader(Shader); CC_GL_CHECK();
        
        CC_TEMP_Free(ErrorLog);
        CC_GL_POP_GROUP_MARKER();
        
        return 0;
    }
    
    CC_GL_POP_GROUP_MARKER();
    
    CCDictionarySetValue(Library, &(CCString){ CCStringCopy(Name) }, &(GLShaderLibrarySource){ .type = Type, .source = { .shader = Shader } });
    
    return (GLShaderSource)Shader;
}

static const GLShaderSource GLShaderLibraryGetSource(GLShaderLibrary Library, CCString Name)
{
    GLShaderLibrarySource *Source = CCDictionaryGetValue(Library, &Name);
    return (GLShaderSource)((Source) && (Source->type != GFXShaderSourceTypeHeader) ? Source->source.shader : 0);
}
