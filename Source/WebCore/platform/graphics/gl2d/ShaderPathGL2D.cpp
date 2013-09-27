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

#define FILL_PATH_SHADER_ARGUMENTS(argument) \
    argument(a, y1y2, y1y2) \
    argument(a, x1x2Cdx1dx2, x1x2Cdx1dx2)

DEFINE_SHADER(
    FillPathShader,
    FILL_PATH_SHADER_ARGUMENTS,
    PlatformContextGL2D::kVec4Position,
    // Vertex variables
    GL2D_PROGRAM(
        attribute vec4 a_x1x2Cdx1dx2;

        // To reduce the rounding issues of float16 variables,
        // the numbers are spearated for integer and fractional parts.
        // On the downside, this doubles the required arguments.
        varying vec4 v_y1y2;
        varying vec4 v_x1x2;
        varying vec2 v_dx1dx2;
    ),
    // Vertex shader
    GL2D_PROGRAM(
        float y = floor(a_position[2]);
        v_y1y2[0] = a_position[1] - y;
        v_y1y2[1] = floor(a_position[3]) - y;
        v_y1y2[2] = fract(a_position[2]);
        v_y1y2[3] = fract(a_position[3]);

        float x = floor(a_x1x2Cdx1dx2[0]);
        v_x1x2[0] = a_position[0] - x;
        v_x1x2[1] = floor(a_x1x2Cdx1dx2[1]) - x;
        v_x1x2[2] = fract(a_x1x2Cdx1dx2[0]);
        v_x1x2[3] = fract(a_x1x2Cdx1dx2[1]);

        v_dx1dx2[0] = a_x1x2Cdx1dx2[2];
        v_dx1dx2[1] = a_x1x2Cdx1dx2[3];
    ),
    // Fragment variables
    GL2D_PROGRAM(
        varying vec4 v_y1y2;
        varying vec4 v_x1x2;
        varying vec2 v_dx1dx2;
    ),
    // Fragment shader
    GL2D_PROGRAM(
        const float rounding = 1.0 / 32.0;

        float y = floor(v_y1y2[0]);
        float from = max(-y + v_y1y2[2], 0.0);
        float to = min(v_y1y2[1] - y + v_y1y2[3], 1.0) - from;

        vec2 x1x2 = (y + from) * (v_dx1dx2 * 16.0);

        float x = floor(v_x1x2[0]);
        x1x2[0] = (-x) + (x1x2[0] + v_x1x2[2]);
        x1x2[1] = (v_x1x2[1] - x) + (x1x2[1] + v_x1x2[3]);

        // Manual loop unrolling.
        float sum = (clamp(x1x2[1], 0.0, 1.0) - clamp(x1x2[0], 0.0, 1.0)) * ceil(to - (0.0 / 16.0 + rounding));

        vec4 resultColor = vec4(0.0, 0.0, 0.0, 1.0);
        vec2 last = x1x2 + v_dx1dx2 * 15.0;
        sum += (clamp(last[1], 0.0, 1.0) - clamp(last[0], 0.0, 1.0)) * ceil(to - (15.0 / 16.0 + rounding));
        if (sum < 2.0) {
            x1x2 += v_dx1dx2;

            sum += (clamp(x1x2[1], 0.0, 1.0) - clamp(x1x2[0], 0.0, 1.0)) * ceil(to - (1.0 / 16.0 + rounding));
            x1x2 += v_dx1dx2;

            sum += (clamp(x1x2[1], 0.0, 1.0) - clamp(x1x2[0], 0.0, 1.0)) * ceil(to - (2.0 / 16.0 + rounding));
            x1x2 += v_dx1dx2;

            sum += (clamp(x1x2[1], 0.0, 1.0) - clamp(x1x2[0], 0.0, 1.0)) * ceil(to - (3.0 / 16.0 + rounding));
            x1x2 += v_dx1dx2;

            sum += (clamp(x1x2[1], 0.0, 1.0) - clamp(x1x2[0], 0.0, 1.0)) * ceil(to - (4.0 / 16.0 + rounding));
            x1x2 += v_dx1dx2;

            sum += (clamp(x1x2[1], 0.0, 1.0) - clamp(x1x2[0], 0.0, 1.0)) * ceil(to - (5.0 / 16.0 + rounding));
            x1x2 += v_dx1dx2;

            sum += (clamp(x1x2[1], 0.0, 1.0) - clamp(x1x2[0], 0.0, 1.0)) * ceil(to - (6.0 / 16.0 + rounding));
            x1x2 += v_dx1dx2;

            sum += (clamp(x1x2[1], 0.0, 1.0) - clamp(x1x2[0], 0.0, 1.0)) * ceil(to - (7.0 / 16.0 + rounding));
            x1x2 += v_dx1dx2;

            sum += (clamp(x1x2[1], 0.0, 1.0) - clamp(x1x2[0], 0.0, 1.0)) * ceil(to - (8.0 / 16.0 + rounding));
            x1x2 += v_dx1dx2;

            sum += (clamp(x1x2[1], 0.0, 1.0) - clamp(x1x2[0], 0.0, 1.0)) * ceil(to - (9.0 / 16.0 + rounding));
            x1x2 += v_dx1dx2;

            sum += (clamp(x1x2[1], 0.0, 1.0) - clamp(x1x2[0], 0.0, 1.0)) * ceil(to - (10.0 / 16.0 + rounding));
            x1x2 += v_dx1dx2;

            sum += (clamp(x1x2[1], 0.0, 1.0) - clamp(x1x2[0], 0.0, 1.0)) * ceil(to - (11.0 / 16.0 + rounding));
            x1x2 += v_dx1dx2;

            sum += (clamp(x1x2[1], 0.0, 1.0) - clamp(x1x2[0], 0.0, 1.0)) * ceil(to - (12.0 / 16.0 + rounding));
            x1x2 += v_dx1dx2;

            sum += (clamp(x1x2[1], 0.0, 1.0) - clamp(x1x2[0], 0.0, 1.0)) * ceil(to - (13.0 / 16.0 + rounding));
            x1x2 += v_dx1dx2;

            sum += (clamp(x1x2[1], 0.0, 1.0) - clamp(x1x2[0], 0.0, 1.0)) * ceil(to - (14.0 / 16.0 + rounding));

            resultColor = vec4(0.0, 0.0, 0.0, sum * (1.0 / 16.0));
        }
    )
)

