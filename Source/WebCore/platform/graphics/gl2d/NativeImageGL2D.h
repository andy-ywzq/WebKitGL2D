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

#ifndef NativeImageGL2D_h
#define NativeImageGL2D_h

#include "GL2DDefs.h"
#include "IntSize.h"
#include <wtf/Assertions.h>
#include <wtf/FastAllocBase.h>
#include <wtf/PassOwnPtr.h>

namespace WebCore {

class NativeImageGL2D {
    WTF_MAKE_FAST_ALLOCATED;
public:

    static NativeImageGL2D* createPtr(int width, int height, const void* buffer = 0, bool alpha = false)
    {
        return new NativeImageGL2D(width, height, buffer, alpha);
    }

    static NativeImageGL2D* createPtr(const IntSize& size, const void* buffer = 0, bool alpha = false)
    {
        return createPtr(size.width(), size.height(), buffer, alpha);
    }

    static PassOwnPtr<NativeImageGL2D> create(int width, int height, const void* buffer = 0, bool alpha = false)
    {
        return adoptPtr(new NativeImageGL2D(width, height, buffer, alpha));
    }

    static PassOwnPtr<NativeImageGL2D> create(const IntSize& size, const void* buffer = 0, bool alpha = false)
    {
        return create(size.width(), size.height(), buffer, alpha);
    }

    static PassOwnPtr<NativeImageGL2D> createShared(int width, int height, uintptr_t sharedImageHandle = 0)
    {
        return adoptPtr(new NativeImageGL2D(width, height, sharedImageHandle));
    }

    static PassOwnPtr<NativeImageGL2D> createShared(const IntSize& size, uintptr_t sharedImageHandle = 0)
    {
        return createShared(size.width(), size.height(), sharedImageHandle);
    }

    ~NativeImageGL2D();

    const IntSize& size() const { return m_size; }
    int width() const { return m_size.width(); }
    int height() const { return m_size.height(); }
    uintptr_t sharedImageHandle() { return m_sharedImageHandle; }
    GLuint texture() const { return m_texture; }
    size_t sizeInBytes() const { return width() * height() * 4; }
    bool hasAlpha() const { return m_alpha; }
    GLuint fbo() const { return m_fbo; }

    void bindTexture() { glBindTexture(GL_TEXTURE_2D, m_texture); }
    void bindFbo();

protected:
    NativeImageGL2D(int, int, const void* = 0, bool = false);
    NativeImageGL2D(int, int, uintptr_t);

private:
    IntSize m_size;
    // m_sharedImageHandle and m_texture cannot be non 0 in the same time.
    uintptr_t m_sharedImageHandle;
    uintptr_t m_privateImageHandle;
    GLuint m_texture;
    GLuint m_fbo;
    GLuint m_renderBuffer;
    bool m_alpha;
};

}
#endif
