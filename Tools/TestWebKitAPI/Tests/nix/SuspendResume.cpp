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

TEST(WebKitNix, SuspendResumeAPI)
{
    // This test uses a page that changes its title - showing current time.
    // As the suspend method is called the page's changes are paused. The
    // animation continues as we call the resume method. During this process
    // there are checkpoints to ensure that changes are pausing/resuming.

    const WKSize size = WKSizeMake(150, 100);
    ToolsNix::GLOffscreenBuffer offscreenBuffer(size.width, size.height);
    ASSERT_TRUE(offscreenBuffer.makeCurrent());

    WKRetainPtr<WKContextRef> context = adoptWK(WKContextCreate());
    WKRetainPtr<WKViewRef> view(AdoptWK, WKViewCreate(context.get(), 0));

    Util::ForceRepaintClient forceRepaintClient(view.get());
    forceRepaintClient.setClearColor(0, 0, 1, 1);

    WKViewInitialize(view.get());
    WKViewSetSize(view.get(), size);

    glViewport(0, 0, size.width, size.height);
    forceRepaintClient.clear();

    Util::PageLoader loader(view.get());
    loader.waitForLoadURLAndRepaint("../nix/SuspendResume");

    size_t bufferSize = WKStringGetMaximumUTF8CStringSize(WKPageCopyTitle(WKViewGetPage(view.get())));
    char firstSampleBeforeSuspend[bufferSize];
    char secondSampleBeforeSuspend[bufferSize];
    char firstSampleAfterSuspend[bufferSize];
    char secondSampleAfterSuspend[bufferSize];
    char firstSampleAfterResume[bufferSize];

    WKStringGetUTF8CString(WKPageCopyTitle(WKViewGetPage(view.get())), firstSampleBeforeSuspend, bufferSize);

    // After collecting the first sample a repaint is needed to get viewport updated accordingly.
    // This proccess is repeated for each collected sample.
    Util::sleep(0.1);
    loader.forceRepaint();
    WKStringGetUTF8CString(WKPageCopyTitle(WKViewGetPage(view.get())), secondSampleBeforeSuspend, bufferSize);
    // The timer is ticking - two different samples.
    EXPECT_STRNE(firstSampleBeforeSuspend, secondSampleBeforeSuspend);

    // Force an update before suspending otherwise we can get same sample value after suspending
    // and the test becomes flacky.
    Util::sleep(0.1);
    loader.forceRepaint();
    WKViewSuspendActiveDOMObjectsAndAnimations(view.get());
    Util::sleep(0.1);
    loader.forceRepaint();
    WKStringGetUTF8CString(WKPageCopyTitle(WKViewGetPage(view.get())), firstSampleAfterSuspend, bufferSize);
    // The timer is paused - still two different samples.
    EXPECT_STRNE(secondSampleBeforeSuspend, firstSampleAfterSuspend);

    Util::sleep(0.1);
    loader.forceRepaint();
    WKStringGetUTF8CString(WKPageCopyTitle(WKViewGetPage(view.get())), secondSampleAfterSuspend, bufferSize);
    // The timer is paused - two samples collected while paused so they are equal.
    EXPECT_STREQ(firstSampleAfterSuspend, secondSampleAfterSuspend);

    WKViewResumeActiveDOMObjectsAndAnimations(view.get());
    Util::sleep(0.1);
    loader.forceRepaint();
    WKStringGetUTF8CString(WKPageCopyTitle(WKViewGetPage(view.get())), firstSampleAfterResume, bufferSize);
    // The timer is ticking again - two different samples.
    EXPECT_STRNE(secondSampleAfterSuspend, firstSampleAfterResume);
}

} // TestWebKitAPI
