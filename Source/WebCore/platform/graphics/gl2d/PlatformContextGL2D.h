/*
 * Copyright (C) 2013 University of Szeged
 * Copyright (C) 2013 Kristof Kosztyo
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

#ifndef PlatformContexGL2D_h
#define PlatformContexGL2D_h

#include "ClipRectGL2D.h"
#include "Color.h"
#include "GL2DDefs.h"
#include "GraphicsTypes.h"
#include "FloatRect.h"
#include "IntPoint.h"
#include "IntRect.h"
#include "IntSize.h"
#include "NativeImageGL2D.h"
#include "TransformGL2D.h"
#include <wtf/OwnPtr.h>
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>

namespace WebCore {

class Color;
class GLPlatformContext;
class GLPlatformSurface;
class NativeImageGL2D;

namespace GL2D {
class PathData;
class TrapezoidList;
}

class PlatformContextGL2D : public RefCounted<PlatformContextGL2D> {
public:
    struct ShaderProgram {
        const GLchar* vertexVariables;
        const GLchar* vertexShader;
        const GLchar* fragmentVariables;
        const GLchar* fragmentShader;
        const GLchar** variables;
        int options;
    };

    static const int kNoOptions = 0;
    static const int kVec4Position = 1 << 0;

    static PassRefPtr<PlatformContextGL2D> create(NativeImageGL2D* targetTexture)
    {
        return adoptRef(new PlatformContextGL2D(targetTexture));
    }

    static PlatformContextGL2D* release(PassRefPtr<PlatformContextGL2D> context) { return context.leakRef(); }
    static void createGLContextIfNeed()
    {
        if (!s_offScreenContext)
            createGLContext();
    }
    static GLPlatformContext* offScreenContext() { return s_offScreenContext; }
    static GLPlatformSurface* offScreenSurface() { return s_offScreenSurface; }
    NativeImageGL2D* targetTexture() const { return m_targetTexture; }

    void save();
    void restore();

    void setStrokeColor(const Color& color);
    void setFillColor(const Color& color);
    void setCompositeOperation(CompositeOperator, BlendMode);

    void translate(float x, float y);
    void scale(float, float);
    void rotate(float);
    const GL2D::AffineTransform& transform() const { return m_state.transform; }
    const GL2D::ClipRect& boundingClipRect() { return m_state.boundingClipRect; }
    void setTransform(GL2D::AffineTransform::Transform transform) { m_state.transform = transform; }
    void multiplyTransform(GL2D::AffineTransform::Transform transform) { m_state.transform *= transform; }

    void clip(const FloatRect&);

    void copyImage(const FloatRect&, NativeImageGL2D*, const FloatRect&, CompositeOperator, BlendMode);
    void fillRect(const FloatRect&, const Color&);
    void fillPath(const GL2D::PathData* path);

private:

    struct GL2DState {
        GL2D::AffineTransform transform;
        // In absoulte coordinates inside targetTexture().
        GL2D::ClipRect boundingClipRect;
        RGBA32 fillColor;
        RGBA32 strokeColor;
        CompositeOperator compositeOperator;
        BlendMode blendMode;
    };

    enum PaintType {
        PaintRect,
        PaintPath,
        PaintImage,
        PaintTransformedImage,
    };

    // The following structs are part of a union, so they can only
    // contain primitive types (including pointers or references)
    struct RectData {
        RGBA32 color;
    };

    struct PathData {
        NativeImageGL2D* pathAlphaTexture;
    };

    struct ImageData {
        NativeImageGL2D* sourceTexture;
        GLfloat textureLeft;
        GLfloat textureBottom;
        GLfloat textureRight;
        GLfloat textureTop;
    };

    struct TransformedImageData {
        NativeImageGL2D* sourceTexture;
        GLfloat textureLeft;
        GLfloat textureBottom;
        GLfloat textureWidth;
        GLfloat textureHeight;
        GLfloat relativePosition[8];
        GLfloat deltaV1X;
        GLfloat deltaV1Y;
        GLfloat deltaV0X;
        GLfloat deltaV0Y;
    };

    struct PipelineData {
        union {
            RectData rectData;
            PathData pathData;
            ImageData imageData;
            TransformedImageData transformedImageData;
        } u;

        bool subimageRequired;
        IntRect subimageRect;
        OwnPtr<NativeImageGL2D> subimage;
        int textureIndex;
        GLfloat position[16];
    };

    PlatformContextGL2D(NativeImageGL2D*);
    static void createGLContext();

    static bool compileShaderInternal(GLuint*, int, const ShaderProgram*);
    static bool compileAndUseShader(GLuint* compiledShader, int count, const ShaderProgram* programList)
    {
        if (compiledShader[0]) {
            glUseProgram(compiledShader[0]);
            return true;
        }
        return compileShaderInternal(compiledShader, count, programList);
    }

    static inline void setColorUniform(GLint location, RGBA32 color)
    {
        float r = static_cast<float>(redChannel(color)) / 255;
        float g = static_cast<float>(greenChannel(color)) / 255;
        float b = static_cast<float>(blueChannel(color)) / 255;
        float a = static_cast<float>(alphaChannel(color)) / 255;
        glUniform4f(location, r, g, b, a);
    }

    static bool isInteger(GLfloat value)
    {
        return value == static_cast<GLfloat>(static_cast<int>(value));
    }

    PassOwnPtr<NativeImageGL2D> createPathAlphaTexture(GL2D::TrapezoidList*);

    inline bool fillRectFastPath(const FloatRect&, const Color&);
    inline void copyImageFastPath(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat,
        NativeImageGL2D*, const FloatRect&, CompositeOperator, BlendMode);

    void paint(PaintType, PipelineData&);

    void rectSetShaderArgument(PipelineData&);
    void pathSetShaderArgument(PipelineData&);
    void imageSetShaderArgument(PipelineData&);
    void transformedImageSetShaderArgument(PipelineData&);

    inline void addTexture(PipelineData& pipelineData, GLuint textureUniformPosition, NativeImageGL2D* texture)
    {
        glActiveTexture(GL_TEXTURE0 + pipelineData.textureIndex);
        texture->bindTexture();
        glUniform1i(textureUniformPosition, pipelineData.textureIndex);
        ++pipelineData.textureIndex;
    }

    inline void absolutePosition(PipelineData& pipelineData, GLfloat& left, GLfloat& bottom, GLfloat& right, GLfloat& top)
    {
        if (pipelineData.subimageRequired) {
            left = 0;
            bottom = 0;
            right = pipelineData.subimageRect.width();
            top = pipelineData.subimageRect.height();
        } else {
            left = pipelineData.subimageRect.x();
            bottom = pipelineData.subimageRect.y();
            right = pipelineData.subimageRect.maxX();
            top = pipelineData.subimageRect.maxY();
        }
    }

    NativeImageGL2D* m_targetTexture;
    GL2DState m_state;
    Vector<GL2DState> m_stack;

    static GLPlatformContext* s_offScreenContext;
    static GLPlatformSurface* s_offScreenSurface;
    static GLubyte s_rectIndicies[4];
};

} //namespace WebCore

#endif
