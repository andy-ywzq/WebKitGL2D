#ifndef Nix_Rect_h
#define Nix_Rect_h

#ifdef BUILDING_NIX__
#include "IntRect.h"
#endif

namespace Nix {

class Rect {
public:
    int x;
    int y;
    int width;
    int height;

    Rect(int x, int y, int width, int height)
        : x(x)
        , y(y)
        , width(width)
        , height(height)
    {
    }

    Rect(const Rect& r)
        : x(r.x)
        , y(r.y)
        , width(r.width)
        , height(r.height)
    {
    }

#ifdef BUILDING_NIX__
    Rect(const WebCore::IntRect& r)
        : x(r.x())
        , y(r.y())
        , width(r.width())
        , height(r.height())
    {
    }

    Rect& operator=(const WebCore::IntRect& r)
    {
        x = r.x();
        y = r.y();
        width = r.width();
        height = r.height();
        return *this;
    }
#endif
};

} // namespace Nix

#endif // Nix_Rect_h
