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

#include "GLShader.h"
#include "GLShaderLibrary.h"

static GLShader GLShaderConstructor(CCAllocatorType Allocator, GLShaderSource Vertex, GLShaderSource Fragment);
static void GLShaderDestructor(GLShader Shader);
static GLShaderInputInfo *GLShaderGetInput(GLShader Shader, const char *Name);


const GFXShaderInterface GLShaderInterface = {
    .create = (GFXShaderConstructorCallback)GLShaderConstructor,
    .destroy = (GFXShaderDestructorCallback)GLShaderDestructor,
    .input = (GFXShaderGetInputCallback)GLShaderGetInput,
};


static void GLShaderAttributeElementDestructor(CCCollection Collection, GLShaderAttributeInfo *Element)
{
    CC_SAFE_Free(Element->name);
}

static void GLShaderUniformElementDestructor(CCCollection Collection, GLShaderUniformInfo *Element)
{
    CC_SAFE_Free(Element->name);
    CC_SAFE_Free(Element->value);
}

static GFXBufferFormat GLShaderBufferFormatFromType(GLenum Type)
{
    switch (Type)
    {
        case GL_BOOL:
            return GFXBufferFormatInt8;
            
        case GL_BOOL_VEC2:
            return GFXBufferFormatInt8x2;
            
        case GL_BOOL_VEC3:
            return GFXBufferFormatInt8x3;
            
        case GL_BOOL_VEC4:
            return GFXBufferFormatInt8x4;
            
        case GL_BYTE:
            return GFXBufferFormatInt8;
            
        case GL_UNSIGNED_BYTE:
            return GFXBufferFormatUInt8;
            
        case GL_SHORT:
            return GFXBufferFormatInt16;
            
        case GL_UNSIGNED_SHORT:
            return GFXBufferFormatUInt16;
            
        case GL_INT:
            return GFXBufferFormatInt32;
            
        case GL_INT_VEC2:
            return GFXBufferFormatInt32x2;
            
        case GL_INT_VEC3:
            return GFXBufferFormatInt32x3;
            
        case GL_INT_VEC4:
            return GFXBufferFormatInt32x4;
            
        case GL_UNSIGNED_INT:
            return GFXBufferFormatUInt32;
            
        case GL_UNSIGNED_INT_VEC2:
            return GFXBufferFormatUInt32x2;
            
        case GL_UNSIGNED_INT_VEC3:
            return GFXBufferFormatUInt32x3;
            
        case GL_UNSIGNED_INT_VEC4:
            return GFXBufferFormatUInt32x4;
            
        case GL_FLOAT:
            return GFXBufferFormatFloat32;
            
        case GL_FLOAT_VEC2:
            return GFXBufferFormatFloat32x2;
            
        case GL_FLOAT_VEC3:
            return GFXBufferFormatFloat32x3;
            
        case GL_FLOAT_VEC4:
            return GFXBufferFormatFloat32x4;
            
        case GL_FLOAT_MAT2:
            return GFXBufferFormatFloat32x2x2;
            
        case GL_FLOAT_MAT3:
            return GFXBufferFormatFloat32x3x3;
            
        case GL_FLOAT_MAT4:
            return GFXBufferFormatFloat32x4x4;
            
        case GL_FLOAT_MAT2x3:
            return GFXBufferFormatFloat32x2x3;
            
        case GL_FLOAT_MAT2x4:
            return GFXBufferFormatFloat32x2x4;
            
        case GL_FLOAT_MAT3x2:
            return GFXBufferFormatFloat32x3x2;
            
        case GL_FLOAT_MAT3x4:
            return GFXBufferFormatFloat32x3x4;
            
        case GL_FLOAT_MAT4x2:
            return GFXBufferFormatFloat32x4x2;
            
        case GL_FLOAT_MAT4x3:
            return GFXBufferFormatFloat32x4x3;
            
        case GL_DOUBLE:
            return GFXBufferFormatFloat64;
            
        case GL_DOUBLE_VEC2:
            return GFXBufferFormatFloat64x2;
            
        case GL_DOUBLE_VEC3:
            return GFXBufferFormatFloat64x3;
            
        case GL_DOUBLE_VEC4:
            return GFXBufferFormatFloat64x4;
            
        case GL_DOUBLE_MAT2:
            return GFXBufferFormatFloat64x2x2;
            
        case GL_DOUBLE_MAT3:
            return GFXBufferFormatFloat64x3x3;
            
        case GL_DOUBLE_MAT4:
            return GFXBufferFormatFloat64x4x4;
            
        case GL_DOUBLE_MAT2x3:
            return GFXBufferFormatFloat64x2x3;
            
        case GL_DOUBLE_MAT2x4:
            return GFXBufferFormatFloat64x2x4;
            
        case GL_DOUBLE_MAT3x2:
            return GFXBufferFormatFloat64x3x2;
            
        case GL_DOUBLE_MAT3x4:
            return GFXBufferFormatFloat64x3x4;
            
        case GL_DOUBLE_MAT4x2:
            return GFXBufferFormatFloat64x4x2;
            
        case GL_DOUBLE_MAT4x3:
            return GFXBufferFormatFloat64x4x3;
            
        case GL_SAMPLER_1D:
        case GL_SAMPLER_2D:
        case GL_SAMPLER_3D:
        case GL_SAMPLER_CUBE:
        case GL_SAMPLER_1D_SHADOW:
        case GL_SAMPLER_2D_SHADOW:
        case GL_SAMPLER_1D_ARRAY:
        case GL_SAMPLER_2D_ARRAY:
        case GL_SAMPLER_1D_ARRAY_SHADOW:
        case GL_SAMPLER_2D_ARRAY_SHADOW:
        case GL_SAMPLER_2D_MULTISAMPLE:
        case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
        case GL_SAMPLER_CUBE_SHADOW:
        case GL_SAMPLER_BUFFER:
        case GL_SAMPLER_2D_RECT:
        case GL_SAMPLER_2D_RECT_SHADOW:
        case GL_INT_SAMPLER_1D:
        case GL_INT_SAMPLER_2D:
        case GL_INT_SAMPLER_3D:
        case GL_INT_SAMPLER_CUBE:
        case GL_INT_SAMPLER_1D_ARRAY:
        case GL_INT_SAMPLER_2D_ARRAY:
        case GL_INT_SAMPLER_2D_MULTISAMPLE:
        case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
        case GL_INT_SAMPLER_BUFFER:
        case GL_INT_SAMPLER_2D_RECT:
        case GL_UNSIGNED_INT_SAMPLER_1D:
        case GL_UNSIGNED_INT_SAMPLER_2D:
        case GL_UNSIGNED_INT_SAMPLER_3D:
        case GL_UNSIGNED_INT_SAMPLER_CUBE:
        case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
        case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
        case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
        case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
        case GL_UNSIGNED_INT_SAMPLER_BUFFER:
        case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
            return GFXBufferFormatInt8;
    }
    
    CCAssertLog(0, "Unsupported type %d", Type);
}

