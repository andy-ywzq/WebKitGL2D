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

#include "BrowserControl.h"

BrowserControl::BrowserControl(WebViewClient * client, int width, int height, std::string url)
    : m_client(client)
    , m_display(XOpenDisplay(0))
    , m_context(XUniqueContext())
    , m_eventSource(0)
{
    if (!m_display)
        fatalError("couldn't connect to X server\n");

    m_eventSource = new XlibEventSource(m_display, this);

    // create the main window
    m_browserWindow = new BrowserWindow(m_display, DefaultRootWindow(m_display), m_context, this, WKRectMake(0, 0, width, height));

    // separate the main window
    m_toolBar = new ToolBar(m_display, m_browserWindow->window(), m_context, this, WKRectMake(0, 0, width, toolBarHeight));
    m_webView = new WebView(m_display, m_browserWindow->window(), m_context, this, WKRectMake(0, toolBarHeight, width, height - toolBarHeight));

    // create navigation buttons
    m_backButton = new Button(m_display, m_toolBar->window(), m_context, this, WKRectMake(backButtonX, toolBarElementY, buttonWidth, buttonHeight), Button::Back);
    m_forwardButton = new Button(m_display, m_toolBar->window(), m_context, this, WKRectMake(forwardButtonX, toolBarElementY, buttonWidth, buttonHeight), Button::Forward);
    m_refreshButton = new Button(m_display, m_toolBar->window(), m_context, this, WKRectMake(refreshButtonX, toolBarElementY, buttonWidth, buttonHeight), Button::Refresh);

    // create a simple urlbar
    m_urlBar = new UrlBar(m_display, m_toolBar->window(), m_context, this, WKRectMake(urlBarX, toolBarElementY, width - urlBarRightOffset, urlBarHeight), url);
}

BrowserControl::~BrowserControl()
{
    delete m_eventSource;

    delete m_backButton;
    delete m_forwardButton;
    delete m_refreshButton;
    delete m_urlBar;
    delete m_toolBar;
    delete m_browserWindow;

    XCloseDisplay(m_display);
}

void BrowserControl::handleXEvent(const XEvent& event)
{
    VisualComponent *component = 0;
    XFindContext(event.xany.display, event.xany.window, m_context, (XPointer*)&component);

    // Send event to the appropriate component that will be handle
    if (component)
        component->handleEvent(event);
}

void BrowserControl::makeCurrent()
{
    m_webView->makeCurrent();
}

void BrowserControl::swapBuffers()
{
    m_webView->swapBuffers();
}

WKSize BrowserControl::windowSize()
{
    return m_browserWindow->size();
}

WKSize BrowserControl::webViewSize()
{
    return m_webView->size();
}

void BrowserControl::setWindowTitle(const char* title)
{
    m_browserWindow->setTitle(title);
}

void BrowserControl::passFocusToWebView()
{
    m_client->addFocus();
    m_urlBar->releaseFocus();
}

void BrowserControl::passFocusToUrlBar()
{
    m_client->releaseFocus();
}

bool BrowserControl::isUrlBarFocused()
{
    return m_urlBar->focused();
}

void BrowserControl::pageGoBack()
{
    m_client->pageGoBack();
    m_urlBar->setText(m_client->activeUrl());
    passFocusToWebView();
}

void BrowserControl::pageGoForward()
{
    m_client->pageGoForward();
    m_urlBar->setText(m_client->activeUrl());
    passFocusToWebView();
}

void BrowserControl::pageReload()
{
    m_client->pageReload();
    m_urlBar->setText(m_client->activeUrl());
    passFocusToWebView();
}

void BrowserControl::loadPage(const char* url)
{
    m_client->loadPage(url);
    m_urlBar->setText(m_client->activeUrl());
    passFocusToWebView();
}

void BrowserControl::setLoadProgress(double progress)
{
    m_urlBar->setLoadProgress(progress);
}

void BrowserControl::handleExposeEvent()
{
    m_client->handleExposeEvent();
}

void BrowserControl::handleKeyPressEvent(const XKeyPressedEvent& keyEvent)
{
    if (!m_urlBar->focused())
        m_client->handleKeyPressEvent(keyEvent);
}

void BrowserControl::handleKeyReleaseEvent(const XKeyReleasedEvent& keyEvent)
{
    if (m_urlBar->focused())
        m_urlBar->handleKeyReleaseEvent(keyEvent);
    else
        m_client->handleKeyReleaseEvent(keyEvent);
}

void BrowserControl::handleButtonPressEvent(const XButtonPressedEvent& event)
{
    m_client->handleButtonPressEvent(event);
    passFocusToWebView();
}

void BrowserControl::handleButtonReleaseEvent(const XButtonReleasedEvent& event)
{
    m_client->handleButtonReleaseEvent(event);
    passFocusToWebView();
}

void BrowserControl::handlePointerMoveEvent(const XPointerMovedEvent& event)
{
    m_client->handlePointerMoveEvent(event);
}

void BrowserControl::handleSizeChanged(int width, int height)
{
    m_urlBar->resize(((width - urlBarRightOffset) > minimumUrlBarWidth) ? width - urlBarRightOffset : minimumUrlBarWidth, urlBarHeight);
    m_toolBar->resize(width, toolBarHeight);
    m_webView->resize(width, height);

    m_client->handleSizeChanged(width, height - toolBarHeight);
}

void BrowserControl::handleClose()
{
    m_client->handleClosed();
}
