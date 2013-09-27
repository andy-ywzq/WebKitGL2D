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

#ifndef GraphicsContextPlatformPrivateGL2D_h
#define GraphicsContextPlatformPrivateGL2D_h

#include "config.h"
#include "GraphicsContext.h"

#include "SurfaceGL2D.h"

namespace WebCore{

class GraphicsContextPlatformPrivate {
    //WTF_MAKE_NONCOPYABLE(GraphicsContextPlatformPrivate); WTF_MAKE_FAST_ALLOCATED;
public:
    GraphicsContextPlatformPrivate();
    GraphicsContextPlatformPrivate(PlatformGraphicsContext* painter);
    ~GraphicsContextPlatformPrivate();

#if PLATFORM(QT)
    inline QPainter* p() const
    {
        return painter->qpainter();
    }

    QRectF clipBoundingRect() const
    {
        return p()->clipBoundingRect();
    }
#endif
    bool antiAliasingForRectsAndLines;


    InterpolationQuality imageInterpolationQuality;
    bool initialSmoothPixmapTransformHint;

private:
    SurfaceGL2D* painter;
    bool platformContextIsOwned;
};

GraphicsContextPlatformPrivate::GraphicsContextPlatformPrivate(PlatformGraphicsContext* painter)
{

}

GraphicsContextPlatformPrivate::~GraphicsContextPlatformPrivate()
{

}

}
#endif