static GLuint g_fillPathShader[5];

static void setupAttributes(GLfloat* trapezoid, GLfloat* position, GLfloat* x1x2Cdx1dx2)
{
    GLfloat dx1 = (trapezoid[4] - trapezoid[1]) / (trapezoid[3] - trapezoid[0]);
    GLfloat dx2 = (trapezoid[5] - trapezoid[2]) / (trapezoid[3] - trapezoid[0]);
    GLfloat bottom = floor(trapezoid[0]);
    GLfloat top = ceil(trapezoid[3]);
    // The fraction is stored in a temporary variable.
    GLfloat temp, x1, x2;
    GLfloat bottomLeft, bottomRight, topLeft, topRight;

    temp = ceil(trapezoid[3]) - trapezoid[3];
    x1 = trapezoid[4] - temp * dx1;
    x2 = trapezoid[5] - temp * dx2;
    topLeft = floor(x1 - fabs(dx1));
    topRight = ceil(x2 + fabs(dx2));

    temp = trapezoid[0] - floor(trapezoid[0]);
    x1 = trapezoid[1] - temp * dx1;
    x2 = trapezoid[2] - temp * dx2;
    bottomLeft = floor(x1 - fabs(dx1));
    bottomRight = ceil(x2 + fabs(dx2));

    dx1 /= 16.0;
    dx2 /= 16.0;

    for (int i = 0; i < 4; i++) {
        // Absolute coordinates are transformed to the [-1,+1] space.
        switch (i) {
        case 0:
            *position++ = bottomLeft;
            *position++ = bottom;
            break;

        case 1:
            *position++ = topLeft;
            *position++ = top;
            break;

        case 2:
            *position++ = bottomRight;
            *position++ = bottom;
            break;

        case 3:
            *position++ = topRight;
            *position++ = top;
            break;
        }

        *position++ = trapezoid[0];
        *position++ = trapezoid[3];

        *x1x2Cdx1dx2++ = x1;
        *x1x2Cdx1dx2++ = x2;
        *x1x2Cdx1dx2++ = dx1;
        *x1x2Cdx1dx2++ = dx2;
    }
}

