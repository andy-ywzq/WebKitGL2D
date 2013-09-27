/*
 * Copyright (C) 2013 University of Szeged
 * Copyright (C) 2013 Zoltan Herczeg
 * Copyright (C) 2013 Kristof Kosztyo
 * Copyright (C) 2013 Szilard Ledan
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

#include "Color.h"
#include "GLDefs.h"
#include "GLPlatformContext.h"
#include "GraphicsContext3D.h"
#include "ShaderCommonGL2D.h"

namespace WebCore {

using GL2D::min;
using GL2D::max;

GLPlatformContext* PlatformContextGL2D::s_offScreenContext = 0;
GLPlatformSurface* PlatformContextGL2D::s_offScreenSurface = 0;
GLubyte PlatformContextGL2D::s_rectIndicies[4] = { 0, 1, 2, 3 };

PlatformContextGL2D::PlatformContextGL2D(NativeImageGL2D* targetBuffer)
    : m_targetTexture(targetBuffer)
{
    createGLContextIfNeed();
    m_state.boundingClipRect.setDimension(targetBuffer->width(), targetBuffer->height());
    m_state.fillColor = 0x000000ff;
    m_state.strokeColor = 0x000000ff;
    m_state.compositeOperator = CompositeSourceOver;
    m_state.blendMode = BlendModeNormal;
}

void PlatformContextGL2D::createGLContext()
{
    s_offScreenContext = GLPlatformContext::createContext(GraphicsContext3D::RenderOffscreen).leakPtr();
    s_offScreenSurface = GLPlatformSurface::createOffScreenSurface().leakPtr();
    s_offScreenContext->initialize(s_offScreenSurface);
    glViewport(0, 0, VIEWPORT_SIZE, VIEWPORT_SIZE);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_BLEND);
}

void PlatformContextGL2D::save()
{
    m_stack.append(m_state);
}

void PlatformContextGL2D::restore()
{
    m_state = m_stack.last();
    m_stack.removeLast();
}

void PlatformContextGL2D::setStrokeColor(const Color& color)
{
    m_state.strokeColor = color.rgb();
}

void PlatformContextGL2D::setFillColor(const Color& color)
{
    m_state.fillColor = color.rgb();
}

void PlatformContextGL2D::setCompositeOperation(CompositeOperator op, BlendMode blendMode)
{
    m_state.compositeOperator = op;
    m_state.blendMode = blendMode;
}

void PlatformContextGL2D::translate(float x, float y)
{
    m_state.transform.translate(x, y);
}

void PlatformContextGL2D::scale(float sx, float sy)
{
    m_state.transform.scale(sx, sy);
}

void PlatformContextGL2D::rotate(float angle)
{
    m_state.transform.rotate(angle);
}

void PlatformContextGL2D::clip(const FloatRect& rect)
{
    FloatRect transformedRect(rect);
    transformedRect.setLocation(m_state.transform.apply(rect.location()));
    m_state.boundingClipRect.intersect(enclosingIntRect(transformedRect));
}

bool PlatformContextGL2D::compileShaderInternal(GLuint* compiledShader, int count, const ShaderProgram* programList)
{
    GLuint shaderProgram = 0;
    GLuint vertexShader = 0;
    GLuint fragmentShader = 0;
    GLuint* offset;
    GLint intValue;

    ASSERT(!compiledShader[0] && count >= 1);
    const GLchar* shaderSource[16];
    const GLchar** current = shaderSource;

    if (programList[0].options & PlatformContextGL2D::kVec4Position)
        *current++ = GL2D_PROGRAM( attribute vec4 a_position; );
    else
        *current++ = GL2D_PROGRAM( attribute vec2 a_position; );

    for (int i = 0; i < count; i++) {
        if (programList[i].vertexVariables && programList[i].vertexVariables[0])
            *current++ = programList[i].vertexVariables;
    }
    *current++ = GL2D_PROGRAM( void main(void) { );
    for (int i = 0; i < count; i++) {
        if (programList[i].vertexShader && programList[i].vertexShader[0])
            *current++ = programList[i].vertexShader;
    }
    *current++ = GL2D_PROGRAM( gl_Position = vec4(a_position.xy * (2.0 / VIEWPORT_SIZE.0) - 1.0, 0.0, 1.0); } );

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, current - shaderSource, shaderSource, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &intValue);
    if (intValue != GL_TRUE)
        goto error;

    current = shaderSource;
    *current++ = "#ifdef GL_ES\nprecision mediump float;\n#endif\n";
    for (int i = 0; i < count; i++) {
        if (programList[i].fragmentVariables && programList[i].fragmentVariables[0])
            *current++ = programList[i].fragmentVariables;
    }
    *current++ = GL2D_PROGRAM( void main(void) { );
    for (int i = 0; i < count; i++) {
        if (programList[i].fragmentShader && programList[i].fragmentShader[0])
            *current++ = programList[i].fragmentShader;
    }
    *current++ = GL2D_PROGRAM( gl_FragColor = resultColor; } );

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, current - shaderSource, shaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &intValue);
    if (intValue != GL_TRUE)
        goto error;

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &intValue);
    if (intValue != GL_TRUE)
        goto error;

    // According to the specification, the shaders are kept
    // around until the program object is freed (reference counted).
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(shaderProgram);
    offset = compiledShader + 1 + count;
    for (int i = 0; i < count; i++) {
        const GLchar** arguments = programList[i].variables;
        compiledShader[i + 1] = offset - compiledShader;
        while (*arguments) {
            const GLchar* name = *arguments;
            ASSERT((name[0] == 'u' || name[0] == 'a') && name[1] == '_');
            if (name[0] == 'u') {
                *offset++ = glGetUniformLocation(shaderProgram, name);
            } else {
                *offset = glGetAttribLocation(shaderProgram, name);
                glEnableVertexAttribArray(*offset);
                offset++;
            }
            arguments++;
        }
    }

    compiledShader[0] = shaderProgram;
    return true;

error:
    if (vertexShader)
        glDeleteShader(vertexShader);
    if (fragmentShader)
        glDeleteShader(fragmentShader);
    if (shaderProgram)
        glDeleteProgram(shaderProgram);
    return false;
}

} // namespace WebCore
