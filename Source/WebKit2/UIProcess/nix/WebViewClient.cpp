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

#include "config.h"
#include "WebViewClientNix.h"

#include "WebViewNix.h"
#include <cstring>

namespace WebKit {

void WebViewClient::initialize(const NIXViewClient* client)
{
    if (!client) {
        memset(&m_client, 0, sizeof(m_client));
        return;
    }

    // TODO: Once struct versions start to matter, change WebViewClient to inherit from
    // APIClient and set the appropriate traits in APIClientTraits. We'll get the right
    // implementation of initialize() for free.
    ASSERT(client->version == kNIXViewClientCurrentVersion);
    m_client = *client;
}

void WebViewClient::viewNeedsDisplay(WebView* view, WKRect rect)
{
    if (!m_client.viewNeedsDisplay)
        return;
    m_client.viewNeedsDisplay(toAPI(view), rect, m_client.clientInfo);
}

void WebViewClient::webProcessCrashed(WebView* view, WKStringRef url)
{
    if (!m_client.webProcessCrashed)
        return;
    m_client.webProcessCrashed(toAPI(view), url, m_client.clientInfo);
}

void WebViewClient::webProcessRelaunched(WebView* view)
{
    if (!m_client.webProcessRelaunched)
        return;
    m_client.webProcessRelaunched(toAPI(view), m_client.clientInfo);
}

void WebViewClient::doneWithTouchEvent(WebView* view, const NIXTouchEvent& event, bool wasEventHandled)
{
    if (!m_client.doneWithTouchEvent)
        return;
    m_client.doneWithTouchEvent(toAPI(view), &event, wasEventHandled, m_client.clientInfo);
}

void WebViewClient::doneWithGestureEvent(WebView* view, const NIXGestureEvent& event, bool wasEventHandled)
{
    if (!m_client.doneWithGestureEvent)
        return;
    m_client.doneWithGestureEvent(toAPI(view), &event, wasEventHandled, m_client.clientInfo);
}

void WebViewClient::pageDidRequestScroll(WebView* view, WKPoint point)
{
    if (!m_client.pageDidRequestScroll)
        return;
    m_client.pageDidRequestScroll(toAPI(view), point, m_client.clientInfo);
}

void WebViewClient::didChangeContentsSize(WebView* view, WKSize size)
{
    if (!m_client.didChangeContentsSize)
        return;
    m_client.didChangeContentsSize(toAPI(view), size, m_client.clientInfo);
}

void WebViewClient::didChangeViewportAttributes(WebView* view, float width, float height, float minimumScale, float maximumScale, float initialScale, int userScalable)
{
    if (!m_client.didChangeViewportAttributes)
        return;
    m_client.didChangeViewportAttributes(toAPI(view), width, height, minimumScale, maximumScale, initialScale, userScalable, m_client.clientInfo);
}

void WebViewClient::didFindZoomableArea(WebView* view, WKPoint target, WKRect area)
{
    if (!m_client.didFindZoomableArea)
        return;
    m_client.didFindZoomableArea(toAPI(view), target, area, m_client.clientInfo);
}

void WebViewClient::updateTextInputState(WebView* view, const WTF::String& selectedText, const WTF::String& surroundingText, uint64_t inputMethodHints,
                                         bool isContentEditable, const WebCore::IntRect& cursorRect, const WebCore::IntRect& editorRect)
{
    if (!m_client.updateTextInputState)
        return;
    m_client.updateTextInputState(toAPI(view), toAPI(selectedText.impl()), toAPI(surroundingText.impl()), inputMethodHints,
                                  isContentEditable, toAPI(cursorRect), toAPI(editorRect), m_client.clientInfo);
}

} // namespace WebKit
