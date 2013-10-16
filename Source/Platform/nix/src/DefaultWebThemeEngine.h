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

#ifndef DefaultWebThemeEngine_h
#define DefaultWebThemeEngine_h

#include "public/ThemeEngine.h"

namespace Nix {

class DefaultWebThemeEngine FINAL : public ThemeEngine {
public:
    // Text selection colors.
    virtual Color activeSelectionBackgroundColor() const;
    virtual Color activeSelectionForegroundColor() const;
    virtual Color inactiveSelectionBackgroundColor() const;
    virtual Color inactiveSelectionForegroundColor() const;

    // List box selection colors
    virtual Color activeListBoxSelectionBackgroundColor() const;
    virtual Color activeListBoxSelectionForegroundColor() const;
    virtual Color inactiveListBoxSelectionBackgroundColor() const;
    virtual Color inactiveListBoxSelectionForegroundColor() const;

    virtual Color activeTextSearchHighlightColor() const;
    virtual Color inactiveTextSearchHighlightColor() const;

    virtual Color focusRingColor() const;

    virtual Color tapHighlightColor() const;

    virtual void paintButton(Canvas*, State, const Rect&, const ButtonExtraParams&) const;
    virtual void paintTextField(Canvas*, State, const Rect&) const;
    virtual void paintTextArea(Canvas*, State, const Rect&) const;
    virtual Size getCheckboxSize() const;
    virtual void paintCheckbox(Canvas*, State, const Rect&, const ButtonExtraParams&) const;
    virtual Size getRadioSize() const;
    virtual void paintRadio(Canvas*, State, const Rect&, const ButtonExtraParams&) const;
    virtual void paintMenuList(Canvas *, State, const Rect &) const;
    virtual void getMenuListPadding(int& paddingTop, int& paddingLeft, int& paddingBottom, int& paddingRight) const;
    virtual Size getProgressBarSize() const;
    virtual void paintProgressBar(Canvas*, State, const Rect&, const ProgressBarExtraParams&) const;
    virtual double getAnimationRepeatIntervalForProgressBar() const;
    virtual double getAnimationDurationForProgressBar() const;
    virtual void paintInnerSpinButton(Canvas *, State, const Rect &, const InnerSpinButtonExtraParams&) const;
    virtual void getInnerSpinButtonPadding(int& paddingTop, int& paddingLeft, int& paddingBottom, int& paddingRight) const;
    virtual void paintMeter(Canvas*, State, const Rect&, const MeterExtraParams&) const;
    virtual void paintSliderTrack(Canvas*, State, const Rect&) const;
    virtual void paintSliderThumb(Canvas*, State, const Rect&) const;

    // Media Player
    virtual void paintMediaPlayButton(Canvas*, MediaPlayerState, const Rect&) const;
    virtual void paintMediaMuteButton(Canvas*, MediaPlayerState, const Rect&) const;
    virtual void paintMediaSeekBackButton(Canvas*, const Rect&) const;
    virtual void paintMediaSeekForwardButton(Canvas*, const Rect&) const;
    virtual void paintMediaVolumeSliderContainer(Canvas*, State, const Rect&) const;
    virtual void paintMediaRewindButton(Canvas*, const Rect&) const;
};

} // namespace Nix

#endif // DefaultWebThemeEngine_h
