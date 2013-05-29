/*
 * Copyright (C) 2012-2013 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2013 University of Szeged
 * Copyright (C) 2013 Roland Takacs <rtakacs@inf.u-szeged.hu>
 * Copyright (C) 2013 Matyas Mustoha <mmatyas@inf.u-szeged.hu>
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

#ifndef UrlBar_h
#define UrlBar_h

#include "VisualComponent.h"

#include <X11/Xatom.h>
#include <cairo-xlib.h>
#include <string>

class UrlBar : public VisualComponent {
public:
    UrlBar(Display*, Window, XContext, BrowserControl*, WKRect, std::string);
    ~UrlBar();

    virtual void handleEvent(const XEvent&);
    void handleKeyReleaseEvent(const XKeyReleasedEvent&);

    void addFocus();
    void releaseFocus();
    bool focused();

    void setText(std::string);

private:
    virtual void createXWindow(Window parent, XContext);

    void removeLastCharacter();
    void appendCharacter(const char);
    void loadPage();

    void drawCursor();
    void drawText();
    void drawUrlBar();

    void becomeClipboardOwner();
    void respondClipboardRequest(const XSelectionRequestEvent&);
    void requestClipboardText();
    void pasteClipboardText(const XEvent&);

    std::string m_url;
    std::string m_copiedText;
    bool m_isFocused;
    Atom m_clipboardAtom;

    cairo_t* m_cairo;
    cairo_surface_t* m_surface;
    cairo_text_extents_t m_extents;
};

#endif
