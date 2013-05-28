#ifndef WebRect_h
#define WebRect_h

#if WEBKIT_IMPLEMENTATION
#include "IntRect.h"
#endif

namespace WebKit {

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

#if WEBKIT_IMPLEMENTATION
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
