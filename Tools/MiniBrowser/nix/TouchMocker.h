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

#ifndef TouchMocker_h
#define TouchMocker_h

#include "NIXEvents.h"
#include "NIXView.h"
#include <map>

struct MockedTouchPoint : public NIXTouchPoint {
    // Means there is a mouse button pressed and the next mouse move
    // will update this touch point.
    bool selected;

    int windowX;
    int windowY;

    MockedTouchPoint();
};

class TouchMocker {
public:
    TouchMocker(WKViewRef);
    ~TouchMocker();

    bool handleMousePress(const NIXMouseEvent&, const WKPoint& windowPos);
    bool handleMouseRelease(const NIXMouseEvent&);
    bool handleMouseMove(const NIXMouseEvent&, const WKPoint& windowPos);
    bool handleKeyRelease(const NIXKeyEvent&);

    void paintTouchPoints(const WKSize& size);

private:
    void trackTouchPoint(WKEventMouseButton id, NIXTouchPointState state, const NIXMouseEvent& event, const WKPoint& windowPos);
    void updateTouchPointsState(NIXTouchPointState);
    void releaseTouchPoints(double timestamp);

    void sendCurrentTouchEvent(NIXTouchPointState state, double timestamp);

    void loadTouchPointTexture();

    WKViewRef m_view;
    NIXInputEventType m_touchType;
    typedef std::map<WKEventMouseButton, MockedTouchPoint> TouchMap;
    TouchMap m_touchPoints;

    unsigned m_touchTextureId;
};


#endif
