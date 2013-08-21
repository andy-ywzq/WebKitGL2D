/*
 * Copyright (C) 2013 Nokia Corporation and/or its subsidiary(-ies).
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
#include "PlatformWebView.h"
#include "Test.h"

#include <cstring>
#include <WebKit2/WKGeolocationManager.h>
#include <WebKit2/WKPage.h>
#include <WebKit2/WKRetainPtr.h>

namespace TestWebKitAPI {

static bool highAccuracyChanged;
static bool highAccuracyValue;
static const void* clientInfoValue;
static WKGeolocationManagerRef geolocationmanagerValue;

static void didChangeEnableHighAccuracy(WKGeolocationManagerRef geolocationManager, bool value, const void* clientInfo)
{
    highAccuracyChanged = true;
    geolocationmanagerValue = geolocationManager;
    highAccuracyValue = value;
    clientInfoValue = clientInfo;
}

static void decidePolicyForGeolocationPermissionRequest(WKPageRef, WKFrameRef, WKSecurityOriginRef, WKGeolocationPermissionRequestRef permissionRequest, const void*)
{
    WKGeolocationPermissionRequestAllow(permissionRequest);
}

TEST(WebKit2, Geolocation)
{
    highAccuracyChanged = false;
    highAccuracyValue = false;
    clientInfoValue = 0;
    geolocationmanagerValue = 0;

    WKRetainPtr<WKContextRef> context(AdoptWK, WKContextCreate());
    PlatformWebView webView(context.get());
    WKRetainPtr<WKGeolocationManagerRef> geolocationManager(AdoptWK, WKContextGetGeolocationManager(context.get()));

    WKGeolocationProvider provider;
    memset(&provider, 0, sizeof(WKGeolocationProvider));

    provider.version = kWKGeolocationProviderCurrentVersion;
    const void* dummyValue = reinterpret_cast<void*>(1);
    provider.clientInfo = dummyValue;
    provider.didChangeEnableHighAccuracy = &didChangeEnableHighAccuracy;
    WKGeolocationManagerSetProvider(geolocationManager.get(), &provider);

    WKPageUIClient uiClient;
    memset(&uiClient, 0, sizeof(WKPageUIClient));

    uiClient.version = kWKPageUIClientCurrentVersion;
    uiClient.decidePolicyForGeolocationPermissionRequest = &decidePolicyForGeolocationPermissionRequest;
    WKPageSetPageUIClient(webView.page(), &uiClient);

    WKRetainPtr<WKURLRef> url(AdoptWK, Util::createURLForResource("geolocation", "html"));
    WKPageLoadURL(webView.page(), url.get());

    Util::run(&highAccuracyChanged);

    EXPECT_TRUE(highAccuracyValue);
    EXPECT_TRUE(clientInfoValue == dummyValue);
    EXPECT_TRUE(geolocationManager.get() == geolocationmanagerValue);
}

}
