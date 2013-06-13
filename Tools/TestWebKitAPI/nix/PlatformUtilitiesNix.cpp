/*
 * Copyright (C) 2012 Igalia S.L.
 * Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "PlatformUtilities.h"
#include "NIXEvents.h"

#include "MainLoop.h"
#include <glib.h>

namespace TestWebKitAPI {
namespace Util {

static gboolean checkTestFinished(gpointer userData)
{
    bool* done = static_cast<bool*>(userData);

    if (*done)
        g_main_loop_quit(mainLoop());

    return !*done;
}

void run(bool* done)
{
    g_idle_add(checkTestFinished, done);
    g_main_loop_run(mainLoop());
}

void sleep(double seconds)
{
    g_usleep(seconds * 1000000);
}

WKURLRef createURLForResource(const char* resource, const char* extension)
{
    char url[PATH_MAX];

    snprintf(url, sizeof(url), "file://%s/%s.%s", TEST_WEBKIT2_RESOURCES_DIR, resource, extension);

    return WKURLCreateWithUTF8CString(url);
}

WKStringRef createInjectedBundlePath()
{
    return WKStringCreateWithUTF8CString(TEST_INJECTED_BUNDLE_PATH);
}

WKURLRef URLForNonExistentResource()
{
    return WKURLCreateWithUTF8CString("file:///does-not-exist.html");
}

bool isKeyDown(WKNativeEventPtr event)
{
    return static_cast<const NIXKeyEvent*>(event)->type == kNIXInputEventTypeKeyDown;
}

} // namespace Util

} // namespace TestWebKitAPI
