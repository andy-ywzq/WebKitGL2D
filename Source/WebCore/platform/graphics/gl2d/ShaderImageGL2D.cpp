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

#include "ShaderCommonGL2D.h"

namespace WebCore {

using GL2D::min;
using GL2D::max;

#define IMAGE_SHADER_ARGUMENTS(argument) \
    argument(u, texture, Texture)

DEFINE_SHADER(
    ImageShader,
    IMAGE_SHADER_ARGUMENTS,
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

static GLuint g_imageShader[5];

inline void PlatformContextGL2D::copyImageFastPath(GLfloat left, GLfloat bottom, GLfloat right, GLfloat top, GLfloat v0X, GLfloat v1Y,
    NativeImageGL2D* sourceTexture, const FloatRect& srcRect, CompositeOperator op, BlendMode blendMode)
{
    if (left >= boundingClipRect().right() || right <= boundingClipRect().left()
            || bottom >= boundingClipRect().top() || top <= boundingClipRect().bottom())
        return;

    GLfloat width = sourceTexture->width();
    GLfloat height = sourceTexture->height();
    GLfloat textureLeft = srcRect.x() / width;
    GLfloat textureBottom = srcRect.x() / width;
    GLfloat textureRight = (srcRect.x() + srcRect.width()) / width;
    GLfloat textureTop = (srcRect.y() + srcRect.height()) / height;
    width = textureRight - textureLeft;
    height = textureTop - textureBottom;

    // Clipping.
    GLfloat limit = boundingClipRect().left();
    if (left < limit) {
        textureLeft += ((limit - left) / v0X) * width;
        left = limit;
    }
    limit = boundingClipRect().bottom();
    if (bottom < limit) {
        textureBottom += ((limit - bottom) / v1Y) * height;
        bottom = limit;
    }
    limit = boundingClipRect().right();
    if (right > limit) {
        textureRight -= ((right - limit) / v0X) * width;
        right = limit;
    }
    limit = boundingClipRect().top();
    if (top > limit) {
        textureTop -= ((top - limit) / v1Y) * height;
        top = limit;
    }
    if (left >= right || bottom >= top)
        return;

    CompositeOperator previousCompositeOperator = m_state.compositeOperator;
    BlendMode previousBlendMode = m_state.blendMode;
    if (op != previousCompositeOperator || blendMode != previousBlendMode)
        setCompositeOperation(op, blendMode);

    PipelineData pipelineData;

    ImageData& imageData = pipelineData.u.imageData;
    imageData.sourceTexture = sourceTexture;
    imageData.textureLeft = textureLeft;
    imageData.textureBottom = textureBottom;
    imageData.textureRight = textureRight;
    imageData.textureTop = textureTop;
    pipelineData.subimageRect = IntRect(left, bottom, right - left, top - bottom);
    paint(PaintImage, pipelineData);

    if (op != previousCompositeOperator || blendMode != previousBlendMode)
        setCompositeOperation(previousCompositeOperator, previousBlendMode);
}

void PlatformContextGL2D::imageSetShaderArgument(PipelineData& pipelineData)
{
    if (!compileAndUseShader(g_imageShader, 1, &ImageShader::s_program))
        return;

    GLfloat left, bottom, right, top;
    absolutePosition(pipelineData, left, bottom, right, top);
    GLuint offset = g_imageShader[1];
    ImageData& imageData = pipelineData.u.imageData;

    addTexture(pipelineData, g_imageShader[offset + ImageShader::uTexture], imageData.sourceTexture);

    SET_POSITION16(left, bottom, imageData.textureLeft, imageData.textureBottom,
                   right, bottom, imageData.textureRight, imageData.textureBottom,
                   left, top, imageData.textureLeft, imageData.textureTop,
                   right, top, imageData.textureRight, imageData.textureTop);
    glVertexAttribPointer(g_imageShader[offset + ImageShader::aPosition], 4, GL_FLOAT, GL_FALSE, 0, pipelineData.position);
}

#define TRANSFORMED_IMAGE_SHADER_ARGUMENTS(argument) \
    argument(u, texture, Texture) \
    argument(u, delta, Delta) \
    argument(u, texturePosition, TexturePosition)

DEFINE_SHADER(
    TransformedImageShader,
    TRANSFORMED_IMAGE_SHADER_ARGUMENTS,
    PlatformContextGL2D::kVec4Position,
    // Vertex variables
    GL2D_PROGRAM(
        varying vec2 v_relativePosition;
    ),
    // Vertex shader
    GL2D_PROGRAM(
        v_relativePosition = a_position.zw;
    ),
    // Fragment variables
    GL2D_PROGRAM(
        uniform sampler2D u_texture;
        uniform vec4 u_delta;
        uniform vec4 u_texturePosition;
        varying vec2 v_relativePosition;
    ),
    // Fragment shader
    GL2D_PROGRAM(
        float s = v_relativePosition[0];
        float t = v_relativePosition[1];

        // Computation order (starting from 0):
        // 4 1 7
        // 3 0 6
        // 5 2 8

        vec4 temporaryColor = vec4(0.0, 0.0, 0.0, 0.0);
        if (s == clamp(s, 0.0, 1.0) && t == clamp(t, 0.0, 1.0)) {
            temporaryColor = texture2D(u_texture, vec2(u_texturePosition[0] + s * u_texturePosition[2], u_texturePosition[1] + t * u_texturePosition[3]));
            temporaryColor.rgb *= temporaryColor.a;
        }
        vec4 resultColor = temporaryColor;

        float temporaryS = s + u_delta[0];
        float temporaryT = t + u_delta[2];
        temporaryColor = vec4(0.0, 0.0, 0.0, 0.0);
        if (temporaryS == clamp(temporaryS, 0.0, 1.0) && temporaryT == clamp(temporaryT, 0.0, 1.0)) {
            temporaryColor = texture2D(u_texture, vec2(u_texturePosition[0] + temporaryS * u_texturePosition[2], u_texturePosition[1] + temporaryT * u_texturePosition[3]));
            temporaryColor.rgb *= temporaryColor.a;
        }
        resultColor += temporaryColor;

        temporaryS = s - u_delta[0];
        temporaryT = t - u_delta[2];
        temporaryColor = vec4(0.0, 0.0, 0.0, 0.0);
        if (temporaryS == clamp(temporaryS, 0.0, 1.0) && temporaryT == clamp(temporaryT, 0.0, 1.0)) {
            temporaryColor = texture2D(u_texture, vec2(u_texturePosition[0] + temporaryS * u_texturePosition[2], u_texturePosition[1] + temporaryT * u_texturePosition[3]));
            temporaryColor.rgb *= temporaryColor.a;
        }
        resultColor += temporaryColor;

        temporaryS = s + u_delta[1];
        temporaryT = t + u_delta[3];
        temporaryColor = vec4(0.0, 0.0, 0.0, 0.0);
        if (temporaryS == clamp(temporaryS, 0.0, 1.0) && temporaryT == clamp(temporaryT, 0.0, 1.0)) {
            temporaryColor = texture2D(u_texture, vec2(u_texturePosition[0] + temporaryS * u_texturePosition[2], u_texturePosition[1] + temporaryT * u_texturePosition[3]));
            temporaryColor.rgb *= temporaryColor.a;
        }
        resultColor += temporaryColor;

        float temporaryS2 = temporaryS + u_delta[0];
        float temporaryT2 = temporaryT + u_delta[2];
        temporaryColor = vec4(0.0, 0.0, 0.0, 0.0);
        if (temporaryS == clamp(temporaryS, 0.0, 1.0) && temporaryT == clamp(temporaryT, 0.0, 1.0)) {
            temporaryColor = texture2D(u_texture, vec2(u_texturePosition[0] + temporaryS * u_texturePosition[2], u_texturePosition[1] + temporaryT * u_texturePosition[3]));
            temporaryColor.rgb *= temporaryColor.a;
        }
        resultColor += temporaryColor;

        temporaryS2 = temporaryS - u_delta[0];
        temporaryT2 = temporaryT - u_delta[2];
        temporaryColor = vec4(0.0, 0.0, 0.0, 0.0);
        if (temporaryS == clamp(temporaryS, 0.0, 1.0) && temporaryT == clamp(temporaryT, 0.0, 1.0)) {
            temporaryColor = texture2D(u_texture, vec2(u_texturePosition[0] + temporaryS * u_texturePosition[2], u_texturePosition[1] + temporaryT * u_texturePosition[3]));
            temporaryColor.rgb *= temporaryColor.a;
        }
        resultColor += temporaryColor;

        temporaryS = s - u_delta[1];
        temporaryT = t - u_delta[3];
        temporaryColor = vec4(0.0, 0.0, 0.0, 0.0);
        if (temporaryS == clamp(temporaryS, 0.0, 1.0) && temporaryT == clamp(temporaryT, 0.0, 1.0)) {
            temporaryColor = texture2D(u_texture, vec2(u_texturePosition[0] + temporaryS * u_texturePosition[2], u_texturePosition[1] + temporaryT * u_texturePosition[3]));
            temporaryColor.rgb *= temporaryColor.a;
        }
        resultColor += temporaryColor;

        temporaryS2 = temporaryS + u_delta[0];
        temporaryT2 = temporaryT + u_delta[2];
        temporaryColor = vec4(0.0, 0.0, 0.0, 0.0);
        if (temporaryS == clamp(temporaryS, 0.0, 1.0) && temporaryT == clamp(temporaryT, 0.0, 1.0)) {
            temporaryColor = texture2D(u_texture, vec2(u_texturePosition[0] + temporaryS * u_texturePosition[2], u_texturePosition[1] + temporaryT * u_texturePosition[3]));
            temporaryColor.rgb *= temporaryColor.a;
        }
        resultColor += temporaryColor;

        temporaryS2 = temporaryS - u_delta[0];
        temporaryT2 = temporaryT - u_delta[2];
        temporaryColor = vec4(0.0, 0.0, 0.0, 0.0);
        if (temporaryS == clamp(temporaryS, 0.0, 1.0) && temporaryT == clamp(temporaryT, 0.0, 1.0)) {
            temporaryColor = texture2D(u_texture, vec2(u_texturePosition[0] + temporaryS * u_texturePosition[2], u_texturePosition[1] + temporaryT * u_texturePosition[3]));
            temporaryColor.rgb *= temporaryColor.a;
        }
        resultColor += temporaryColor;

        resultColor = (resultColor.a > 0.0) ? vec4(resultColor.rgb / resultColor.a, resultColor.a * (1.0 / 9.0)) : vec4(0.0, 0.0, 0.0, 0.0);
    )
)

static GLuint g_transformedImageShader[7];

void PlatformContextGL2D::copyImage(const FloatRect& destRect, NativeImageGL2D* sourceTexture, const FloatRect& srcRect, CompositeOperator op, BlendMode blendMode)
{
    FloatPoint coords[4];

    coords[0] = transform().apply(destRect.location());
    FloatPoint point(destRect.x() + destRect.width(), destRect.y());
    coords[1] = transform().apply(point);
    point.setY(destRect.y() + destRect.height());
    coords[3] = transform().apply(point);
    point.setX(destRect.x());
    coords[2] = transform().apply(point);

    GLfloat v0X = coords[1].x() - coords[0].x();
    GLfloat v0Y = coords[1].y() - coords[0].y();
    GLfloat v1X = coords[2].x() - coords[0].x();
    GLfloat v1Y = coords[2].y() - coords[0].y();

    GLfloat discriminant = v1X * v0Y - v1Y * v0X;
    // Parallel or zero length vectors.
    if (!discriminant)
        return;

    if (v0X > 0 && !v0Y && !v1X && v1Y > 0
            && isInteger(coords[0].x()) && isInteger(coords[0].y()) && isInteger(v0X) && isInteger(v1Y)
            && v0X * 1.25 >= srcRect.width() && v0X * 0.75 <= srcRect.width()
            && v1Y * 1.25 >= srcRect.height() && v1Y * 0.75 <= srcRect.height()) {
        copyImageFastPath(coords[0].x(), coords[0].y(), coords[1].x(), coords[2].y(),
            v0X, v1Y, sourceTexture, srcRect, op, blendMode);
        return;
    }

    GLfloat minX = min(coords[0].x(), coords[1].x());
    minX = min(minX, coords[2].x());
    minX = floor(min(minX, coords[3].x()));
    GLfloat maxX = max(coords[0].x(), coords[1].x());
    maxX = max(maxX, coords[2].x());
    maxX = ceil(max(maxX, coords[3].x()));

    GLfloat minY = min(coords[0].y(), coords[1].y());
    minY = min(minY, coords[2].y());
    minY = floor(min(minY, coords[3].y()));
    GLfloat maxY = max(coords[0].y(), coords[1].y());
    maxY = max(maxY, coords[2].y());
    maxY = ceil(max(maxY, coords[3].y()));

    if (minX >= boundingClipRect().right() || maxX <= boundingClipRect().left()
            || minY >= boundingClipRect().top() || maxY <= boundingClipRect().bottom())
        return;

    minX = max(minX, boundingClipRect().left());
    maxX = min(maxX, boundingClipRect().right());
    minY = max(minY, boundingClipRect().bottom());
    maxY = min(maxY, boundingClipRect().top());

    CompositeOperator previousCompositeOperator = m_state.compositeOperator;
    BlendMode previousBlendMode = m_state.blendMode;
    if (op != previousCompositeOperator || blendMode != previousBlendMode)
        setCompositeOperation(op, blendMode);

    PipelineData pipelineData;

    pipelineData.subimageRect = IntRect(minX, minY, maxX - minX, maxY - minY);

    minX -= coords[0].x();
    maxX -= coords[0].x();
    minY -= coords[0].y();
    maxY -= coords[0].y();
    v1X = v1X / discriminant;
    v1Y = v1Y / discriminant;
    v0X = v0X / -discriminant;
    v0Y = v0Y / -discriminant;

    TransformedImageData& transformedImageData = pipelineData.u.transformedImageData;
    transformedImageData.sourceTexture = sourceTexture;

    GLfloat width = sourceTexture->width();
    GLfloat height = sourceTexture->height();
    transformedImageData.textureLeft = srcRect.x() / width;
    transformedImageData.textureBottom = srcRect.y() / height;
    transformedImageData.textureWidth = srcRect.width() / width;
    transformedImageData.textureHeight = srcRect.height() / height;

    transformedImageData.relativePosition[0] = minY * v1X - minX * v1Y;
    transformedImageData.relativePosition[1] = minY * v0X - minX * v0Y;
    transformedImageData.relativePosition[2] = minY * v1X - maxX * v1Y;
    transformedImageData.relativePosition[3] = minY * v0X - maxX * v0Y;
    transformedImageData.relativePosition[4] = maxY * v1X - minX * v1Y;
    transformedImageData.relativePosition[5] = maxY * v0X - minX * v0Y;
    transformedImageData.relativePosition[6] = maxY * v1X - maxX * v1Y;
    transformedImageData.relativePosition[7] = maxY * v0X - maxX * v0Y;

    transformedImageData.deltaV1X = v1X / 3;
    transformedImageData.deltaV1Y = v1Y / 3;
    transformedImageData.deltaV0X = v0X / 3;
    transformedImageData.deltaV0Y = v0Y / 3;

    paint(PaintTransformedImage, pipelineData);

    if (op != previousCompositeOperator || blendMode != previousBlendMode)
        setCompositeOperation(previousCompositeOperator, previousBlendMode);
}

void PlatformContextGL2D::transformedImageSetShaderArgument(PipelineData& pipelineData)
{
    if (!compileAndUseShader(g_transformedImageShader, 1, &TransformedImageShader::s_program))
        return;

    TransformedImageData& transformedImageData = pipelineData.u.transformedImageData;
    GLuint offset = g_transformedImageShader[1];

    glUniform4f(g_transformedImageShader[offset + TransformedImageShader::uTexturePosition],
                transformedImageData.textureLeft, transformedImageData.textureBottom,
                transformedImageData.textureWidth, transformedImageData.textureHeight);
    glUniform4f(g_transformedImageShader[offset + TransformedImageShader::uDelta],
                transformedImageData.deltaV1X, transformedImageData.deltaV1Y,
                transformedImageData.deltaV0X, transformedImageData.deltaV0Y);

    GLfloat left, bottom, right, top;
    absolutePosition(pipelineData, left, bottom, right, top);

    addTexture(pipelineData, g_transformedImageShader[offset + TransformedImageShader::uTexture], transformedImageData.sourceTexture);

    SET_POSITION16(left, bottom, transformedImageData.relativePosition[0], transformedImageData.relativePosition[1],
                   right, bottom, transformedImageData.relativePosition[2], transformedImageData.relativePosition[3],
                   left, top, transformedImageData.relativePosition[4], transformedImageData.relativePosition[5],
                   right, top, transformedImageData.relativePosition[6], transformedImageData.relativePosition[7]);

    glVertexAttribPointer(g_transformedImageShader[offset + TransformedImageShader::aPosition], 4, GL_FLOAT, GL_FALSE, 0, pipelineData.position);
}

} // namespace WebCore