static GLShader GLShaderConstructor(CCAllocatorType Allocator, GLShaderSource Vertex, GLShaderSource Fragment)
{
    CC_GL_PUSH_GROUP_MARKER("Create Shader");
    
    GLuint Program = glCreateProgram(); CC_GL_CHECK();
    glAttachShader(Program, Vertex->shader); CC_GL_CHECK();
    glAttachShader(Program, Fragment->shader); CC_GL_CHECK();
    glLinkProgram(Program); CC_GL_CHECK();
    
    GLint Linked;
    glGetProgramiv(Program, GL_LINK_STATUS, &Linked); CC_GL_CHECK();
    
    if (!Linked)
    {
        GLsizei Length;
        glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &Length); CC_GL_CHECK();
        
        char *ErrorLog = NULL;
        CC_TEMP_Malloc(ErrorLog, Length,
                       CC_LOG_ERROR("Failed to link shader program. Unable to get reason.");
                       CC_GL_POP_GROUP_MARKER();
                       return NULL;
                       );
        
        glGetProgramInfoLog(Program, Length, NULL, ErrorLog); CC_GL_CHECK();
        CC_LOG_ERROR("Failed to link shader program. Reason:\n%s", ErrorLog);
        
        glDeleteProgram(Program); CC_GL_CHECK();
        
        CC_TEMP_Free(ErrorLog);
        CC_GL_POP_GROUP_MARKER();
        
        return NULL;
    }
    
    
    GLint Active, MaxLength, Size;
    GLenum Type;
    glGetProgramiv(Program, GL_ACTIVE_ATTRIBUTES, &Active); CC_GL_CHECK();
    
    CCCollection Attributes = CCCollectionCreate(Allocator, CCCollectionHintSizeSmall | CCCollectionHintHeavyFinding | CCCollectionHintConstantLength | CCCollectionHintConstantElements, sizeof(GLShaderAttributeInfo), (CCCollectionElementDestructor)GLShaderAttributeElementDestructor);
    if (Active)
    {
        glGetProgramiv(Program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &MaxLength); CC_GL_CHECK();
        
        for (GLint Loop = 0; Loop < Active; Loop++)
        {
            GLShaderAttributeInfo CurrentAttribute = { .input = GLShaderInputTypeAttribute };
            if (!(CurrentAttribute.name = CCMalloc(Allocator, MaxLength, NULL, CC_DEFAULT_ERROR_CALLBACK)))
            {
                CC_LOG_ERROR("Failed to create shader program, due to failure to allocate name for attribute. Allocation size (%d)", MaxLength);
                
                glDeleteProgram(Program); CC_GL_CHECK();
                CCCollectionDestroy(Attributes);
                return NULL;
            }
            
            glGetActiveAttrib(Program, Loop, MaxLength, NULL, &Size, &Type, CurrentAttribute.name); CC_GL_CHECK();
            CurrentAttribute.type = GLShaderBufferFormatFromType(Type);
            CurrentAttribute.location = glGetAttribLocation(Program, CurrentAttribute.name); CC_GL_CHECK();
            
            CCCollectionInsertElement(Attributes, &CurrentAttribute);
        }
    }
    
    
    glGetProgramiv(Program, GL_ACTIVE_UNIFORMS, &Active); CC_GL_CHECK();
    
    CCCollection Uniforms = CCCollectionCreate(Allocator, CCCollectionHintSizeSmall | CCCollectionHintHeavyFinding | CCCollectionHintConstantLength | CCCollectionHintConstantElements, sizeof(GLShaderUniformInfo), (CCCollectionElementDestructor)GLShaderUniformElementDestructor);
    if (Active)
    {
        glGetProgramiv(Program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &MaxLength); CC_GL_CHECK();
        
        for (GLint Loop = 0; Loop < Active; Loop++)
        {
            //TODO: Handle structures (so can submit a buffer for a structure)
            GLShaderUniformInfo CurrentUniform = { .input = GLShaderInputTypeUniform };
            if (!(CurrentUniform.name = CCMalloc(Allocator, MaxLength, NULL, CC_DEFAULT_ERROR_CALLBACK)))
            {
                CC_LOG_ERROR("Failed to create shader program, due to failure to allocate name for attribute. Allocation size (%d)", MaxLength);
                
                glDeleteProgram(Program); CC_GL_CHECK();
                CCCollectionDestroy(Attributes);
                CCCollectionDestroy(Uniforms);
                return NULL;
            }
            
            GLsizei Length;
            glGetActiveUniform(Program, Loop, MaxLength, &Length, &Size, &Type, CurrentUniform.name); CC_GL_CHECK();
            CurrentUniform.type = GLShaderBufferFormatFromType(Type);
            CurrentUniform.count = Size;
            CurrentUniform.location = glGetUniformLocation(Program, CurrentUniform.name); CC_GL_CHECK();
            
            CCAssertLog(CurrentUniform.location != -1, "UBOs currently unsupported"); //TODO: Add support for UBOs
            
            if (Size > 1)
            {
                char *Array = CurrentUniform.name + Length;
                while (*Array-- != '[');
                
                Array[1] = 0;
            }
            
            const size_t BufferSize = GFXBufferFormatGetSize(CurrentUniform.type) * CurrentUniform.count;
            if ((CurrentUniform.value = CCMalloc(Allocator, BufferSize, NULL, CC_DEFAULT_ERROR_CALLBACK))) memset(CurrentUniform.value, 0, BufferSize);
            
            CCCollectionInsertElement(Uniforms, &CurrentUniform);
        }
    }
    
    CC_GL_POP_GROUP_MARKER();
    
    GLShader Shader = CCMalloc(Allocator, sizeof(GLShaderInfo), NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (Shader)
    {
        *Shader = (GLShaderInfo){
            .program = Program,
            .attributes = Attributes,
            .uniforms = Uniforms
        };
    }
    
    return Shader;
}

