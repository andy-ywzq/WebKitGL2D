add_custom_target(forwarding-headersEflForTestWebKitAPI
    COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl ${WEBKIT2_DIR} ${DERIVED_SOURCES_WEBKIT2_DIR}/include efl
    COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl ${WEBKIT2_DIR} ${DERIVED_SOURCES_WEBKIT2_DIR}/include CoordinatedGraphics
    COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl ${TESTWEBKITAPI_DIR} ${DERIVED_SOURCES_WEBKIT2_DIR}/include efl
)
set(ForwardingHeadersForTestWebKitAPI_NAME forwarding-headersEflForTestWebKitAPI)

add_custom_target(forwarding-headersSoupForTestWebKitAPI
    COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl ${WEBKIT2_DIR} ${DERIVED_SOURCES_WEBKIT2_DIR}/include soup
    COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl ${TESTWEBKITAPI_DIR} ${DERIVED_SOURCES_WEBKIT2_DIR}/include soup
)
set(ForwardingNetworkHeadersForTestWebKitAPI_NAME forwarding-headersSoupForTestWebKitAPI)

include_directories(
    ${WEBKIT2_DIR}/UIProcess/API/C/CoordinatedGraphics
    ${WEBKIT2_DIR}/UIProcess/API/C/soup
    ${WEBKIT2_DIR}/UIProcess/API/C/efl
    ${WEBKIT2_DIR}/UIProcess/API/efl
    ${ECORE_EVAS_INCLUDE_DIRS}
    ${ECORE_INCLUDE_DIRS}
    ${EINA_INCLUDE_DIRS}
    ${EO_INCLUDE_DIRS}
    ${EVAS_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
    ${LIBSOUP_INCLUDE_DIRS}
)

set(test_main_SOURCES
    ${TESTWEBKITAPI_DIR}/efl/main.cpp
)

list(APPEND TestWebKitAPIInjectedBundle_SOURCES
    ${TESTWEBKITAPI_DIR}/efl/InjectedBundleController.cpp
    ${TESTWEBKITAPI_DIR}/efl/PlatformUtilities.cpp

    # In here we list the bundles that are used by our specific WK2 API Tests
    ${TESTWEBKITAPI_DIR}/Tests/WebKit2/efl/WKViewClientWebProcessCallbacks_Bundle.cpp
)

list(APPEND TestWebKitAPIBase_SOURCES
    ${TESTWEBKITAPI_DIR}/efl/PlatformUtilities.cpp
    ${TESTWEBKITAPI_DIR}/efl/PlatformWebView.cpp
)

# The list below works like a test expectation. Tests in the
# test_{webkit2_api|webcore}_BINARIES list are added to the test runner and
# tried on the bots on every build. Tests in test_{webkit2_api|webcore}_BINARIES
# are compiled and suffixed with fail and skipped from the test runner.
#
# Make sure that the tests are passing on both Debug and
# Release builds before adding it to test_{webkit2_api|webcore}_BINARIES.

set(test_webcore_BINARIES
    LayoutUnit
    KURL
)

set(webkit2TestList
    WebKit2/AboutBlankLoad
    WebKit2/CookieManager
    WebKit2/DOMWindowExtensionNoCache
    WebKit2/DocumentStartUserScriptAlertCrash
    WebKit2/EvaluateJavaScript
    WebKit2/FailedLoad
    WebKit2/Find
    WebKit2/ForceRepaint
    WebKit2/FrameMIMETypeHTML
    WebKit2/FrameMIMETypePNG
    WebKit2/GetInjectedBundleInitializationUserDataCallback
    WebKit2/HitTestResultNodeHandle
    WebKit2/InjectedBundleBasic
    WebKit2/InjectedBundleFrameHitTest
    WebKit2/InjectedBundleInitializationUserDataCallbackWins
    WebKit2/LoadAlternateHTMLStringWithNonDirectoryURL
    WebKit2/LoadCanceledNoServerRedirectCallback
    WebKit2/LoadPageOnCrash
    WebKit2/MouseMoveAfterCrash
    WebKit2/ReloadPageAfterCrash
    WebKit2/ResizeWindowAfterCrash
    WebKit2/NewFirstVisuallyNonEmptyLayout
    WebKit2/NewFirstVisuallyNonEmptyLayoutFails
    WebKit2/NewFirstVisuallyNonEmptyLayoutForImages
    WebKit2/PageLoadBasic
    WebKit2/PageLoadDidChangeLocationWithinPageForFrame
    WebKit2/ParentFrame
    WebKit2/PreventEmptyUserAgent
    WebKit2/PrivateBrowsingPushStateNoHistoryCallback
    WebKit2/UserMessage
    WebKit2/WKConnection
    WebKit2/WKPreferences
    WebKit2/WKString
    WebKit2/WKStringJSString
    WebKit2/WKURL
    WebKit2/WillSendSubmitEvent
    WebKit2/efl/WKViewClientWebProcessCallbacks
)

# Seccomp filters is an internal API and its symbols
# are not (and should not) be exposed by default. We
# can only test it when building shared core.
if (ENABLE_SECCOMP_FILTERS AND SHARED_CORE)
    list(APPEND webkit2TestList
        WebKit2/SeccompFilters
    )
endif ()

set(webkit2FailTestList
    WebKit2/CanHandleRequest
    WebKit2/DOMWindowExtensionBasic
    WebKit2/DownloadDecideDestinationCrash
    WebKit2/NewFirstVisuallyNonEmptyLayoutFrames
    WebKit2/RestoreSessionStateContainingFormData
    WebKit2/ShouldGoToBackForwardListItem
    WebKit2/WKPageGetScaleFactorNotZero
)

# Tests disabled because of missing features on the test harness:
#
#   WebKit2/ResponsivenessTimerDoesntFireEarly
#   WebKit2/SpacebarScrolling
