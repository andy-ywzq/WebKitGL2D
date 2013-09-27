/*
 * Copyright (C) 2012-2013 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2013 University of Szeged
 * Copyright (C) 2013 Roland Takacs <rtakacs@inf.u-szeged.hu>
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

#include "../BrowserControl.h"
#if WTF_USE_OPENGL_ES_2
#include <GLES2/gl2.h>
#else
#include <GL/gl.h>
#endif

WebView::WebView(Display* display, Window parent, XContext context, BrowserControl* control, WKRect size)
    : VisualComponent(display, control, size)
{
    createXWindow(parent, context);

    createGLContext();
    makeCurrent();
    glEnable(GL_DEPTH_TEST);
}

WebView::~WebView()
{
    destroyGLContext();
    XDestroyWindow(m_display, m_window);
}

void WebView::createXWindow(Window parent, XContext context)
{
    XVisualInfo visualInfoTemplate;
    int visualInfoCount;
    visualInfoTemplate.visualid = setupXVisualID();
    XVisualInfo* visualInfo = XGetVisualInfo(m_display, VisualIDMask, &visualInfoTemplate, &visualInfoCount);
    if (!visualInfo)
        fatalError("couldn't get X visual\n");

    XSetWindowAttributes setAttributes;
    setAttributes.colormap = XCreateColormap(m_display, parent, visualInfo->visual, AllocNone);
    setAttributes.event_mask = ExposureMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;

    m_window = XCreateWindow(m_display, parent, m_size.origin.x, m_size.origin.y, m_size.size.width, m_size.size.height, 0, visualInfo->depth, InputOutput, visualInfo->visual, CWColormap | CWEventMask, &setAttributes);
    XFree(visualInfo);

    XMapWindow(m_display, m_window);
    XSaveContext(m_display, m_window, context, (XPointer)this);
}

void WebView::handleEvent(const XEvent& event)
{
    switch (event.type) {
    case Expose:
        m_control->handleExposeEvent();
        break;
    case ButtonPress:
        m_control->handleButtonPressEvent(reinterpret_cast<const XButtonPressedEvent&>(event));
        break;
    case ButtonRelease:
        m_control->handleButtonReleaseEvent(reinterpret_cast<const XButtonReleasedEvent&>(event));
        break;
    case MotionNotify:
        m_control->handlePointerMoveEvent(reinterpret_cast<const XPointerMovedEvent&>(event));
        break;
    }
}