PassOwnPtr<NativeImageGL2D> PlatformContextGL2D::createPathAlphaTexture(GL2D::TrapezoidList* trapezoidList)
{
    OwnPtr<NativeImageGL2D> pathAlphaTexture = NativeImageGL2D::create(trapezoidList->boundingBox().size());

    pathAlphaTexture->bindFbo();

    glClear(GL_COLOR_BUFFER_BIT);

    const char maxNumber = 64;

    if (!compileAndUseShader(g_fillPathShader, 1, &FillPathShader::s_program))
        return 0;

    glBlendFunc(GL_ONE, GL_ONE);
    GLuint offset = g_fillPathShader[1];

    GLfloat position[maxNumber * 16];
    GLfloat x1x2Cdx1dx2[maxNumber * 16];
    GLubyte indicies[maxNumber * 6];

    int index = 0;
    int current = 0;
    while (current < maxNumber * 6) {
        indicies[current++] = index++;
        indicies[current++] = index;
        indicies[current++] = index + 1;
        indicies[current++] = index++;
        indicies[current++] = index++;
        indicies[current++] = index++;
    }

    GL2D::TrapezoidList::Trapezoid* trapezoids = trapezoidList->trapezoids();
    size_t trapezoidCount = trapezoidList->trapezoidCount();

    float* points;
    int trapezoidIndex = 0;
    for (size_t i = 0; i < trapezoidCount; i++) {
        points = trapezoids[i].points();

        setupAttributes(points, position + trapezoidIndex * 16, x1x2Cdx1dx2 + trapezoidIndex * 16);

        trapezoidIndex++;
        if (trapezoidIndex % maxNumber == 0) {
            glVertexAttribPointer(g_fillPathShader[offset + FillPathShader::aPosition], 4, GL_FLOAT, GL_FALSE, 0, position);
            glVertexAttribPointer(g_fillPathShader[offset + FillPathShader::ax1x2Cdx1dx2], 4, GL_FLOAT, GL_FALSE, 0, x1x2Cdx1dx2);
            glDrawElements(GL_TRIANGLES, maxNumber * 6, GL_UNSIGNED_BYTE, indicies);
            trapezoidIndex = 0;
        }
    }

    if (trapezoidIndex) {
        glVertexAttribPointer(g_fillPathShader[offset + FillPathShader::aPosition], 4, GL_FLOAT, GL_FALSE, 0, position);
        glVertexAttribPointer(g_fillPathShader[offset + FillPathShader::ax1x2Cdx1dx2], 4, GL_FLOAT, GL_FALSE, 0, x1x2Cdx1dx2);
        glDrawElements(GL_TRIANGLES, trapezoidIndex * 6, GL_UNSIGNED_BYTE, indicies);
    }

    return pathAlphaTexture.release();
}

#define FILL_COLOR_SHADER_ARGUMENTS(argument) \
    argument(u, texture, Texture) \
    argument(u, color, Color)

DEFINE_SHADER(
    FillColorShader,
    FILL_COLOR_SHADER_ARGUMENTS,
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
        uniform vec4 u_color;
        varying vec2 v_texturePosition;
    ),
    // Fragment shader
    GL2D_PROGRAM(
        vec4 resultColor = u_color * texture2D(u_texture, v_texturePosition).a;
    )
)

static GLuint g_fillColorShader[5];

void PlatformContextGL2D::fillPath(const GL2D::PathData* path)
{
    OwnPtr<GL2D::TrapezoidList> trapezoidList = path->fillTrapezoidList(m_state.boundingClipRect, transform());

    const IntSize size = trapezoidList->boundingBox().size();

    if (!size.width() || !size.height())
        return;

    PipelineData pipelineData;
    OwnPtr<NativeImageGL2D> pathAlphaTexture = createPathAlphaTexture(trapezoidList.get());
    if (!pathAlphaTexture)
        return;

    pipelineData.u.pathData.pathAlphaTexture = pathAlphaTexture.get();
    pipelineData.subimageRect = trapezoidList->boundingBox();
    paint(PaintPath, pipelineData);
}

void PlatformContextGL2D::pathSetShaderArgument(PipelineData& pipelineData)
{
    if (!compileAndUseShader(g_fillColorShader, 1, &FillColorShader::s_program))
        return;

    GLfloat left, bottom, right, top;
    absolutePosition(pipelineData, left, bottom, right, top);
    SET_POSITION16(left, bottom, 0, 0, right, bottom, 1, 0, left, top, 0, 1, right, top, 1, 1);

    int offset = g_fillColorShader[1];

    setColorUniform(g_fillColorShader[offset + FillColorShader::uColor], m_state.fillColor);

    addTexture(pipelineData, g_fillColorShader[offset + FillColorShader::uTexture], pipelineData.u.pathData.pathAlphaTexture);

    glVertexAttribPointer(g_fillColorShader[offset + FillColorShader::aPosition], 4, GL_FLOAT, GL_FALSE, 0, pipelineData.position);
}

} // namespace WebCore
