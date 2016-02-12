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
static const GLShaderSource GLShaderLibraryCompile(GLShaderLibrary Library, GFXShaderSourceType Type, const char *Name, const char *Source);
static const GLShaderSource GLShaderLibraryGetSource(GLShaderLibrary Library, const char *Name);


const GFXShaderLibraryInterface GLShaderLibraryInterface = {
    .create = (GFXShaderLibraryConstructorCallback)GLShaderLibraryConstructor,
    .destroy = (GFXShaderLibraryDestructorCallback)GLShaderLibraryDestructor,
    .compile = (GFXShaderLibraryCompileCallback)GLShaderLibraryCompile,
    .source = (GFXShaderLibraryGetSourceCallback)GLShaderLibraryGetSource
};


static void GLShaderLibraryElementDestructor(CCCollection Collection, GLShaderSource Element)
{
    CC_SAFE_Free(Element->name);
    glDeleteShader(Element->shader); CC_GL_CHECK();
}

static GLShaderLibrary GLShaderLibraryConstructor(CCAllocatorType Allocator)
{
    return CCCollectionCreate(Allocator, CCCollectionHintHeavyFinding, sizeof(GLShaderSourceInfo), (CCCollectionElementDestructor)GLShaderLibraryElementDestructor);
}

static void GLShaderLibraryDestructor(GLShaderLibrary Library)
{
    CCCollectionDestroy(Library);
}

static inline GLenum GLShaderType(GFXShaderSourceType Type)
{
    switch (Type)
    {
        case GFXShaderSourceTypeVertex:
            return GL_VERTEX_SHADER;
            
        case GFXShaderSourceTypeFragment:
            return GL_FRAGMENT_SHADER;
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
    }
    
    return NULL;
}

static const GLShaderSource GLShaderLibraryCompile(GLShaderLibrary Library, GFXShaderSourceType Type, const char *Name, const char *Source)
{
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
                       CC_LOG_ERROR("Failed to compile %s shader (%s). Unable to get reason.", GLShaderTypeString(Type), Name);
                       CC_GL_POP_GROUP_MARKER();
                       return NULL;
                       );
        
        glGetShaderInfoLog(Shader, Length, NULL, ErrorLog); CC_GL_CHECK();
        CC_LOG_ERROR("Failed to compile %s shader (%s). Reason:\n%s", GLShaderTypeString(Type), Name, ErrorLog);
        
        glDeleteShader(Shader); CC_GL_CHECK();
        
        CC_TEMP_Free(ErrorLog);
        CC_GL_POP_GROUP_MARKER();
        
        return NULL;
    }
    
    char *Str;
    CC_SAFE_Malloc(Str, sizeof(char) * (strlen(Name) + 1),
                   CC_LOG_ERROR("Failed to create shader source due to allocation failure. Allocation size: %zu", sizeof(char) * (strlen(Name) + 1));
                   glDeleteShader(Shader); CC_GL_CHECK();
                   CC_GL_POP_GROUP_MARKER();
                   return NULL;
                   );
    
    strcpy(Str, Name);
    
    
    CCCollectionEntry Entry = CCCollectionInsertElement(Library, &(GLShaderSourceInfo){ .name = Str, .shader = Shader });
    
    CC_GL_POP_GROUP_MARKER();
    
    return CCCollectionGetElement(Library, Entry);
}

static CCComparisonResult GLShaderLibraryElementFind(const GLShaderSource left, const GLShaderSource right)
{
    return !strcmp(left->name, right->name) ? CCComparisonResultEqual : CCComparisonResultInvalid;
}

static const GLShaderSource GLShaderLibraryGetSource(GLShaderLibrary Library, const char *Name)
{
    return CCCollectionGetElement(Library, CCCollectionFindElement(Library, &(GLShaderSourceInfo){ .name = (char*)Name }, (CCComparator)GLShaderLibraryElementFind));
}
