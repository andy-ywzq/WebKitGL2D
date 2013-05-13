/*
 * Copyright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>
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
#include "WebViewNix.h"

#include "DrawingAreaProxyImpl.h"
#include "CoordinatedLayerTreeHostProxy.h"
#include "NativeWebGestureEvent.h"
#include "NativeWebKeyboardEvent.h"
#include "NativeWebMouseEvent.h"
#include "NativeWebWheelEvent.h"
#include "WebContext.h"
#include "WebPageGroup.h"
#include "WebPreferences.h"
#include <WebCore/CoordinatedGraphicsScene.h>
#include <WebCore/TextureMapperGL.h>

using namespace WebCore;

namespace WebKit {

WebView::WebView(WebContext* context, WebPageGroup* pageGroup)
    : m_focused(true)
    , m_visible(true)
    , m_active(true)
    , m_isSuspended(false)
    , m_scale(1.f)
    , m_opacity(1.f)
    , m_activeContextMenu(WebContextMenuProxyNix::create())
{
    m_webPageProxy = context->createWebPage(this, pageGroup);
    m_webPageProxy->pageGroup()->preferences()->setForceCompositingMode(true);
}

void WebView::setViewClient(const NIXViewClient* viewClient)
{
    m_viewClient.initialize(viewClient);
}

void WebView::initialize()
{
    m_webPageProxy->initializeWebPage();
    coordinatedGraphicsScene()->setActive(true);
}

void WebView::setTransparentBackground(bool value)
{
    m_webPageProxy->setDrawsTransparentBackground(value);
}

bool WebView::transparentBackground() const
{
    return m_webPageProxy->drawsTransparentBackground();
}

void WebView::setDrawBackground(bool value)
{
    m_webPageProxy->setDrawsBackground(value);
}

bool WebView::drawBackground() const
{
    return m_webPageProxy->drawsBackground();
}

void WebView::setScale(float scale)
{
    if (m_scale == scale)
        return;

    m_scale = scale;
    m_webPageProxy->scalePage(scale, roundedViewportPosition());
    if (!m_isSuspended)
        commitViewportChanges();
}

void WebView::setSize(const WKSize& size)
{
    IntSize newSize(size.width, size.height);

    if (m_size == newSize)
        return;

    m_size = newSize;
    commitViewportChanges();
}


IntPoint WebView::roundedViewportPosition() const
{
    return IntPoint(round(m_scrollPosition.x() * m_scale), round(m_scrollPosition.y() * m_scale));
}

void WebView::setScrollPosition(const WKPoint& position)
{
    if (m_scrollPosition.x() == position.x && m_scrollPosition.y() == position.y)
        return;

    FloatPoint trajectoryVector(position.x - m_scrollPosition.x(), position.y - m_scrollPosition.y());
    m_scrollPosition = FloatPoint(position.x, position.y);

    DrawingAreaProxy* drawingArea = m_webPageProxy->drawingArea();
    if (!drawingArea)
        return;

    drawingArea->setVisibleContentsRect(visibleRect(), trajectoryVector);
}

WKPoint WebView::userViewportToContents(WKPoint point)
{
    WKPoint result = point;
    userViewportToContentTransformation().map(point.x, point.y, result.x, result.y);
    return result;
}

bool WebView::isFocused() const
{
    return m_focused;
}

void WebView::setFocused(bool focused)
{
    m_focused = focused;
    m_webPageProxy->viewStateDidChange(WebPageProxy::ViewIsFocused);
}

bool WebView::isVisible() const
{
    return m_visible;
}

void WebView::setVisible(bool visible)
{
    m_visible = visible;
    m_webPageProxy->viewStateDidChange(WebPageProxy::ViewIsVisible);
}

bool WebView::isActive() const
{
    return m_active;
}

void WebView::setActive(bool active)
{
    m_active = active;
    m_webPageProxy->viewStateDidChange(WebPageProxy::ViewWindowIsActive);
}

PassOwnPtr<DrawingAreaProxy> WebView::createDrawingAreaProxy()
{
    return DrawingAreaProxyImpl::create(m_webPageProxy.get());
}

void WebView::setViewNeedsDisplay(const IntRect& rect)
{
    m_viewClient.viewNeedsDisplay(this, WKRectMake(rect.x(), rect.y(), rect.width(), rect.height()));
}

WKPageRef WebView::pageRef()
{
    return toAPI(m_webPageProxy.get());
}

void WebView::sendMouseEvent(const NIXMouseEvent& event)
{
    m_webPageProxy->handleMouseEvent(NativeWebMouseEvent(event, &m_lastCursorPosition));
}

void WebView::sendTouchEvent(const NIXTouchEvent& event)
{
    m_webPageProxy->handleTouchEvent(NativeWebTouchEvent(event));
}

void WebView::sendWheelEvent(const NIXWheelEvent& event)
{
    m_webPageProxy->handleWheelEvent(NativeWebWheelEvent(event));
}

void WebView::sendKeyEvent(const NIXKeyEvent& event)
{
    m_webPageProxy->handleKeyboardEvent(NativeWebKeyboardEvent(event));
}

void WebView::sendGestureEvent(const NIXGestureEvent& event)
{
    m_webPageProxy->handleGestureEvent(NativeWebGestureEvent(event));
}

CoordinatedGraphicsScene* WebView::coordinatedGraphicsScene()
{
    DrawingAreaProxy* drawingArea = m_webPageProxy->drawingArea();
    if (!drawingArea)
        return 0;

    CoordinatedLayerTreeHostProxy* coordinatorProxy = drawingArea->coordinatedLayerTreeHostProxy();
    if (!coordinatorProxy)
        return 0;

    CoordinatedGraphicsScene* scene = coordinatorProxy->coordinatedGraphicsScene();
    if (!scene)
        return 0;

    return scene;
}

FloatRect WebView::visibleRect() const
{
    WKSize vcSize = visibleContentsSize();
    FloatRect viewport(m_scrollPosition, FloatSize(vcSize.width, vcSize.height));
    FloatRect visibleRect(0, 0, m_contentsSize.width(), m_contentsSize.height());
    visibleRect.intersect(viewport);
    return visibleRect;
}

TransformationMatrix WebView::contentToUserViewportTransformation() const
{
    TransformationMatrix transform = m_userViewportTransformation;

    if (m_webPageProxy->useFixedLayout()) {
        // We switch the order between scale and translate here to make the resulting matrix hold
        // integer values for translation. If we did the natural order, the translation would be
        // the product between scroll position and scale and it could yield a float number for offset,
        // which would make the tiles blurry when painted. In the reverse order, we apply translation first
        // but it is a translation that already takes into account the scale, so the new matrix should be the
        // the same as in the natural order. In this case, though, we are free to do the rounding of the offset
        // and now will never translate into non-integer unit, producing now a slightly different matrix with
        // rounded translation.
        IntPoint roundedScrollPosition = roundedViewportPosition();
        transform.translate(-roundedScrollPosition.x(), -roundedScrollPosition.y());
    }

    transform.scale(m_scale);

    return transform;
}

void WebView::paintToCurrentGLContext()
{
    CoordinatedGraphicsScene* scene = coordinatedGraphicsScene();
    if (!scene)
        return;

    FloatRect viewport = m_userViewportTransformation.mapRect(FloatRect(FloatPoint(), m_size));

    scene->paintToCurrentGLContext(contentToUserViewportTransformation(), m_opacity, viewport);
}

void WebView::commitViewportChanges()
{
    DrawingAreaProxy* drawingArea = m_webPageProxy->drawingArea();

    if (!drawingArea)
        return;

    drawingArea->setSize(m_size, IntSize(), IntSize(m_scrollPosition.x(), m_scrollPosition.y()));
    drawingArea->setVisibleContentsRect(visibleRect(), FloatPoint());
}

void WebView::findZoomableAreaForPoint(const WKPoint& point, int horizontalRadius, int verticalRadius)
{
    IntPoint contentsPoint(point.x, point.y);
    IntSize touchSize(horizontalRadius * 2, verticalRadius * 2);
    m_webPageProxy->findZoomableAreaForPoint(contentsPoint, touchSize);
}

void WebView::processDidCrash()
{
    m_viewClient.webProcessCrashed(this, toCopiedAPI(m_webPageProxy->urlAtProcessExit()));
}

void WebView::didRelaunchProcess()
{
    coordinatedGraphicsScene()->setActive(true);
    m_viewClient.webProcessRelaunched(this);
}

void WebView::pageDidRequestScroll(const IntPoint& point)
{
    m_viewClient.pageDidRequestScroll(this, WKPointMake(point.x(), point.y()));
    // FIXME: It's not clear to me yet whether we should ask for display here or this is
    // at the wrong level and we should simply notify the client about this.
    setViewNeedsDisplay(IntRect(IntPoint(), m_size));
}

void WebView::didChangeContentsSize(const IntSize& size)
{
    m_contentsSize = size;
    m_viewClient.didChangeContentsSize(this, WKSizeMake(size.width(), size.height()));
    commitViewportChanges();
}

void WebView::didChangeViewportProperties(const WebCore::ViewportAttributes& viewportAttributes)
{
    m_viewClient.didChangeViewportAttributes(this, viewportAttributes.layoutSize.width(), viewportAttributes.layoutSize.height(), viewportAttributes.minimumScale, viewportAttributes.maximumScale, viewportAttributes.initialScale, !!viewportAttributes.userScalable);
}

void WebView::didFindZoomableArea(const IntPoint& target, const IntRect& area)
{
    m_viewClient.didFindZoomableArea(this, toAPI(target), toAPI(area));
}

void WebView::pageTransitionViewportReady()
{
    m_webPageProxy->commitPageTransitionViewport();
}

TransformationMatrix WebView::userViewportToContentTransformation() const
{
    return contentToUserViewportTransformation().inverse();
}

#if ENABLE(TOUCH_EVENTS)
void WebView::doneWithTouchEvent(const NativeWebTouchEvent& event, bool wasEventHandled)
{
    m_viewClient.doneWithTouchEvent(this, *event.nativeEvent(), wasEventHandled);
}
#endif

#if ENABLE(GESTURE_EVENTS)
void WebView::doneWithGestureEvent(const NativeWebGestureEvent& event, bool wasEventHandled)
{
    m_viewClient.doneWithGestureEvent(this, *event.nativeEvent(), wasEventHandled);
}
#endif

void WebView::updateTextInputState()
{
    const EditorState& editor = m_webPageProxy->editorState();
    bool isContentEditable = editor.isContentEditable;
    const IntRect& cursorRect = editor.cursorRect;
    const IntRect& editorRect = editor.editorRect;
    m_viewClient.updateTextInputState(this, isContentEditable, toAPI(cursorRect), toAPI(editorRect));
}

void WebView::suspendActiveDOMObjectsAndAnimations()
{
    m_webPageProxy->suspendActiveDOMObjectsAndAnimations();
    m_isSuspended = true;
}

void WebView::resumeActiveDOMObjectsAndAnimations()
{
    m_webPageProxy->resumeActiveDOMObjectsAndAnimations();
    m_isSuspended = false;
    commitViewportChanges();
}

bool WebView::isSuspended()
{
    return m_isSuspended;
}

} // namespace WebKit
