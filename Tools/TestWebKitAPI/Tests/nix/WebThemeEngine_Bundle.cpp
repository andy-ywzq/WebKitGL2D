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
#include "InjectedBundleTest.h"
#include <public/Platform.h>
#include <public/WebColor.h>
#include <public/WebRect.h>
#include <public/WebSize.h>
#include <public/WebThemeEngine.h>
#include <WebKit2/WKRetainPtr.h>

#include <cstdlib>

using namespace Nix;

namespace TestWebKitAPI {

static void fillRect(Canvas *canvas, const WebRect& rect, const WebColor& color)
{
    cairo_save(canvas);

   // Cairo uses a coordinate system not based on pixel coordinates, so
   // we need to add  0.5 to x and y coord or the line will stay between
   // two pixels instead of in the middle of a pixel.
    cairo_rectangle(canvas, rect.x, rect.y, rect.width, rect.height);
    cairo_set_source_rgb(canvas, color.r() / 255.0, color.g() / 255.0, color.b() / 255.0);
    cairo_fill(canvas);

    cairo_restore(canvas);
}

class TestThemeEngine : public WebThemeEngine {
    WebColor activeSelectionBackgroundColor() const { return WebColor(); }
    WebColor activeSelectionForegroundColor() const { return WebColor(); }
    WebColor inactiveSelectionBackgroundColor() const { return WebColor(); }
    WebColor inactiveSelectionForegroundColor() const { return WebColor(); }

    // List box selection colors
    WebColor activeListBoxSelectionBackgroundColor() const { return WebColor(); }
    WebColor activeListBoxSelectionForegroundColor() const { return WebColor(); }
    WebColor inactiveListBoxSelectionBackgroundColor() const { return WebColor(); }
    WebColor inactiveListBoxSelectionForegroundColor() const { return WebColor(); }

    WebColor activeTextSearchHighlightColor() const { return WebColor(); }
    WebColor inactiveTextSearchHighlightColor() const { return WebColor(); }

    WebColor focusRingColor() const { return WebColor(); }

    WebColor tapHighlightColor() const { return WebColor(); }

    void paintButton(Canvas* canvas, State, const WebRect& rect, const ButtonExtraParams&) const
    {
        WebColor red = 0xFFFF0000;
        fillRect(canvas, rect, red);
    }

    void paintTextField(Canvas*, State, const WebRect&) const
    {
    }

    WebSize getCheckboxSize() const
    {
        return WebSize();
    }
    void paintCheckbox(Canvas*, State, const WebRect&, const ButtonExtraParams&) const { }
    WebSize getRadioSize() const
    {
        return WebSize();
    }
    void paintRadio(Canvas*, State, const WebRect&, const ButtonExtraParams&) const { }
    void paintTextArea(Canvas*, State, const WebRect&) const { }
    void getMenuListPadding(int& paddingTop, int& paddingLeft, int& paddingBottom, int& paddingRight) const { }
    void paintMenuList(Canvas*, State, const WebRect&) const { }
    void paintProgressBar(Canvas* canvas, State, const WebRect& rect, const ProgressBarExtraParams& params) const
    {
        WebColor red = 0xFFFF0000;
        fillRect(canvas, rect, red);

        WebColor green = 0xFF00FF00;
        WebRect progressRect = rect;
        progressRect.width *= params.position;
        fillRect(canvas, progressRect, green);
    }
    double getAnimationRepeatIntervalForProgressBar() const
    {
        return 0;
    }
    double getAnimationDurationForProgressBar() const
    {
        return 0;
    }
    void getInnerSpinButtonPadding(int& paddingTop, int& paddingLeft, int& paddingBottom, int& paddingRight) const { }
    void paintInnerSpinButton(Canvas*, State, const WebRect&, const InnerSpinButtonExtraParams&) const { }
    void paintMeter(Canvas*, State, const WebRect&, const MeterExtraParams&) const { }
    void paintSliderTrack(Canvas*, State, const WebRect&) const { }
    void paintSliderThumb(Canvas*, State, const WebRect&) const { }
};

class TestThemeEnginePlatform : public Platform {
public:
    WebThemeEngine* themeEngine() {
        static TestThemeEngine testTheme;
        return &testTheme;
    }
};

class WebThemeEngineTest : public InjectedBundleTest {
public:
    WebThemeEngineTest(const std::string& identifier)
        : InjectedBundleTest(identifier)
    {
        static TestThemeEnginePlatform platform;
        Platform::initialize(&platform);
    }

    void didReceiveMessage(WKBundleRef bundle, WKStringRef messageName, WKTypeRef messageBody)
    {
        if (!WKStringIsEqualToUTF8CString(messageName, "Crash"))
            return;
        abort();
    }
};

static InjectedBundleTest::Register<WebThemeEngineTest> registrar("WebThemeEngineTest");

} // namespace TestWebKitAPI

