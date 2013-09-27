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

#include "WebView.h"

#include <EGL/egl.h>
#include <GLES2/gl2.h>

struct WebView::GLContextData {
    EGLDisplay eglDisplay;
    EGLSurface surface;
    EGLContext context;
    EGLConfig config;
};

VisualID WebView::setupXVisualID()
{
    m_glContextData = new GLContextData;

    m_glContextData->eglDisplay = eglGetDisplay(m_display);
    if (!m_glContextData->eglDisplay)
        fatalError("eglGetDisplay() failed\n");

    if (!eglInitialize(m_glContextData->eglDisplay, 0, 0))
        fatalError("eglInitialize() failed\n");

    static const EGLint attributes[] = {
        EGL_DEPTH_SIZE, 24,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };

    EGLint configCount;
    if (!eglChooseConfig(m_glContextData->eglDisplay, attributes, &m_glContextData->config, 1, &configCount))
        fatalError("eglChooseConfig() failed\n");

    EGLint visualID;
    if (!eglGetConfigAttrib(m_glContextData->eglDisplay, m_glContextData->config, EGL_NATIVE_VISUAL_ID, &visualID))
        fatalError("couldn't get the native visual ID\n");

    return visualID;
}

void WebView::createGLContext()
{
    static const EGLint contextAttributes[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE,
    };

    eglBindAPI(EGL_OPENGL_ES_API);
    m_glContextData->context = eglCreateContext(m_glContextData->eglDisplay, m_glContextData->config, EGL_NO_CONTEXT, contextAttributes);
    if (!m_glContextData->context)
        fatalError("eglCreateContext failed\n");

    m_glContextData->surface = eglCreateWindowSurface(m_glContextData->eglDisplay, m_glContextData->config, m_window, 0);
}

void WebView::destroyGLContext()
{
    eglMakeCurrent(m_glContextData->eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(m_glContextData->eglDisplay, m_glContextData->context);
    eglDestroySurface(m_glContextData->eglDisplay, m_glContextData->surface);
    eglTerminate(m_glContextData->eglDisplay);

    delete m_glContextData;
}

void WebView::makeCurrent()
{
    eglMakeCurrent(m_glContextData->eglDisplay, m_glContextData->surface, m_glContextData->surface, m_glContextData->context);
}

void WebView::swapBuffers()
{
    eglSwapBuffers(m_glContextData->eglDisplay, m_glContextData->surface);
}
