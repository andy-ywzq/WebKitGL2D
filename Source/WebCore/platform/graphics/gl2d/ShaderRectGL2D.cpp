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

namespace WebCore {

using GL2D::min;
using GL2D::max;

#define INTEGER_RECT_SHADER_ARGUMENTS(argument) \
    argument(u, color, Color)

DEFINE_SHADER(
    IntegerRectShader,
    INTEGER_RECT_SHADER_ARGUMENTS,
    PlatformContextGL2D::kNoOptions,
    // Vertex variables
    GL2D_EMPTY_PROGRAM,
    // Vertex shader
    GL2D_EMPTY_PROGRAM,
    // Fragment variables
    GL2D_PROGRAM(
        uniform vec4 u_color;
    ),
    // Fragment shader
    GL2D_PROGRAM(
        vec4 resultColor = u_color;
    )
)

static GLuint g_integerRectShader[4];

inline bool PlatformContextGL2D::fillRectFastPath(const FloatRect& rect, const Color& color)
{
    if (transform().type() != GL2D::AffineTransform::Move)
        return false;

    GLfloat left = rect.x() + transform().transform().m_matrix[4];
    GLfloat bottom = rect.y() + transform().transform().m_matrix[5];
    GLfloat right = left + rect.width();
    GLfloat top = bottom + rect.height();

    if (!isInteger(left) || !isInteger(bottom) || !isInteger(right) || !isInteger(top))
        return false;

    left = max(left, boundingClipRect().left());
    bottom = max(bottom, boundingClipRect().bottom());
    right = min(right, boundingClipRect().right());
    top = min(top, boundingClipRect().top());
    if (left >= right || bottom >= top)
        return true;

    PipelineData pipelineData;

    pipelineData.u.rectData.color = color.rgb();
    pipelineData.subimageRect = IntRect(left, bottom, right - left, top - bottom);

    paint(PaintRect, pipelineData);

    return true;
}

void PlatformContextGL2D::rectSetShaderArgument(PipelineData& pipelineData)
{
    if (!compileAndUseShader(g_integerRectShader, 1, &IntegerRectShader::s_program))
        return;

    GLfloat left, bottom, right, top;
    absolutePosition(pipelineData, left, bottom, right, top);

    GLuint offset = g_integerRectShader[1];
    setColorUniform(g_integerRectShader[offset + IntegerRectShader::uColor], pipelineData.u.rectData.color);

    SET_POSITION8(left, bottom, right, bottom, left, top, right, top);
    glVertexAttribPointer(g_integerRectShader[offset + IntegerRectShader::aPosition], 2, GL_FLOAT, GL_FALSE, 0, pipelineData.position);
}

void PlatformContextGL2D::fillRect(const FloatRect& rect, const Color& color)
{
    if (fillRectFastPath(rect, color))
        return;

    GL2D::PathData path;
    path.addRect(rect);

    RGBA32 fillColor = m_state.fillColor;
    m_state.fillColor = color.rgb();
    fillPath(&path);
    m_state.fillColor = fillColor;
}

} // namespace WebCore
