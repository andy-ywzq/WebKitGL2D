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

#ifndef RenderThemeNix_h
#define RenderThemeNix_h

#include "RenderTheme.h"

namespace WebCore {

class RenderThemeNix : public RenderTheme {
public:
    static PassRefPtr<RenderTheme> create();

    virtual ~RenderThemeNix();

    virtual void systemFont(int cssValueId, FontDescription&) const;

#if ENABLE(PROGRESS_ELEMENT)
    // Returns the repeat interval of the animation for the progress bar.
    virtual double animationRepeatIntervalForProgressBar(RenderProgress*) const;
    // Returns the duration of the animation for the progress bar.
    virtual double animationDurationForProgressBar(RenderProgress*) const;
#endif

#if ENABLE(METER_ELEMENT)
    virtual IntSize meterSizeForBounds(const RenderMeter*, const IntRect&) const;
    virtual bool supportsMeter(ControlPart) const;
#endif

protected:

    virtual bool paintButton(RenderObject*, const PaintInfo&, const IntRect&);
    virtual bool paintTextField(RenderObject*, const PaintInfo&, const IntRect&);
    virtual bool paintTextArea(RenderObject*, const PaintInfo&, const IntRect&);

    virtual bool paintCheckbox(RenderObject*, const PaintInfo&, const IntRect&);
    virtual void setCheckboxSize(RenderStyle*) const;

    virtual bool paintRadio(RenderObject*, const PaintInfo&, const IntRect&);
    virtual void setRadioSize(RenderStyle*) const;

    virtual bool paintMenuList(RenderObject*, const PaintInfo&, const IntRect&);
    virtual void adjustMenuListStyle(StyleResolver*, RenderStyle*, Element*) const;
    virtual bool paintMenuListButton(RenderObject* o, const PaintInfo& i, const IntRect& r) { return paintMenuList(o, i, r); }

    virtual void adjustInnerSpinButtonStyle(StyleResolver*, RenderStyle*, Element*) const;
    virtual bool paintInnerSpinButton(RenderObject*, const PaintInfo&, const IntRect&);

#if ENABLE(PROGRESS_ELEMENT)
    virtual void adjustProgressBarStyle(StyleResolver*, RenderStyle*, Element*) const;
    virtual bool paintProgressBar(RenderObject*, const PaintInfo&, const IntRect&) OVERRIDE;
#endif

#if ENABLE(METER_ELEMENT)
    virtual void adjustMeterStyle(StyleResolver*, RenderStyle*, Element*) const;
    virtual bool paintMeter(RenderObject*, const PaintInfo&, const IntRect&);
#endif

    virtual bool paintSliderTrack(RenderObject*, const PaintInfo&, const IntRect&);
    virtual void adjustSliderTrackStyle(StyleResolver*, RenderStyle*, Element*) const;

    virtual bool paintSliderThumb(RenderObject*, const PaintInfo&, const IntRect&);
    virtual void adjustSliderThumbStyle(StyleResolver*, RenderStyle*, Element*) const;

    virtual void adjustSliderThumbSize(RenderStyle*, Element*) const;

private:
    RenderThemeNix();
};

}

#endif // RenderThemeNix_h
