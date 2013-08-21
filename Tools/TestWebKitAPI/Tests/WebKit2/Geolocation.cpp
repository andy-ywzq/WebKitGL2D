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
