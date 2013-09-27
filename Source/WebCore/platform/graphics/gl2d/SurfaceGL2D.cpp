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

#include "config.h"
#include "SurfaceGL2D.h"
#include "EGLUtils.h"

namespace WebCore {

    
SurfaceGL2D::SurfaceGL2D()
{
}

// SurfaceGL2D

bool SurfaceGL2D::getEGLConfig(EGLConfig* config, SurfaceGL2D::EGLSurfaceType surfaceType)
{
    EGLint attributeList[] = {
        //#if USE(OPENGL_ES_2)
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        //#else
        //EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        //#endif
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_STENCIL_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_SURFACE_TYPE, EGL_NONE,
        EGL_NONE
    };
    
    switch (surfaceType) {
        case SurfaceGL2D::PbufferSurface:
            attributeList[13] = EGL_PBUFFER_BIT;
            break;
        case SurfaceGL2D::PixmapSurface:
            attributeList[13] = EGL_PIXMAP_BIT;
            break;
        case SurfaceGL2D::WindowSurface:
            attributeList[13] = EGL_WINDOW_BIT;
            break;
    }
    
    EGLint numberConfigsReturned;
    return eglChooseConfig(m_eglDisplay, attributeList, config, 1, &numberConfigsReturned) && numberConfigsReturned;
}

bool SurfaceGL2D::isValid() const
{
    return (m_eglSurface != EGL_NO_SURFACE);
}

EGLConfig SurfaceGL2D::defaultWindowConfig()
{
    EGLConfig config;
    EGLint numConfigs;
    
    // Hopefully the client will have set the window config of its choice
    // by now - if not, use a 32-bit generic one as default.
    //if (!0/*m_windowConfigId*/) {
        static const EGLint configAttribs[] = {
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_ALPHA_MASK_SIZE, 1,
            EGL_LUMINANCE_SIZE, EGL_DONT_CARE,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE, EGL_OPENVG_BIT,
            EGL_NONE
        };
        eglChooseConfig(m_eglDisplay, configAttribs, &config, 1, &numConfigs);
    //} else {
//         const EGLint configAttribs[] = {
//             EGL_CONFIG_ID, m_windowConfigId,
//             EGL_NONE
//         };
//         eglChooseConfig(m_eglDisplay, configAttribs, &config, 1, &numConfigs);
//     }
    
    ASSERT_EGL_NO_ERROR();
    //ASSERT(numConfigs == 1);
    return config;
}

#if PLATFORM(QT)
SurfaceGL2D::SurfaceGL2D(QImage* image)
{
    m_qpainter = new QPainter(image);
    printf("qimage\n");
}

SurfaceGL2D::SurfaceGL2D(QPainter* painter)
{
    m_qpainter = painter;
    eglBindAPI(EGL_OPENGL_ES_API);
    ASSERT_EGL_NO_ERROR();
    m_eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(m_eglDisplay, 0, 0);
    ASSERT_EGL_NO_ERROR();
    getEGLConfig(&m_eglConfig, PbufferSurface);
    const EGLint attribList[] = {
        EGL_WIDTH, m_qpainter->device()->width(),
        EGL_HEIGHT, m_qpainter->device()->height(),
        EGL_NONE
    };
    ASSERT_EGL_NO_ERROR();
    m_eglSurface = eglCreatePbufferSurface(m_eglDisplay, m_eglConfig, attribList);
    ASSERT_EGL_NO_ERROR();
}

QRectF SurfaceGL2D::clipBoundingRect()
{
    return QRectF();
}

QPainter* SurfaceGL2D::qpainter()
{
    return m_qpainter;
}

SurfaceGL2D::operator QPainter&()
{
    static QPainter *p = 0;
//     static QImage *i = 0;

    return *p;
}

SurfaceGL2D::operator QPainter*()
{
    static QPainter *p = 0;
    //     static QImage *i = 0;
    
    return p;
}
#endif

} // WebCore
