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
#include "NativeImageGL2D.h"

#include "GLDefs.h"
#include "PlatformContextGL2D.h"

#if USE(EGL) && PLATFORM(X11)
#include "EGLConfigSelector.h"
#include "EGLXSurface.h"
#include "GLPlatformContext.h"
#include "GLPlatformSurface.h"
#include "X11Helper.h"
#else
#error Needs shared image implementation
#endif

#include "stdio.h"

namespace WebCore{

NativeImageGL2D::NativeImageGL2D(int width, int height, const void* buffer,  bool alpha)
    : m_size(width, height)
    , m_sharedImageHandle(0)
    , m_privateImageHandle(0)
    , m_texture(0)
    , m_fbo(0)
    , m_renderBuffer(0)
    , m_alpha(alpha)
{
    PlatformContextGL2D::createGLContextIfNeed();

    if (!width || !height) {
        m_size = IntSize(0, 0);
        return;
    }

    glGenTextures(1, &m_texture);
    if (!m_texture)
        CRASH();

    glBindTexture(GL_TEXTURE_2D, m_texture);
    // Note: mipmaps are not used.
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
}

NativeImageGL2D::NativeImageGL2D(int width, int height, uintptr_t sharedImageHandle)
    : m_size(width, height)
    , m_sharedImageHandle(0)
    , m_privateImageHandle(0)
    , m_texture(0)
    , m_fbo(0)
    , m_renderBuffer(0)
    , m_alpha(false)
{
    PlatformContextGL2D::createGLContextIfNeed();
#if USE(EGL) && PLATFORM(X11)
    EGLint imageAttributes[] = {
        EGL_IMAGE_PRESERVED_KHR, EGL_FALSE,
        EGL_NONE
    };

    EGLConfigSelector* configSelector = reinterpret_cast<EGLPixmapSurface*>(PlatformContextGL2D::offScreenSurface())->configSelector();
    ASSERT(configSelector);

    EGLConfig config = configSelector->pixmapContextConfig();
    ASSERT(config);

    EGLint visualId = configSelector->nativeVisualId(config);
    ASSERT(visualId);

    Pixmap pixmap;
    if (!sharedImageHandle) {
        X11Helper::createPixmap(&pixmap, visualId, true, IntSize(width, height));
        m_sharedImageHandle = static_cast<uintptr_t>(pixmap);
    } else
        pixmap = static_cast<Pixmap>(sharedImageHandle);

    if(!pixmap)
        CRASH();

    EGLImageKHR eglImage = eglCreateImageKHR(PlatformContextGL2D::offScreenSurface()->sharedDisplay(),
                                    EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR,
                                    reinterpret_cast<EGLClientBuffer> (pixmap), imageAttributes);

    if(!eglImage)
        CRASH();

    m_privateImageHandle = reinterpret_cast<uintptr_t>(eglImage);
#endif
}

NativeImageGL2D::~NativeImageGL2D()
{
    if (m_fbo)
        glDeleteFramebuffers(1, &m_fbo);
    if (m_renderBuffer)
        glDeleteRenderbuffers(1, &m_renderBuffer);
#if USE(EGL) && PLATFORM(X11)
    if (m_privateImageHandle)
        eglDestroyImageKHR(PlatformContextGL2D::offScreenSurface()->sharedDisplay(), reinterpret_cast<EGLImageKHR>(m_privateImageHandle));
    if (m_sharedImageHandle)
        X11Helper::destroyPixmap(m_sharedImageHandle);
#endif
    if (m_texture)
        glDeleteTextures(1, &m_texture);
}

void NativeImageGL2D::bindFbo()
{
    if (m_fbo) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        return;
    }

    // Create a framebuffer object.
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    if (m_texture) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);
        return;
    }

    ASSERT(m_privateImageHandle);
    glGenRenderbuffers(1, &m_renderBuffer);
    if (!m_renderBuffer)
        CRASH();

    glBindRenderbuffer(GL_RENDERBUFFER, m_renderBuffer);
    glEGLImageTargetRenderbufferStorageOES(GL_RENDERBUFFER, reinterpret_cast<GLeglImageOES>(m_privateImageHandle));
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_renderBuffer);
}

} // namespace WebCore
