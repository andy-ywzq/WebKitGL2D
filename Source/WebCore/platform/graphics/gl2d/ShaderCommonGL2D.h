/*
 * Copyright (C) 2013 University of Szeged
 * Copyright (C) 2013 Kristof Kosztyo
 * Copyright (C) 2013 Szilárd Ledán
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY UNIVERSITY OF SZEGED ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL UNIVERSITY OF SZEGED OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ShaderCommonGL2D_h
#define ShaderCommonGL2D_h

// Global macros used by various shaders.
#define GL2D_PROGRAM_STR(...)  #__VA_ARGS__
#define GL2D_PROGRAM(...) GL2D_PROGRAM_STR(__VA_ARGS__)
#define GL2D_EMPTY_PROGRAM 0

#define SET_POSITION8(a0, a1, a2, a3, a4, a5, a6, a7) \
    pipelineData.position[0] = (a0); \
    pipelineData.position[1] = (a1); \
    pipelineData.position[2] = (a2); \
    pipelineData.position[3] = (a3); \
    pipelineData.position[4] = (a4); \
    pipelineData.position[5] = (a5); \
    pipelineData.position[6] = (a6); \
    pipelineData.position[7] = (a7);

#define SET_POSITION16(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15) \
    pipelineData.position[0] = (a0); \
    pipelineData.position[1] = (a1); \
    pipelineData.position[2] = (a2); \
    pipelineData.position[3] = (a3); \
    pipelineData.position[4] = (a4); \
    pipelineData.position[5] = (a5); \
    pipelineData.position[6] = (a6); \
    pipelineData.position[7] = (a7); \
    pipelineData.position[8] = (a8); \
    pipelineData.position[9] = (a9); \
    pipelineData.position[10] = (a10); \
    pipelineData.position[11] = (a11); \
    pipelineData.position[12] = (a12); \
    pipelineData.position[13] = (a13); \
    pipelineData.position[14] = (a14); \
    pipelineData.position[15] = (a15);

// Can only be used in PlatformContexGL2D.
#define VIEWPORT_SIZE 2048

// The following macros are helpers:
#define UNDERSCORE_PREFIX(name) _ ## name
#define SHADER_ARGUMENT_NAME_NO_EXPAND(type, name) type ## name
#define SHADER_ARGUMENT_NAME(type, name) SHADER_ARGUMENT_NAME_NO_EXPAND(type, name)

#define SHADER_NAME_STRING_NO_EXPAND(name) #name
#define SHADER_NAME_STRING(name) SHADER_NAME_STRING_NO_EXPAND(name)

// This macro expects two identical names: one with capital, and the other with
// non-capital starting character. All other characters must be the same.
// example: argument(u, texture, Texture)
#define ARGUMENT_LIST_ENUM(type, name, Name) type ## Name,
#define ARGUMENT_LIST_STRING(type, name, Name) SHADER_NAME_STRING(SHADER_ARGUMENT_NAME(type, UNDERSCORE_PREFIX(name))),

// This is the only macro used elsewhere:
#define DEFINE_SHADER(className, arguments, options, vertexVariables, vertexShader, fragmentVariables, fragmentShader) \
     class className { \
     public: \
         enum { \
             aPosition, \
             arguments(ARGUMENT_LIST_ENUM) \
         }; \
         static const GLchar* s_argumentList[]; \
         static PlatformContextGL2D::ShaderProgram s_program; \
     }; \
     \
     const GLchar* className::s_argumentList[] = { \
         "a_position", \
         arguments(ARGUMENT_LIST_STRING) \
         0 \
     }; \
     \
     PlatformContextGL2D::ShaderProgram className::s_program = { \
         vertexVariables, \
         vertexShader, \
         fragmentVariables, \
         fragmentShader, \
         className::s_argumentList, \
         options \
     };

// Shader coding guidelines
//    First shader is the source shader.
//       * Can specify kVec4Position.
//       * Fragment shader must specify "vec4 resultColor".
//       * No need variable prefixing.
//    Other shaders
//       * Must prefix all their variables with the type of the shader.

namespace WebCore{

} //namespace WebCore

#endif
