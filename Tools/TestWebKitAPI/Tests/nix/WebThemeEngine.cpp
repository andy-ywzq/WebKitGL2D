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

#include "GLUtilities.h"
#include "PageLoader.h"
#include "PlatformUtilities.h"
#include "NIXView.h"
#include <WebKit2/WKContext.h>
#include <WebKit2/WKRetainPtr.h>

namespace TestWebKitAPI {

TEST(WebKitNix, WebThemeEngine)
{
    const WKSize size = WKSizeMake(100, 100);
    ToolsNix::GLOffscreenBuffer offscreenBuffer(size.width, size.height);
    ASSERT_TRUE(offscreenBuffer.makeCurrent());

    WKRetainPtr<WKContextRef> context = adoptWK(Util::createContextForInjectedBundleTest("WebThemeEngineTest"));
    WKRetainPtr<WKViewRef> view(AdoptWK, WKViewCreate(context.get(), 0));

    Util::ForceRepaintClient forceRepaintClient(view.get());
    forceRepaintClient.setClearColor(0, 0, 1, 1);

    WKViewInitialize(view.get());
    WKViewSetSize(view.get(), size);
    Util::PageLoader loader(view.get());

    glViewport(0, 0, size.width, size.height);

    ToolsNix::RGBAPixel green = ToolsNix::RGBAPixel::green();
    ToolsNix::RGBAPixel red = ToolsNix::RGBAPixel::red();

    forceRepaintClient.clear();
    ToolsNix::RGBAPixel clearedSample = offscreenBuffer.readPixelAtPoint(0, 0);
    EXPECT_EQ(ToolsNix::RGBAPixel::blue(), clearedSample);

    forceRepaintClient.clear();
    loader.waitForLoadURLAndRepaint("../nix/theme-button");
    // Testing Edges
    EXPECT_EQ(red, offscreenBuffer.readPixelAtPoint(0, 0));
    EXPECT_EQ(red, offscreenBuffer.readPixelAtPoint(size.width - 1, 0));
    EXPECT_EQ(red, offscreenBuffer.readPixelAtPoint(0, size.height - 1));
    EXPECT_EQ(red, offscreenBuffer.readPixelAtPoint(size.width - 1, size.height - 1));

    forceRepaintClient.clear();
    loader.waitForLoadURLAndRepaint("../nix/theme-progress");
    // Testing Corners
    EXPECT_EQ(green, offscreenBuffer.readPixelAtPoint(0, 0));
    EXPECT_EQ(green, offscreenBuffer.readPixelAtPoint(0, size.height - 1));
    EXPECT_EQ(red, offscreenBuffer.readPixelAtPoint(size.width - 1, 0));
    EXPECT_EQ(red, offscreenBuffer.readPixelAtPoint(size.width - 1, size.height - 1));
    // Testing Middle
    EXPECT_EQ(green, offscreenBuffer.readPixelAtPoint(size.width / 2, 0));
    EXPECT_EQ(green, offscreenBuffer.readPixelAtPoint(size.width / 2, size.height - 1));
    // Testing Boundary
    int boundary = size.width * 0.7;
    EXPECT_EQ(green, offscreenBuffer.readPixelAtPoint(boundary - 1, 0));
    EXPECT_EQ(green, offscreenBuffer.readPixelAtPoint(boundary - 1, size.height - 1));
    EXPECT_EQ(red, offscreenBuffer.readPixelAtPoint(boundary, 0));
    EXPECT_EQ(red, offscreenBuffer.readPixelAtPoint(boundary, size.height - 1));
}

} // TestWebKitAPI
