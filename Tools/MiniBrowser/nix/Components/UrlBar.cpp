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

#include "UrlBar.h"

#include "../BrowserControl.h"
#include <fstream>
#include <X11/cursorfont.h>
#include <X11/keysym.h>

UrlBar::UrlBar(Display* display, Window parent, XContext context, BrowserControl* control, WKRect size, std::string url)
    : VisualComponent(display, control, size)
    , m_isFocused(false)
    , m_url(url)
    , m_copiedText(url)
{
    createXWindow(parent, context);

    m_surface = cairo_xlib_surface_create(display, m_window, DefaultVisual(display, 0), m_size.size.width, m_size.size.height);
    m_cairo = cairo_create(m_surface);
    cairo_select_font_face(m_cairo, "Verdana", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(m_cairo, 14);

    XDefineCursor(display, m_window, XCreateFontCursor(display, XC_xterm));

    m_clipboardAtom = XInternAtom(m_display, "CLIPBOARD", True);
}

UrlBar::~UrlBar()
{
    cairo_surface_destroy(m_surface);
    cairo_destroy(m_cairo);
}

void UrlBar::createXWindow(Window parent, XContext context)
{
    m_window = XCreateSimpleWindow(m_display, parent, m_size.origin.x, m_size.origin.y, m_size.size.width, m_size.size.height, 1, createXColor("#C1C1C1"), WhitePixel(m_display, 0));

    XSelectInput(m_display, m_window, ExposureMask | ButtonPressMask | ButtonReleaseMask);
    XSaveContext(m_display, m_window, context, (XPointer)this);
    XMapWindow(m_display, m_window);
}

void UrlBar::handleEvent(const XEvent& event)
{
    switch (event.type) {
    case Expose:
        drawUrlBar();
        break;
    case ButtonRelease:
        addFocus();
        break;
    case SelectionRequest:
        respondClipboardRequest(event.xselectionrequest);
        break;
    case SelectionNotify:
        pasteClipboardText(event);
        break;
    }
}

void UrlBar::setText(std::string url)
{
    m_url = url;

    drawUrlBar();
}

void UrlBar::setLoadProgress(double progress)
{
    m_loadProgress = progress;

    drawUrlBar();
}

bool UrlBar::focused()
{
    return m_isFocused;
}

void UrlBar::addFocus()
{
    if (!m_isFocused) {
        m_isFocused = true;
        m_control->passFocusToUrlBar();

        drawCursor();
    }
}

void UrlBar::releaseFocus()
{
    if (m_isFocused) {
        m_isFocused = false;
        drawUrlBar();
    }
}

void UrlBar::drawUrlBar()
{
    XClearWindow(m_display, m_window);
    drawBackground();
    drawText();
    drawCursor();
}

void UrlBar::drawBackground()
{
    if (m_loadProgress == 0.0 || m_loadProgress == 1.0)
        return;
    cairo_set_source_rgb(m_cairo, 0.8, 0.8, 0.8);
    cairo_rectangle(m_cairo, 0, 0, m_size.size.width * m_loadProgress, m_size.size.height);
    cairo_fill(m_cairo);
}

void UrlBar::drawText()
{
    cairo_set_source_rgb(m_cairo, 0.2, 0.2, 0.2);
    cairo_text_extents(m_cairo, m_url.c_str(), &m_extents);

    int offset = 8;

    if (m_extents.x_advance + offset > m_size.size.width - offset)
        offset = m_size.size.width - m_extents.x_advance - offset;

    cairo_move_to(m_cairo, offset, 16);
    cairo_show_text(m_cairo, m_url.c_str());
}

void UrlBar::drawCursor()
{
    if (m_isFocused) {
        int padding = 8;
        int x = m_extents.x_advance + padding;

        if (m_extents.x_advance + padding > m_size.size.width - padding)
            x = m_size.size.width - padding;

        cairo_move_to(m_cairo, x, 4);
        cairo_line_to(m_cairo, x, 18);

        cairo_stroke(m_cairo);
    }
}

void UrlBar::handleKeyReleaseEvent(const XKeyReleasedEvent& event)
{
    char normalKey;
    KeySym specialKey;

    XLookupString(const_cast<XKeyEvent*>(&event), &normalKey, sizeof(char), &specialKey, 0);

    switch (specialKey) {
    case XK_BackSpace:
        removeLastCharacter();
        break;
    case XK_Return:
        loadPage();
        break;
    case XK_Insert:
        if (event.state & ControlMask)
            becomeClipboardOwner();
        else if (event.state & ShiftMask)
            requestClipboardText();
        break;
    default:
        if (event.state & ControlMask) {
            // while the Control key is pressed, character codes start from 1 (A=1, B=2, ...).
            switch (normalKey + 'A' - 1) {
            case 'C':
                becomeClipboardOwner();
                break;

            case 'V':
                requestClipboardText();
                break;
            }
        } else
            appendCharacter(normalKey);

        break;
    }

    drawUrlBar();
}

void UrlBar::removeLastCharacter()
{
    if (m_url.size())
        m_url.erase(m_url.size() - 1);
}

void UrlBar::appendCharacter(const char c)
{
    if ((c >= 32) && (c <= 127))
        m_url.append(sizeof(char), c);
}

void UrlBar::loadPage()
{
    if (m_url.find("http") != 0 && m_url.find("file://") != 0) {
        std::ifstream localFile(m_url.c_str());
        m_url.insert(0, localFile ? "file://" : "http://");
    }

    m_control->loadPage(m_url.c_str());
}

void UrlBar::requestClipboardText()
{
    XConvertSelection(m_display, m_clipboardAtom, XA_STRING, XA_STRING, m_window, CurrentTime);
}

void UrlBar::pasteClipboardText(const XEvent &event)
{
    Atom clipType;
    int clipFormat;
    unsigned long clipLength;
    unsigned long clipRemainingBytes;
    unsigned char *clipBuffer = 0;

    if (event.xselection.property == None)
        return;

    XGetWindowProperty(m_display, m_window,
        event.xselection.property, 0, 255, False, AnyPropertyType,
        &clipType, &clipFormat, &clipLength, &clipRemainingBytes, &clipBuffer);

    for (unsigned long i = 0; i < clipLength; i++)
        appendCharacter(clipBuffer[i]);

    XFree(clipBuffer);
}

void UrlBar::becomeClipboardOwner()
{
    m_copiedText = m_url;

    XSetSelectionOwner(m_display, m_clipboardAtom, m_window, CurrentTime);
    if (XGetSelectionOwner(m_display, m_clipboardAtom) != m_window)
        fprintf(stderr, "Could not set clipboard ownership.\n");

    XSetSelectionOwner(m_display, XA_PRIMARY, m_window, CurrentTime);
    if (XGetSelectionOwner(m_display, XA_PRIMARY) != m_window)
        fprintf(stderr, "Could not set primary selection ownership.\n");
}

void UrlBar::respondClipboardRequest(const XSelectionRequestEvent& requestEvent)
{
    XEvent responseEvent;
    responseEvent.xselection.type = SelectionNotify;
    responseEvent.xselection.property = None;
    responseEvent.xselection.display = requestEvent.display;
    responseEvent.xselection.requestor = requestEvent.requestor;
    responseEvent.xselection.selection = requestEvent.selection;
    responseEvent.xselection.target = requestEvent.target;
    responseEvent.xselection.time = requestEvent.time;

    XChangeProperty(m_display, requestEvent.requestor, requestEvent.property, XA_STRING,
        8, PropModeReplace, reinterpret_cast<const unsigned char *>(m_copiedText.c_str()), m_copiedText.length());

    responseEvent.xselection.property = requestEvent.property;
    XSendEvent(m_display, requestEvent.requestor, False, 0, &responseEvent);
    XFlush(m_display);
}
