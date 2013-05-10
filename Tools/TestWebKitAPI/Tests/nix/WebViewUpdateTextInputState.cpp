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
#include "NIXViewAutoPtr.h"

namespace TestWebKitAPI {

static bool didFinishLoad = false;
static bool didUpdateTextInputState = false;
static bool contentEditableValue = false;
static bool isInPasswordFieldValue = true;
static bool isDoneWithSingleTapEvent = false;
static WKStringRef surroundingTextValue;
static const WKRect invalidRectState = WKRectMake(0, 0, 0, 0);
static WKRect editorRectState = invalidRectState;
static WKRect cursorRectState = invalidRectState;

static bool WKRectIsEqual(const WKRect& a, const WKRect& b)
{
    return a.origin.x == b.origin.x && a.origin.y == b.origin.y && a.size.width == b.size.width && a.size.height == b.size.height;
}

static void didFinishLoadForFrame(WKPageRef page, WKFrameRef, WKTypeRef, const void*)
{
    didFinishLoad = true;
}

static void updateTextInputState(WKViewRef, WKStringRef, WKStringRef surroundingText, uint64_t inputMethodHints, bool isContentEditable, WKRect cursorRect, WKRect editorRect, const void*)
{
    didUpdateTextInputState = true;
    contentEditableValue = isContentEditable;
    isInPasswordFieldValue = inputMethodHints & NixImhSensitiveData;
    surroundingTextValue = surroundingText;
    WKRetain(surroundingTextValue);
    cursorRectState = cursorRect;
    editorRectState = editorRect;
}

static void doneWithGestureEvent(WKViewRef, const NIXGestureEvent* event, bool, const void*)
{
    isDoneWithSingleTapEvent = event->type == kNIXInputEventTypeGestureSingleTap;
}

TEST(WebKitNix, WebViewWebProcessCrashed)
{
    WKRetainPtr<WKContextRef> context = adoptWK(WKContextCreate());

    NIXViewAutoPtr view(WKViewCreate(context.get(), 0));

    NIXViewClient nixViewClient;
    memset(&nixViewClient, 0, sizeof(NIXViewClient));
    nixViewClient.version = kNIXViewClientCurrentVersion;
    nixViewClient.updateTextInputState = updateTextInputState;
    nixViewClient.doneWithGestureEvent = doneWithGestureEvent;
    NIXViewSetNixViewClient(view.get(), &nixViewClient);

    WKViewInitialize(view.get());

    WKPageLoaderClient loaderClient;
    memset(&loaderClient, 0, sizeof(loaderClient));

    loaderClient.version = 0;
    loaderClient.didFinishLoadForFrame = didFinishLoadForFrame;
    WKPageSetPageLoaderClient(WKViewGetPage(view.get()), &loaderClient);

    const WKSize size = WKSizeMake(100, 100);
    WKViewSetSize(view.get(), size);

    WKRetainPtr<WKURLRef> editableContentUrl = adoptWK(Util::createURLForResource("../nix/single-tap-on-editable-content", "html"));
    WKPageLoadURL(WKViewGetPage(view.get()), editableContentUrl.get());
    Util::run(&didFinishLoad);

    NIXGestureEvent tapEvent;
    tapEvent.type = kNIXInputEventTypeGestureSingleTap;
    tapEvent.timestamp = 0;
    tapEvent.modifiers = static_cast<NIXInputEventModifiers>(0);
    tapEvent.x = 55;
    tapEvent.y = 55;
    tapEvent.globalX = 55;
    tapEvent.globalY = 55;
    tapEvent.width = 20;
    tapEvent.height = 20;
    tapEvent.deltaX = 0.0;
    tapEvent.deltaY = 0.0;
    NIXViewSendGestureEvent(view.get(), &tapEvent);
    Util::run(&isDoneWithSingleTapEvent);

    ASSERT_TRUE(didFinishLoad);
    ASSERT_TRUE(isDoneWithSingleTapEvent);
    ASSERT_TRUE(didUpdateTextInputState);
    ASSERT_TRUE(contentEditableValue);

    ASSERT_TRUE(WKStringIsEqualToUTF8CString(surroundingTextValue, "foobar"));
    ASSERT_FALSE(isInPasswordFieldValue);
    ASSERT_TRUE(!WKRectIsEqual(cursorRectState, invalidRectState));
    ASSERT_TRUE(!WKRectIsEqual(editorRectState, invalidRectState));
    ASSERT_TRUE(!WKRectIsEqual(cursorRectState, editorRectState));
}

} // TestWebKitAPI
