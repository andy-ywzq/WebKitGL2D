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

#include "config.h"
#include "PlatformContextGL2D.h"

#include "PathGL2D.h"
#include "ShaderCommonGL2D.h"
#include "TrapezoidListGL2D.h"

namespace WebCore {

#define COPY_SHADER_ARGUMENTS(argument) \
    argument(u, texture, Texture) \

DEFINE_SHADER(
    CopyShader,
    COPY_SHADER_ARGUMENTS,
    PlatformContextGL2D::kVec4Position,
    // Vertex variables
    GL2D_PROGRAM(
        varying vec2 v_texturePosition;
    ),
    // Vertex shader
    GL2D_PROGRAM(
        v_texturePosition = a_position.zw;
    ),
    // Fragment variables
    GL2D_PROGRAM(
        uniform sampler2D u_texture;
        varying vec2 v_texturePosition;
    ),
    // Fragment shader
    GL2D_PROGRAM(
        vec4 resultColor = texture2D(u_texture, v_texturePosition);
    )
)

static GLuint g_CopyShader[4];

void PlatformContextGL2D::paint(PaintType type, PipelineData& pipelineData)
{
    pipelineData.subimageRequired = false;
    pipelineData.textureIndex = 0;

    // Check whether subimageRequired needs to be set.

    if (pipelineData.subimageRequired) {
        pipelineData.subimage = NativeImageGL2D::create(pipelineData.subimageRect.size());
        pipelineData.subimage->bindFbo();

        glClear(GL_COLOR_BUFFER_BIT);
        glBlendFunc(GL_ONE, GL_ZERO);

        switch (type) {
        case PaintRect:
            rectSetShaderArgument(pipelineData);
            break;
        case PaintPath:
            pathSetShaderArgument(pipelineData);
            break;
        case PaintImage:
            imageSetShaderArgument(pipelineData);
            break;
        case PaintTransformedImage:
            transformedImageSetShaderArgument(pipelineData);
            break;
        default:
            ASSERT_NOT_REACHED();
            break;
        }
        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, s_rectIndicies);
    }

    switch (m_state.compositeOperator) {
    case CompositeCopy:
        // FIXME Fall through at the moment
    case CompositeSourceOver:
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        break;
    case CompositeSourceIn:
        glBlendFunc(GL_DST_ALPHA, GL_ZERO);
        break;
    case CompositeSourceOut:
        glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ZERO);
        break;
    case CompositeSourceAtop:
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;
    case CompositeDestinationOver:
        glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
        break;
    case CompositeDestinationIn:
        glBlendFunc(GL_ZERO, GL_SRC_ALPHA);
        break;
    case CompositeDestinationOut:
        glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
        break;
    case CompositeDestinationAtop:
        glBlendFuncSeparate(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_SRC_ALPHA);
        break;
    default:
        ASSERT_NOT_REACHED();
        break;
    }

    if (!pipelineData.subimageRequired) {
        targetTexture()->bindFbo();

        switch (type) {
        case PaintRect:
            rectSetShaderArgument(pipelineData);
            break;
        case PaintPath:
            pathSetShaderArgument(pipelineData);
            break;
        case PaintImage:
            imageSetShaderArgument(pipelineData);
            break;
        case PaintTransformedImage:
            transformedImageSetShaderArgument(pipelineData);
            break;
        default:
            ASSERT_NOT_REACHED();
            break;
        }
        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, s_rectIndicies);
        return;
    }

    if (!compileAndUseShader(g_CopyShader, 1, &CopyShader::s_program))
        return;

    GLuint offset = g_CopyShader[1];

    targetTexture()->bindFbo();
    glActiveTexture(GL_TEXTURE0);
    pipelineData.subimage->bindTexture();
    glUniform1i(g_CopyShader[offset + CopyShader::uTexture], 0);

    GLfloat position[4 * 4] = {
        pipelineData.subimageRect.x(), pipelineData.subimageRect.y(), 0, 0,
        pipelineData.subimageRect.maxX(), pipelineData.subimageRect.y(), 1, 0,
        pipelineData.subimageRect.x(), pipelineData.subimageRect.maxY(), 0, 1,
        pipelineData.subimageRect.maxX(), pipelineData.subimageRect.maxY(), 1, 1
    };
    glVertexAttribPointer(g_CopyShader[offset + CopyShader::aPosition], 4, GL_FLOAT, GL_FALSE, 0, position);

    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, s_rectIndicies);

    pipelineData.subimage.clear();
}

} // namespace WebCore
