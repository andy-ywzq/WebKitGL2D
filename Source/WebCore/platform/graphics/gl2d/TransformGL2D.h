/*
 * Copyright (C) 2013 University of Szeged
 * Copyright (C) 2013 Zoltan Herczeg
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

#ifndef TransformGL2D_h
#define TransformGL2D_h

#include "FloatPoint.h"

namespace WebCore {
namespace GL2D {

class AffineTransform {
public:
    struct Transform {
        // Bottom row of the 3x3 matrix is always [ 0 0 1 ].
        float m_matrix[6];
    };

    enum Type {
        Move,
        Affine,
    };

    AffineTransform()
    {
        m_type = Move;
        m_transform = kIdentity;
    }

    AffineTransform(Transform transform)
    {
        m_transform = transform;
        autoDetectType();
    }

    Type type() const { return m_type; }
    const Transform& transform() const { return m_transform; }

    bool isIdentity() const
    {
        return m_type == Move && m_transform.m_matrix[4] == 0 && m_transform.m_matrix[5] == 0;
    }

    FloatPoint apply(const FloatPoint& location) const
    {
        if (m_type == Move)
            return FloatPoint(location.x() + m_transform.m_matrix[4], location.y() + m_transform.m_matrix[5]);

        float x = location.x() * m_transform.m_matrix[0] + location.y() * m_transform.m_matrix[2] + m_transform.m_matrix[4];
        float y = location.x() * m_transform.m_matrix[1] + location.y() * m_transform.m_matrix[3] + m_transform.m_matrix[5];
        return FloatPoint(x, y);
    }

    void operator= (const AffineTransform& transform)
    {
        m_type = transform.m_type;
        m_transform = transform.m_transform;
    }

    void operator= (Transform transform)
    {
        m_transform = transform;
        autoDetectType();
    }

    void operator*= (const AffineTransform& transform) { multiply(transform.m_transform); }
    void operator*= (const Transform& transform) { multiply(transform); }
    void multiply(const Transform& transform);

    void translate(float, float);
    void scale(float, float);
    void rotate(float);

private:
    static const Transform kIdentity;

    inline void inlineAutoDetectType();
    void autoDetectType();

    // Helps optimizing certain operations..
    Type m_type;
    // Optimized for SIMD instruction sets.
    Transform m_transform;
};

} // namespace GL2D
} // namespace WebCore

#endif // TransformGL2D_h
