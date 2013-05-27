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

#ifndef WebViewClient_h
#define WebViewClient_h

#include "NIXView.h"
#include "IntRect.h"
#include "wtf/text/WTFString.h"

namespace WebKit {

class WebView;

class WebViewClient {
public:
    WebViewClient()
    {
        initialize(0);
    }

    void initialize(const NIXViewClient* client);

    const NIXViewClient& client() const { return m_client; }

    void viewNeedsDisplay(WebView*, WKRect area);
    void webProcessCrashed(WebView*, WKStringRef url);
    void webProcessRelaunched(WebView*);
    void doneWithTouchEvent(WebView*, const NIXTouchEvent&, bool wasEventHandled);
    void doneWithGestureEvent(WebView*, const NIXGestureEvent&, bool wasEventHandled);
    void pageDidRequestScroll(WebView*, WKPoint position);
    void didChangeContentsSize(WebView*, WKSize size);
    void didChangeViewportAttributes(WebView*, float width, float height, float minimumScale, float maximumScale, float initialScale, int userScalable);
    void didFindZoomableArea(WebView*, WKPoint target, WKRect area);
    void updateTextInputState(WebView*, const WTF::String& selectedText, const WTF::String& surroundingText, uint64_t inputMethodHints, bool isContentEditable, const WebCore::IntRect& cursorRect, const WebCore::IntRect& editorRect);

private:
    NIXViewClient m_client;
};

} // namespace WebKit

#endif // WebViewClient_h
