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
#include <public/Color.h>
#include <public/Platform.h>
#include <public/Rect.h>
#include <public/Size.h>
#include <public/ThemeEngine.h>
#include <WebKit2/WKRetainPtr.h>

#include <cstdlib>

using namespace Nix;

namespace TestWebKitAPI {

static void fillRect(Canvas *canvas, const Rect& rect, const Color& color)
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

class TestThemeEngine : public ThemeEngine {
    Color activeSelectionBackgroundColor() const OVERRIDE { return Color::black; }
    Color activeSelectionForegroundColor() const OVERRIDE { return Color::black; }
    Color inactiveSelectionBackgroundColor() const OVERRIDE { return Color::black; }
    Color inactiveSelectionForegroundColor() const OVERRIDE { return Color::black; }

    // List box selection colors
    Color activeListBoxSelectionBackgroundColor() const OVERRIDE { return Color::black; }
    Color activeListBoxSelectionForegroundColor() const OVERRIDE { return Color::black; }
    Color inactiveListBoxSelectionBackgroundColor() const OVERRIDE { return Color::black; }
    Color inactiveListBoxSelectionForegroundColor() const OVERRIDE { return Color::black; }

    Color activeTextSearchHighlightColor() const OVERRIDE { return Color::black; }
    Color inactiveTextSearchHighlightColor() const OVERRIDE { return Color::black; }

    Color focusRingColor() const OVERRIDE { return Color::black; }

    Color tapHighlightColor() const OVERRIDE { return Color::black; }

    void paintButton(Canvas* canvas, State, const Rect& rect, const ButtonExtraParams&) const OVERRIDE
    {
        fillRect(canvas, rect, Color::red);
    }

    void paintTextField(Canvas*, State, const Rect&) const OVERRIDE
    {
    }

    Size getCheckboxSize() const OVERRIDE
    {
        return Size();
    }
    void paintCheckbox(Canvas*, State, const Rect&, const ButtonExtraParams&) const OVERRIDE { }
    Size getRadioSize() const OVERRIDE
    {
        return Size();
    }
    void paintRadio(Canvas*, State, const Rect&, const ButtonExtraParams&) const OVERRIDE { }
    void paintTextArea(Canvas*, State, const Rect&) const OVERRIDE { }
    void getMenuListPadding(int& paddingTop, int& paddingLeft, int& paddingBottom, int& paddingRight) const OVERRIDE { }
    void paintMenuList(Canvas*, State, const Rect&) const OVERRIDE { }
    void paintProgressBar(Canvas* canvas, State, const Rect& rect, const ProgressBarExtraParams& params) const OVERRIDE
    {
        Color red = 0xFFFF0000;
        fillRect(canvas, rect, red);

        Color green = 0xFF00FF00;
        Rect progressRect = rect;
        progressRect.width *= params.position;
        fillRect(canvas, progressRect, green);
    }
    double getAnimationRepeatIntervalForProgressBar() const OVERRIDE
    {
        return 0;
    }
    double getAnimationDurationForProgressBar() const OVERRIDE
    {
        return 0;
    }
    void getInnerSpinButtonPadding(int& paddingTop, int& paddingLeft, int& paddingBottom, int& paddingRight) const OVERRIDE { }
    void paintInnerSpinButton(Canvas*, State, const Rect&, const InnerSpinButtonExtraParams&) const OVERRIDE { }
    void paintMeter(Canvas*, State, const Rect&, const MeterExtraParams&) const OVERRIDE { }
    void paintSliderTrack(Canvas*, State, const Rect&) const OVERRIDE { }
    void paintSliderThumb(Canvas*, State, const Rect&) const OVERRIDE { }
    virtual void paintMediaPlayButton(Canvas*, MediaPlayerState, const Rect&) const OVERRIDE { }
    virtual void paintMediaMuteButton(Canvas*, MediaPlayerState, const Rect&) const OVERRIDE { }
    virtual void paintMediaSeekBackButton(Canvas*, const Rect&) const OVERRIDE { }
    virtual void paintMediaSeekForwardButton(Canvas*, const Rect&) const OVERRIDE { }
    virtual void paintMediaRewindButton(Canvas*, const Rect&) const OVERRIDE { }

};

class TestThemeEnginePlatform : public Platform {
public:
    ThemeEngine* themeEngine() OVERRIDE
    {
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

    void didReceiveMessage(WKBundleRef bundle, WKStringRef messageName, WKTypeRef messageBody) OVERRIDE
    {
        if (!WKStringIsEqualToUTF8CString(messageName, "Crash"))
            return;
        abort();
    }
};

static InjectedBundleTest::Register<WebThemeEngineTest> registrar("WebThemeEngineTest");

} // namespace TestWebKitAPI

