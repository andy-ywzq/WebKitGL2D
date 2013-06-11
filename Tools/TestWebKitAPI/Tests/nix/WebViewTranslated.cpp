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

#include "PageLoader.h"
#include "GLUtilities.h"
#include "NIXView.h"
#include <WebKit2/WKContext.h>
#include <WebKit2/WKRetainPtr.h>

namespace TestWebKitAPI {

TEST(WebKitNix, WebViewTranslated)
{
    const WKSize size = WKSizeMake(100, 100);
    ToolsNix::GLOffscreenBuffer offscreenBuffer(size.width, size.height);
    ASSERT_TRUE(offscreenBuffer.makeCurrent());

    WKRetainPtr<WKContextRef> context = adoptWK(WKContextCreate());
    WKRetainPtr<WKViewRef> view(AdoptWK, WKViewCreate(context.get(), 0));

    Util::ForceRepaintClient forceRepaintClient(view.get());
    forceRepaintClient.setClearColor(0, 0, 1, 1);

    const int translationDelta = 20;
    WKViewSetUserViewportTranslation(view.get(), translationDelta, translationDelta);

    WKViewInitialize(view.get());
    WKPageSetUseFixedLayout(WKViewGetPage(view.get()), true);
    WKViewSetSize(view.get(), size);

    glViewport(0, 0, size.width, size.height);
    forceRepaintClient.clear();

    Util::PageLoader loader(view.get());

    loader.waitForLoadURLAndRepaint("../nix/red-background");

    // Note that glReadPixels [0, 0] is at the bottom-left of the buffer, so a diagonal
    // line from the top-left, to the bottom-right, will have X values going up and Y
    // values going down. We are testing this diagonal here.

    // Original background.
    ToolsNix::RGBAPixel sample[int(size.width * size.height)];
    glReadPixels(0, 0, size.width, size.height, GL_RGBA, GL_UNSIGNED_BYTE, &sample);
    for (int x = 0, y = size.height - 1; x < translationDelta; x++, y--) {
        int index = y * size.height + x;
        EXPECT_EQ(ToolsNix::RGBAPixel::blue(), sample[index]) << "Error when checking for pixel (" << x << ", " << y << ")";
    }

    // Red background page.
    for (int x = translationDelta, y = size.height - translationDelta - 1; x < size.width; x++, y--) {
        int index = y * size.height + x;
        EXPECT_EQ(ToolsNix::RGBAPixel::red(), sample[index]) << "Error when checking for pixel (" << x << ", " << y << ")";
    }
}

} // TestWebKitAPI
