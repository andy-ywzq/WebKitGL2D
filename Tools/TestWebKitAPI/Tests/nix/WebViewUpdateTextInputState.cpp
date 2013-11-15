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

#include "PlatformUtilities.h"
#include <WebKit2/WKContext.h>
#include <WebKit2/WKPage.h>
#include <WebKit2/WKRetainPtr.h>
#include <WebKit2/WKString.h>
#include "NIXView.h"

namespace TestWebKitAPI {

static bool didFinishLoad = false;
static bool didUpdateTextInputState = false;
static NIXTextInputState stateReceived;
static const WKRect invalidRectState = WKRectMake(0, 0, 0, 0);

static bool WKRectIsEqual(const WKRect& a, const WKRect& b)
{
    return a.origin.x == b.origin.x && a.origin.y == b.origin.y && a.size.width == b.size.width && a.size.height == b.size.height;
}

static void didFinishLoadForFrame(WKPageRef page, WKFrameRef, WKTypeRef, const void*)
{
    didFinishLoad = true;
}

static void updateTextInputState(WKViewRef, const NIXTextInputState* state, const void*)
{
    didUpdateTextInputState = true;

    memcpy(&stateReceived, state, sizeof(NIXTextInputState));
    WKRetain(stateReceived.surroundingText);
    WKRetain(stateReceived.submitLabel);
}

TEST(WebKitNix, WebViewUpdateTextInputState)
{
    memset(&stateReceived, 0, sizeof(stateReceived));
    WKRetainPtr<WKContextRef> context = adoptWK(WKContextCreate());
    WKRetainPtr<WKViewRef> view(AdoptWK, WKViewCreate(context.get(), 0));

    NIXViewClient nixViewClient;
    memset(&nixViewClient, 0, sizeof(NIXViewClient));
    nixViewClient.version = kNIXViewClientCurrentVersion;
    nixViewClient.updateTextInputState = updateTextInputState;
    NIXViewSetNixViewClient(view.get(), &nixViewClient);

    WKViewInitialize(view.get());

    WKPageLoaderClient pageLoaderClient;
    memset(&pageLoaderClient, 0, sizeof(WKPageLoaderClient));
    pageLoaderClient.version = kWKPageLoaderClientCurrentVersion;
    pageLoaderClient.didFinishLoadForFrame = didFinishLoadForFrame;
    WKPageSetPageLoaderClient(WKViewGetPage(view.get()), &pageLoaderClient);

    const WKSize size = WKSizeMake(100, 100);
    WKViewSetSize(view.get(), size);

    NIXMouseEvent nixEvent;
    memset(&nixEvent, 0, sizeof(NIXMouseEvent));
    nixEvent.type = kNIXInputEventTypeMouseDown;
    nixEvent.button = kWKEventMouseButtonLeftButton;
    nixEvent.x = 55;
    nixEvent.y = 55;
    nixEvent.globalX = 55;
    nixEvent.globalY = 55;
    nixEvent.clickCount = 1;
    nixEvent.modifiers = static_cast<NIXInputEventModifiers>(0);;
    nixEvent.timestamp = 0;

    // Simple test on content editable.
    WKRetainPtr<WKURLRef> editableContentUrl = adoptWK(Util::createURLForResource("../nix/single-tap-on-editable-content", "html"));
    WKPageLoadURL(WKViewGetPage(view.get()), editableContentUrl.get());
    Util::run(&didFinishLoad);
    NIXViewSendMouseEvent(view.get(), &nixEvent);
    nixEvent.type = kNIXInputEventTypeMouseUp;
    NIXViewSendMouseEvent(view.get(), &nixEvent);

    Util::run(&didUpdateTextInputState);

    ASSERT_TRUE(didFinishLoad);
    ASSERT_TRUE(didUpdateTextInputState);
    ASSERT_TRUE(stateReceived.isContentEditable);

    ASSERT_TRUE(WKStringIsEqualToUTF8CString(stateReceived.surroundingText, "foobar"));
    WKRelease(stateReceived.surroundingText);
    ASSERT_TRUE(WKStringIsEmpty(stateReceived.submitLabel));
    WKRelease(stateReceived.submitLabel);
    ASSERT_FALSE(stateReceived.inputMethodHints & NIXImhSensitiveData);
    ASSERT_TRUE(!WKRectIsEqual(stateReceived.cursorRect, invalidRectState));
    ASSERT_TRUE(!WKRectIsEqual(stateReceived.editorRect, invalidRectState));
    ASSERT_TRUE(!WKRectIsEqual(stateReceived.cursorRect, stateReceived.editorRect));

    // Test on a form field.
    didFinishLoad = false;
    memset(&stateReceived, 0, sizeof(stateReceived));

    editableContentUrl = adoptWK(Util::createURLForResource("../nix/single-tap-on-form-field", "html"));
    WKPageLoadURL(WKViewGetPage(view.get()), editableContentUrl.get());
    Util::run(&didFinishLoad);
    nixEvent.type = kNIXInputEventTypeMouseDown;
    NIXViewSendMouseEvent(view.get(), &nixEvent);
    nixEvent.type = kNIXInputEventTypeMouseUp;
    NIXViewSendMouseEvent(view.get(), &nixEvent);

    didUpdateTextInputState = false;
    Util::run(&didUpdateTextInputState);

    WKRelease(stateReceived.surroundingText);
    ASSERT_TRUE(didFinishLoad);
    ASSERT_TRUE(didUpdateTextInputState);
    ASSERT_TRUE(WKStringIsEqualToUTF8CString(stateReceived.submitLabel, "submitLabelValue"));
    WKRelease(stateReceived.submitLabel);
    ASSERT_TRUE(stateReceived.inputMethodHints & NIXImhSensitiveData);
}

} // TestWebKitAPI
