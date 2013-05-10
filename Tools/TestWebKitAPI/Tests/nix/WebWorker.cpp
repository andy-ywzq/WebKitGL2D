#include "config.h"

#include "PlatformUtilities.h"
#include <WebKit2/WKContext.h>
#include <WebKit2/WKPage.h>
#include <WebKit2/WKPreferencesPrivate.h>
#include "NIXView.h"
#include "NIXViewAutoPtr.h"

namespace TestWebKitAPI {

static bool initialized = false;
static bool terminated = false;
static bool changedTitle = false;
static bool initializedShared = false;
static bool terminatedShared = false;
static bool changedTitleShared = false;

static void didReceiveMessageFromInjectedBundle(WKContextRef, WKStringRef messageName, WKTypeRef, const void*)
{
    if (WKStringIsEqualToUTF8CString(messageName, "WebWorkerInitMessage"))
        initialized = true;
    else if(WKStringIsEqualToUTF8CString(messageName, "WebWorkerTerminateMessage"))
        terminated = true;
    else if (WKStringIsEqualToUTF8CString(messageName, "WebWorkerSharedInitMessage"))
        initializedShared = true;
    else if(WKStringIsEqualToUTF8CString(messageName, "WebWorkerSharedTerminateMessage"))
        terminatedShared = true;
}

static void didReceiveTitleForFrame(WKPageRef, WKStringRef title, WKFrameRef, WKTypeRef, const void *)
{
    if (WKStringIsEqualToUTF8CString(title, "WebWorkerTitle"))
        changedTitle = true;
    else if (WKStringIsEqualToUTF8CString(title, "WebWorkerSharedTitle"))
        changedTitleShared = true;
}


TEST(WebKitNix, WebWorker)
{
    // This test cover the binding between functions of JavaScript Workers and
    // functions defined at a previously loaded bundle. It verifies the two
    // types of Workers, dedicated and shared.

    WKRetainPtr<WKContextRef> context = adoptWK(Util::createContextForInjectedBundleTest("WebWorkerTest"));
    NIXViewAutoPtr view(WKViewCreate(context.get(), 0));

    WKViewInitialize(view.get());

    WKPageLoaderClient pageLoaderClient;
    memset(&pageLoaderClient, 0, sizeof(WKPageLoaderClient));
    pageLoaderClient.version = kWKPageLoaderClientCurrentVersion;
    pageLoaderClient.didReceiveTitleForFrame = didReceiveTitleForFrame;
    WKPageSetPageLoaderClient(WKViewGetPage(view.get()), &pageLoaderClient);

    WKContextInjectedBundleClient injectedBundleClient;
    memset(&injectedBundleClient, 0, sizeof(injectedBundleClient));
    injectedBundleClient.didReceiveMessageFromInjectedBundle = didReceiveMessageFromInjectedBundle;
    WKContextSetInjectedBundleClient(context.get(), &injectedBundleClient);

    WKPageGroupRef pageGroup = WKPageGetPageGroup(WKViewGetPage(view.get()));
    WKPreferencesRef preferences = WKPageGroupGetPreferences(pageGroup);
    WKPreferencesSetFileAccessFromFileURLsAllowed(preferences, true);

    WKRetainPtr<WKURLRef> url = adoptWK(Util::createURLForResource("../nix/WebWorkerTest", "html"));
    WKPageLoadURL(WKViewGetPage(view.get()), url.get());
    Util::run(&initialized);
    Util::run(&terminated);
    Util::run(&changedTitle);

    url = adoptWK(Util::createURLForResource("../nix/WebWorkerSharedTest", "html"));
    WKPageLoadURL(WKViewGetPage(view.get()), url.get());
    Util::run(&initializedShared);
    Util::run(&terminatedShared);
    Util::run(&changedTitleShared);
}

} // TestWebKitAPI
