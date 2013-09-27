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

#include "TouchMocker.h"
#include "touchTexture.h"

#if WTF_USE_OPENGL_ES_2
#include <GLES2/gl2.h>
#else
#include <GL/gl.h>
#endif
#include <cassert>
#include <cstring>

// On single touches, every mouse interaction is used as touch event.
// Holding CTRL key means you're building a multi-touch event and each mouse,
// button represents a touch point. All touch points will be released when
// you release the CTRL key.

MockedTouchPoint::MockedTouchPoint()
{
    verticalRadius = 20;
    horizontalRadius = 20;
    rotationAngle = 0.0;
    pressure = 1.0;
    selected = false;
}

static inline bool isSingleTouch(const NIXMouseEvent& event)
{
    return !(event.modifiers & kNIXInputEventModifiersControlKey);
}

TouchMocker::TouchMocker(WKViewRef view)
    : m_view(view)
{
    loadTouchPointTexture();
}

TouchMocker::~TouchMocker()
{
    glDeleteTextures(1, &m_touchTextureId);
}

void TouchMocker::paintTouchPoints(const WKSize& size)
{
    
}

bool TouchMocker::handleMousePress(const NIXMouseEvent& event, const WKPoint& windowPos)
{
    NIXTouchPointState state = m_touchPoints.count(event.button) ? kNIXTouchPointStateTouchMoved : kNIXTouchPointStateTouchPressed;

    trackTouchPoint(event.button, state, event, windowPos);
    sendCurrentTouchEvent(state, event.timestamp);
    m_touchPoints[event.button].selected = true;
    return true;
}

bool TouchMocker::handleMouseRelease(const NIXMouseEvent& event)
{
    if (isSingleTouch(event))
        releaseTouchPoints(event.timestamp);
    else
        m_touchPoints[event.button].selected = false;
    return true;
}

bool TouchMocker::handleMouseMove(const NIXMouseEvent& event, const WKPoint& windowPos)
{
    if (m_touchPoints.empty())
        return false;

    TouchMap::const_iterator it = m_touchPoints.begin();
    for (; it != m_touchPoints.end(); ++it) {
        if (it->second.selected) {
            trackTouchPoint(it->first, kNIXTouchPointStateTouchMoved, event, windowPos);
            sendCurrentTouchEvent(kNIXTouchPointStateTouchMoved, event.timestamp);
        }
    }

    return true;
}

bool TouchMocker::handleKeyRelease(const NIXKeyEvent& event)
{
    if (event.key == kNIXKeyEventKey_Control) {
        releaseTouchPoints(event.timestamp);
        return true;
    }
    return false;
}

void TouchMocker::releaseTouchPoints(double timestamp)
{
    if (m_touchPoints.empty())
        return;

    // FIXME: When we have proper gesture recognition, this code should move away.
    NIXTouchPoint touch;

    if (m_touchPoints.size() == 1)
        // Keep track of the single touch we have so far to emit it as SingleTap afterwards.
        touch = m_touchPoints.begin()->second;

    updateTouchPointsState(kNIXTouchPointStateTouchReleased);
    sendCurrentTouchEvent(kNIXTouchPointStateTouchReleased, timestamp);

    m_touchPoints.clear();

}

void TouchMocker::updateTouchPointsState(NIXTouchPointState state)
{
    TouchMap::iterator it = m_touchPoints.begin();
    for (; it != m_touchPoints.end(); ++it)
        it->second.state = state;
}

void TouchMocker::trackTouchPoint(WKEventMouseButton id, NIXTouchPointState state, const NIXMouseEvent& event, const WKPoint& windowPos)
{
    // While we update some touch's state the others should be on stationary state.
    updateTouchPointsState(kNIXTouchPointStateTouchStationary);

    MockedTouchPoint& touch = m_touchPoints[id];
    touch.id = static_cast<unsigned>(id);
    touch.state = state;
    touch.x = event.x;
    touch.y = event.y;
    touch.windowX = windowPos.x;
    touch.windowY = windowPos.y;
    touch.globalX = event.globalX;
    touch.globalY = event.globalY;
}

void TouchMocker::sendCurrentTouchEvent(NIXTouchPointState state, double timestamp)
{
    NIXTouchEvent ev;

    memset(&ev, 0, sizeof(NIXTouchEvent));
    ev.timestamp = timestamp;

    switch (state) {
    case kNIXTouchPointStateTouchPressed:
        ev.type = kNIXInputEventTypeTouchStart;
        break;
    case kNIXTouchPointStateTouchMoved:
        ev.type = kNIXInputEventTypeTouchMove;
        break;
    case kNIXTouchPointStateTouchReleased:
        ev.type = kNIXInputEventTypeTouchEnd;
        break;
    }

    int i = 0;
    TouchMap::iterator it = m_touchPoints.begin();
    for (; it != m_touchPoints.end(); ++it, ++i)
        ev.touchPoints[i] = it->second;
    ev.numTouchPoints = m_touchPoints.size();

    NIXViewSendTouchEvent(m_view, &ev);
}

void TouchMocker::loadTouchPointTexture()
{
    // Texture data size, 64x64 RGBA
    assert(sizeof(textureData) == 64 * 64 * 4);
    glGenTextures(1, &m_touchTextureId);
    glBindTexture(GL_TEXTURE_2D, m_touchTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
}
