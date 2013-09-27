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

#ifndef SurfaceGL2D_h
#define SurfaceGL2D_h

#include "GraphicsContext.h"

#if PLATFORM(QT)
#include <QImage>
#include <QPainter>
#include <QRectF>
#endif

#include <EGL/egl.h>

#include <wtf/Noncopyable.h>
#include <wtf/StdLibExtras.h>

namespace WebCore {

#if PLATFORM(EGL)
class EGLDisplayGL2D;
#endif
class PainterGL2D;
class IntSize;
    
class SurfaceGL2D{
    WTF_MAKE_NONCOPYABLE(SurfaceGL2D);
public:
    SurfaceGL2D();
//     ~SurfaceGL2D();

    enum EGLSurfaceType { PbufferSurface, WindowSurface, PixmapSurface };

    bool isValid() const;

    int width() const;
    int height() const;

    bool getEGLConfig(EGLConfig*, EGLSurfaceType);
    EGLConfig defaultWindowConfig();

#if PLATFORM(QT)
    SurfaceGL2D(QImage*);
    SurfaceGL2D(QPainter*);

    QRectF clipBoundingRect();

    QPainter* qpainter();
    operator QPainter&();
    operator QPainter*();
#endif
    
private:
    EGLDisplay m_eglDisplay;
    EGLSurface m_eglSurface;
    EGLContext m_eglContext;
    EGLConfig m_eglConfig;

#if PLATFORM(QT)
QPainter* m_qpainter;
#endif
    
};

    
} // webcore namespace

#endif
