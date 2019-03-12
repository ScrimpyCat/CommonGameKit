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
static GLShaderLibrary GLShaderLibraryPrecompiledConstructor(CCAllocatorType Allocator, const void *Data);
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
    .createPrecompiled = (GFXShaderLibraryPrecompiledConstructorCallback)GLShaderLibraryPrecompiledConstructor,
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

static GLShaderLibrary GLShaderLibraryPrecompiledConstructor(CCAllocatorType Allocator, const void *Data)
{
    CCAssertLog(0, "Precompiled libraries are unsupported");
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

_Bool GLShaderLibraryPreprocessSource(const char *Source, CCArray Sections, CCArray SectionLengths)
{
    const char *CurSource = Source, *Segment = Source, Include[] = "#include <";
    enum {
        GLShaderLibrarySourceTokenNone,
        GLShaderLibrarySourceTokenComment,
        GLShaderLibrarySourceTokenMultilineComment,
        GLShaderLibrarySourceTokenOther
    } Token = GLShaderLibrarySourceTokenNone;
    for (char Chr, PrevChr = 0, IncludeChrCount = 0; (Chr = *CurSource); CurSource++)
    {
        switch (Token)
        {
            case GLShaderLibrarySourceTokenNone:
                if (IncludeChrCount == (sizeof(Include) - 1))
                {
                    const char *End = strchr(CurSource, '>');
                    if (End)
                    {
                        if (GFXShaderLibraryForName)
                        {
                            CCArrayAppendElement(Sections, &Segment);
                            CCArrayAppendElement(SectionLengths, &(GLint){ (GLint)(CurSource - (Segment + sizeof(Include) - 1)) });
                            Segment = End + 1;
                            
                            CCString Path = CCStringCreateWithSize(CC_STD_ALLOCATOR, (CCStringHint)CCStringEncodingASCII, CurSource, End - CurSource);
                            CCOrderedCollection Reference = CCStringCreateBySeparatingOccurrencesOfString(Path, CC_STRING("/"));
                            CCStringDestroy(Path);
                            
                            CCEnumerator Enumerator;
                            CCCollectionGetEnumerator(Reference, &Enumerator);
                            
                            _Bool Included = FALSE;
                            CCString *Library = CCCollectionEnumeratorGetCurrent(&Enumerator);
                            if (Library)
                            {
                                GLShaderLibrary Lib = (GLShaderLibrary)GFXShaderLibraryForName(*Library);
                                if (Lib)
                                {
                                    CCString *Header = CCCollectionEnumeratorNext(&Enumerator);
                                    if (Header)
                                    {
                                        GLShaderLibrarySource *Source = CCDictionaryGetValue(Lib, Header);
                                        if (Source->type == GFXShaderSourceTypeHeader)
                                        {
                                            GLShaderLibraryPreprocessSource(Source->source.header, Sections, SectionLengths);
                                            Included = TRUE;
                                        }
                                    }
                                    
                                    else
                                    {
                                        CC_DICTIONARY_FOREACH_VALUE_PTR(GLShaderLibrarySource, Source, Lib)
                                        {
                                            if (Source->type == GFXShaderSourceTypeHeader)
                                            {
                                                GLShaderLibraryPreprocessSource(Source->source.header, Sections, SectionLengths);
                                            }
                                        }
                                        
                                        Included = TRUE;
                                    }
                                    
                                    GFXShaderLibraryDestroy((GFXShaderLibrary)Lib);
                                }
                            }
                            
                            CCCollectionDestroy(Reference);
                            
                            if (!Included)
                            {
                                CC_LOG_ERROR_CUSTOM("No header could be resolved for: #include <%S>", Path);
                                return FALSE;
                            }
                            
                            CurSource = End;
                        }
                        
                        else
                        {
                            CC_LOG_ERROR("Cannot support #include unless a callback to GFXShaderLibraryForName is provided.");
                            return FALSE;
                        }
                    }
                    
                    IncludeChrCount = 0;
                }
                
                else
                {
                    if (Include[IncludeChrCount] == Chr) IncludeChrCount++;
                    else
                    {
                        IncludeChrCount = 0;
                        
                        if (!isspace(Chr)) Token = GLShaderLibrarySourceTokenOther;
                    }
                }
                break;
                
            case GLShaderLibrarySourceTokenOther:
                if ((PrevChr == '/') && (Chr == '/'))
                {
                    Token = GLShaderLibrarySourceTokenComment;
                    PrevChr = 0;
                    break;
                }
                
                else if ((PrevChr == '/') && (Chr == '*'))
                {
                    Token = GLShaderLibrarySourceTokenMultilineComment;
                    PrevChr = 0;
                    break;
                }
            case GLShaderLibrarySourceTokenComment:
                if (Chr == '\n') Token = GLShaderLibrarySourceTokenNone;
                break;
                
            case GLShaderLibrarySourceTokenMultilineComment:
                if ((PrevChr == '*') && (Chr == '/'))
                {
                    Token = GLShaderLibrarySourceTokenNone;
                    PrevChr = 0;
                    continue;
                }
                break;
        }
        
        PrevChr = Chr;
    }
    
    CCArrayAppendElement(Sections, &Segment);
    CCArrayAppendElement(SectionLengths, &(GLint){ (GLint)(CurSource - Segment) });
    
    return TRUE;
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
        
        CCDictionarySetValue(Library, &(CCString){ CCStringCopy(Name) }, &Shader);
        
        return 0;
    }
    
    CC_GL_PUSH_GROUP_MARKER("Compile Shader");
    
    GLuint Shader = glCreateShader(GLShaderType(Type)); CC_GL_CHECK();
    
    CCArray Sections = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(char*), 4), SectionLengths = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(GLint), 4);
    if (!GLShaderLibraryPreprocessSource(Source, Sections, SectionLengths))
    {
        CCArrayDestroy(Sections);
        CCArrayDestroy(SectionLengths);
        
        CC_LOG_ERROR_CUSTOM("Failed to compile %s shader (%S). Unable to preprocess the source.", GLShaderTypeString(Type), Name);
        return 0;
    }
    
    glShaderSource(Shader, (GLsizei)CCArrayGetCount(Sections), Sections->data, SectionLengths->data); CC_GL_CHECK();
    CCArrayDestroy(Sections);
    CCArrayDestroy(SectionLengths);
    
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
