#ifndef WebRect_h
#define WebRect_h

#ifdef BUILDING_NIX__
#include "IntRect.h"
#endif

namespace Nix {

class WebRect {
public:
    int x;
    int y;
    int width;
    int height;

    WebRect(const WebRect& r)
        : x(r.x)
        , y(r.y)
        , width(r.width)
        , height(r.height)
    {
    }

#ifdef BUILDING_NIX__
    WebRect(const WebCore::IntRect& r)
        : x(r.x())
        , y(r.y())
        , width(r.width())
        , height(r.height())
    {
    }

    WebRect& operator=(const WebCore::IntRect& r)
    {
        x = r.x();
        y = r.y();
        width = r.width();
        height = r.height();
        return *this;
    }
#endif
};

}
#endif