static void GLShaderDestructor(GLShader Shader)
{
    CCAssertLog(Shader, "Shader must not be null");
    
    glDeleteProgram(Shader->program); CC_GL_CHECK();
    
    CCCollectionDestroy(Shader->attributes);
    CCCollectionDestroy(Shader->uniforms);
    CC_SAFE_Free(Shader);
}

static CCComparisonResult GLShaderFindAttribute(const GLShaderAttributeInfo *left, const GLShaderAttributeInfo *right)
{
    return !strcmp(left->name, right->name) ? CCComparisonResultEqual : CCComparisonResultInvalid;
}

static CCComparisonResult GLShaderFindUniform(const GLShaderUniformInfo *left, const GLShaderUniformInfo *right)
{
    return !strcmp(left->name, right->name) ? CCComparisonResultEqual : CCComparisonResultInvalid;
}

static GLShaderInputInfo *GLShaderGetInput(GLShader Shader, const char *Name)
{
    CCAssertLog(Shader, "Shader must not be null");
    
    GLShaderInputInfo *Input = CCCollectionGetElement(Shader->attributes, CCCollectionFindElement(Shader->attributes, &(GLShaderAttributeInfo){ .name = (char*)Name }, (CCComparator)GLShaderFindAttribute));
    
    if (!Input)
    {
        Input = CCCollectionGetElement(Shader->uniforms, CCCollectionFindElement(Shader->uniforms, &(GLShaderUniformInfo){ .name = (char*)Name }, (CCComparator)GLShaderFindUniform));
        
        if (!Input) CC_LOG_DEBUG("Shader (%p) has no input: %s", Shader, Name);
    }
    
    return Input;
}

