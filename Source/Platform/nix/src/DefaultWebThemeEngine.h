#ifndef DefaultWebThemeEngine_h
#define DefaultWebThemeEngine_h

#include "public/WebThemeEngine.h"

namespace Nix {

class DefaultWebThemeEngine : public WebThemeEngine {
public:
    // Text selection colors.
    virtual WebColor activeSelectionBackgroundColor() const;
    virtual WebColor activeSelectionForegroundColor() const;
    virtual WebColor inactiveSelectionBackgroundColor() const;
    virtual WebColor inactiveSelectionForegroundColor() const;

    // List box selection colors
    virtual WebColor activeListBoxSelectionBackgroundColor() const;
    virtual WebColor activeListBoxSelectionForegroundColor() const;
    virtual WebColor inactiveListBoxSelectionBackgroundColor() const;
    virtual WebColor inactiveListBoxSelectionForegroundColor() const;

    virtual WebColor activeTextSearchHighlightColor() const;
    virtual WebColor inactiveTextSearchHighlightColor() const;

    virtual WebColor focusRingColor() const;

    virtual WebColor tapHighlightColor() const;

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
};

}
#endif
