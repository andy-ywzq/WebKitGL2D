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

#ifndef ClipRect_h
#define ClipRect_h

#include "IntRect.h"

namespace WebCore {
namespace GL2D {

class ClipRect {
    friend class PlatformContextGL2D;
public:
    ClipRect();

    static ClipRect createRect(IntRect rect)
    {
        return ClipRect(rect.x(), rect.y(), rect.maxX(), rect.maxY());
    }

    int leftInteger() const { return m_intPoints[0]; }
    int bottomInteger() const { return m_intPoints[1]; }
    int rightInteger() const { return m_intPoints[2]; }
    int topInteger() const { return m_intPoints[3]; }

    float left() const { return m_floatPoints[0]; }
    float bottom() const { return m_floatPoints[1]; }
    float right() const { return m_floatPoints[2]; }
    float top() const { return m_floatPoints[3]; }

    void intersect(int, int, int, int);
    void intersect(IntRect rect)
    {
        intersect(rect.x(), rect.y(), rect.maxX(), rect.maxY());
    }

    void setDimension(int, int);

private:
    ClipRect(int, int, int, int);

    int m_intPoints[4];
    float m_floatPoints[4];
};

} // namespace GL2D
} // namespace WebCore

#endif