void GLShaderSetUniform(GLShader Shader, GLShaderUniformInfo *Uniform, size_t Count, const void *Data)
{
    CCAssertLog(Shader, "Shader must not be null");
    CCAssertLog(Uniform, "Uniform must not be null");
    CCAssertLog(Data, "Data must not be null");
    CCAssertLog(Count <= Uniform->count, "Count must not exceed bounds");
    
    const size_t BufferSize = GFXBufferFormatGetSize(Uniform->type) * Count;
    if (memcmp(Uniform->value, Data, BufferSize))
    {
        CC_GL_USE_PROGRAM(Shader->program);
        
        memcpy(Uniform->value, Data, BufferSize);
        
        if (GFXBufferFormatIsMatrix(Uniform->type))
        {
            CCAssertLog(GFXBufferFormatIsFloat(Uniform->type), "Matrix must be of float type");
            
            void (*MatrixUniforms[2][3][3])(GLint, GLsizei, GLboolean, const void *) = {
                {
                    {
                        (void(*)(GLint,GLsizei,GLboolean,const void*))glUniformMatrix2fv,
                        (void(*)(GLint,GLsizei,GLboolean,const void*))glUniformMatrix2x3fv,
                        (void(*)(GLint,GLsizei,GLboolean,const void*))glUniformMatrix2x4fv
                    },
                    {
                        (void(*)(GLint,GLsizei,GLboolean,const void*))glUniformMatrix3x2fv,
                        (void(*)(GLint,GLsizei,GLboolean,const void*))glUniformMatrix3fv,
                        (void(*)(GLint,GLsizei,GLboolean,const void*))glUniformMatrix3x4fv
                    },
                    {
                        (void(*)(GLint,GLsizei,GLboolean,const void*))glUniformMatrix4x2fv,
                        (void(*)(GLint,GLsizei,GLboolean,const void*))glUniformMatrix4x3fv,
                        (void(*)(GLint,GLsizei,GLboolean,const void*))glUniformMatrix4fv
                    }
                },
                {
                    {
                        (void(*)(GLint,GLsizei,GLboolean,const void*))glUniformMatrix2dv,
                        (void(*)(GLint,GLsizei,GLboolean,const void*))glUniformMatrix2x3dv,
                        (void(*)(GLint,GLsizei,GLboolean,const void*))glUniformMatrix2x4dv
                    },
                    {
                        (void(*)(GLint,GLsizei,GLboolean,const void*))glUniformMatrix3x2dv,
                        (void(*)(GLint,GLsizei,GLboolean,const void*))glUniformMatrix3dv,
                        (void(*)(GLint,GLsizei,GLboolean,const void*))glUniformMatrix3x4dv
                    },
                    {
                        (void(*)(GLint,GLsizei,GLboolean,const void*))glUniformMatrix4x2dv,
                        (void(*)(GLint,GLsizei,GLboolean,const void*))glUniformMatrix4x3dv,
                        (void(*)(GLint,GLsizei,GLboolean,const void*))glUniformMatrix4dv
                    }
                }
            };
            
            void (*(*MatrixUniform)[3])(GLint, GLsizei, GLboolean, const void *) = NULL;
            switch (GFXBufferFormatGetBitSize(Uniform->type))
            {
                case 32:
                    MatrixUniform = MatrixUniforms[0];
                    break;
                    
                case 64:
                    MatrixUniform = MatrixUniforms[1];
                    break;
                    
                default:
                    CCAssertLog(0, "Unsupported uniform type %d", Uniform->type);
                    break;
            }
            
            MatrixUniform[GFXBufferFormatGetMatrixM(Uniform->type)][GFXBufferFormatGetMatrixN(Uniform->type)](Uniform->location, (GLsizei)Count, GL_FALSE, Data);
        }
        
        else //vector or scalar
        {
            void (*VectorUniform)(GLint, GLsizei, const void *) = NULL;
            if (GFXBufferFormatIsInteger(Uniform->type))
            {
                void (*VectorIntegerUniforms[2][4])(GLint, GLsizei, const void *) = {
                    {
                        (void(*)(GLint,GLsizei,const void*))glUniform1iv,
                        (void(*)(GLint,GLsizei,const void*))glUniform2iv,
                        (void(*)(GLint,GLsizei,const void*))glUniform3iv,
                        (void(*)(GLint,GLsizei,const void*))glUniform4iv
                    },
                    {
                        (void(*)(GLint,GLsizei,const void*))glUniform1uiv,
                        (void(*)(GLint,GLsizei,const void*))glUniform2uiv,
                        (void(*)(GLint,GLsizei,const void*))glUniform3uiv,
                        (void(*)(GLint,GLsizei,const void*))glUniform4uiv
                    }
                };
                
                CCAssertLog(GFXBufferFormatGetBitSize(Uniform->type) == 32, "Unsupported uniform type %d", Uniform->type); //TODO: Support other sizes (GL_BOOL)
                
                VectorUniform = VectorIntegerUniforms[GFXBufferFormatIsUnsigned(Uniform->type)][GFXBufferFormatGetElementCount(Uniform->type) - 1];
            }
            
            else //float
            {
                void (*VectorFloatUniforms[2][4])(GLint, GLsizei, const void *) = {
                    {
                        (void(*)(GLint,GLsizei,const void*))glUniform1fv,
                        (void(*)(GLint,GLsizei,const void*))glUniform2fv,
                        (void(*)(GLint,GLsizei,const void*))glUniform3fv,
                        (void(*)(GLint,GLsizei,const void*))glUniform4fv
                    },
                    {
                        (void(*)(GLint,GLsizei,const void*))glUniform1dv,
                        (void(*)(GLint,GLsizei,const void*))glUniform2dv,
                        (void(*)(GLint,GLsizei,const void*))glUniform3dv,
                        (void(*)(GLint,GLsizei,const void*))glUniform4dv
                    }
                };
                
                void (**FloatUniform)(GLint, GLsizei, const void *) = NULL;
                switch (GFXBufferFormatGetBitSize(Uniform->type))
                {
                    case 32:
                        FloatUniform = VectorFloatUniforms[0];
                        break;
                        
                    case 64:
                        FloatUniform = VectorFloatUniforms[1];
                        break;
                        
                    default:
                        CCAssertLog(0, "Unsupported uniform type %d", Uniform->type);
                        break;
                }
                
                VectorUniform = FloatUniform[GFXBufferFormatGetElementCount(Uniform->type) - 1];
            }
            
            VectorUniform(Uniform->location, (GLsizei)Count, Data); CC_GL_CHECK();
        }
    }
}
