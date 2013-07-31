/*
 * Copyright (C) 2012-2013 Nokia Corporation and/or its subsidiary(-ies).
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef GLUtilities_h
#define GLUtilities_h

#include <iostream>
#include <iomanip>

#if USE(OPENGL_ES_2)
#include <GLES2/gl2.h>
#else
#include <GL/gl.h>
#endif

#if USE(EGL)
#include <EGL/egl.h>
#else
#include <X11/Xlib.h>
#include <GL/glx.h>
#endif

namespace ToolsNix {

struct RGBAPixel {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;

    bool operator == (const RGBAPixel& other) const { return compare(other) == 0; }
    bool operator != (const RGBAPixel& other) const { return compare(other) != 0; }
    bool operator < (const RGBAPixel& other) const { return compare(other) == -1; }
    bool operator > (const RGBAPixel& other) const { return compare(other) == 1; }

    int compare (const RGBAPixel& other) const {
        if (r != other.r) return r < other.r ? (-1) : (1);
        if (g != other.g) return g < other.g ? (-1) : (1);
        if (b != other.b) return b < other.b ? (-1) : (1);
        if (a != other.a) return a < other.a ? (-1) : (1);
        return 0;
    }

    static RGBAPixel white() { RGBAPixel p = { 255, 255, 255, 255 }; return p; }
    static RGBAPixel black() { RGBAPixel p = { 0, 0, 0, 255 }; return p; }
    static RGBAPixel red() { RGBAPixel p = { 255, 0, 0, 255 }; return p; }
    static RGBAPixel green() { RGBAPixel p = { 0, 255, 0, 255 }; return p; }
    static RGBAPixel blue() { RGBAPixel p = { 0, 0, 255, 255 }; return p; }
};

// Gtest uses this to print RGBAPixels when tests fail.
inline std::ostream& operator<<(std::ostream& os, const RGBAPixel& p) {
    return os << std::hex << std::setfill('0')
              << std::setw(2) << unsigned(p.r)
              << std::setw(2) << unsigned(p.g)
              << std::setw(2) << unsigned(p.b)
              << std::setw(2) << unsigned(p.a);
}

void dumpGLBufferToPng(const char* fileName, int width, int height);

class GLOffscreenBuffer {
public:
    GLOffscreenBuffer(unsigned width, unsigned height);
    ~GLOffscreenBuffer();

    bool makeCurrent();
    void dumpToPng(const char* fileName);
    RGBAPixel readPixelAtPoint(unsigned x, unsigned y);

private:
    int m_width;
    int m_height;
#if USE(EGL)
    EGLDisplay m_display;
    EGLContext m_context;
    EGLSurface m_surface;
#else
    Display* m_display;
    GLXPbuffer m_pbuffer;
    GLXContext m_context;
#endif
};

} // namespace ToolsNix

#endif // GLUtilities_h
