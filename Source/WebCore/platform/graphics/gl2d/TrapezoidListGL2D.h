/*
 * Copyright (C) 2013 University of Szeged
 * Copyright (C) 2013 Zoltan Herczeg
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

#ifndef TrapezoidListGL2D_h
#define TrapezoidListGL2D_h

#include "ArenaGL2D.h"
#include "IntRect.h"
#include <wtf/PassOwnPtr.h>
#include <wtf/Vector.h>

namespace WebCore {
namespace GL2D {

class TrapezoidList {
public:
    static PassOwnPtr<TrapezoidList> create(size_t trapezoidCount, IntRect& boundingBox)
    {
        return adoptPtr(new TrapezoidList(trapezoidCount, boundingBox));
    }

    struct Trapezoid {
        void setPoints(float* points)
        {
            for (int i = 0; i < 6; i++)
                m_points[i] = points[i];
        }

        float* points() { return m_points; }

        float& at(size_t i) { return m_points[i]; }

        float& operator[](size_t i) { return m_points[i]; }

        float m_points[6];
    };

    size_t trapezoidCount() { return m_trapezoids.size(); }
    Trapezoid* trapezoids() { return m_trapezoids.data(); }
    const IntRect& boundingBox() const { return m_boundingBox; }
    void insertTrapezoid(Trapezoid&);

private:
    TrapezoidList(size_t trapezoidCount, IntRect& boundingBox)
        : m_boundingBox(boundingBox)
    {
        m_trapezoids.reserveCapacity(trapezoidCount);
    }

    IntRect m_boundingBox;
    Vector<Trapezoid> m_trapezoids;
};

} // namespace GL2D
} // namespace WebCore

#endif // TrapezoidListGL2D_h
