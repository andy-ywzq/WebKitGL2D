#include "config.h"
#include "PlatformUtilities.h"
#include "InjectedBundleTest.h"
#include <WebKit2/WKBundleFrame.h>

namespace TestWebKitAPI {

JSValueRef foo(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t, const JSValueRef[], JSValueRef*)
{
    WKBundlePostMessage(InjectedBundleController::shared().bundle(), Util::toWK("WebWorkerInitMessage").get(), 0);
    return JSValueMakeString(ctx, JSStringCreateWithUTF8CString("WebWorkerTitle"));
}

JSValueRef bar(JSContextRef ctx, JSObjectRef, JSObjectRef, size_t, const JSValueRef[], JSValueRef*)
{
    WKBundlePostMessage(InjectedBundleController::shared().bundle(), Util::toWK("WebWorkerSharedInitMessage").get(), 0);
    return JSValueMakeString(ctx, JSStringCreateWithUTF8CString("WebWorkerSharedTitle"));
}

void webWorkerInitialize(JSContextRef context, int isDedicated)
{
    if (isDedicated) {
        JSStringRef funcName = JSStringCreateWithUTF8CString("foo");
        JSObjectRef jsFunc = JSObjectMakeFunctionWithCallback(context, funcName, &foo);
        JSObjectSetProperty(context, JSContextGetGlobalObject(context), funcName, jsFunc, kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete, 0);
        JSStringRelease(funcName);
    } else {
        JSStringRef funcName = JSStringCreateWithUTF8CString("bar");
        JSObjectRef jsFunc = JSObjectMakeFunctionWithCallback(context, funcName, &bar);
        JSObjectSetProperty(context, JSContextGetGlobalObject(context), funcName, jsFunc, kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete, 0);
        JSStringRelease(funcName);
    }
}

void webWorkerTerminate(JSContextRef context, int isDedicated)
{
    if(isDedicated)
        WKBundlePostMessage(InjectedBundleController::shared().bundle(), Util::toWK("WebWorkerTerminateMessage").get(), 0);
    else
        WKBundlePostMessage(InjectedBundleController::shared().bundle(), Util::toWK("WebWorkerSharedTerminateMessage").get(), 0);
}

class WebWorkerTest : public InjectedBundleTest {
public:
    WebWorkerTest(const std::string& identifier)
        : InjectedBundleTest(identifier)
    {
    }

    virtual void initialize(WKBundleRef, WKTypeRef)
    {
        WKBundleFrameSetWorkerInitializeCallback(webWorkerInitialize);
        WKBundleFrameSetWorkerTerminateCallback(webWorkerTerminate);
    }
};

static InjectedBundleTest::Register<WebWorkerTest> registrar("WebWorkerTest");

} // namespace TestWebKitAPI
