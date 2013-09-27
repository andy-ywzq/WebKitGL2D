/*
 * Copyright (C) 2013 University of Szeged
 * Copyright (C) 2013 Zoltan Herczeg
 * Copyright (C) 2013 Kristof Kosztyo
 * Copyright (C) 2013 Szilard Ledan
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
 * THIS SOFTWARE IS PROVIDED BY UNIVERSITY OF SZEGED ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL UNIVERSITY OF SZEGED OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "config.h"
#include "Path.h"

#include "AffineTransform.h"
#include "FloatPoint.h"
#include "FloatRect.h"
#include "FloatSize.h"
#include "NotImplemented.h"
#include "PathGL2D.h"
#include "TransformGL2D.h"
#include "TrapezoidBuilderGL2D.h"
#include "TrapezoidListGL2D.h"
#include "WindRule.h"
#include "wtf/MathExtras.h"

namespace WebCore {
namespace GL2D {

PathElement* PathData::alloc(int size)
{
    PathElement* next = reinterpret_cast<PathElement*>(m_arena->alloc(size));

    if (!m_firstElement)
        m_firstElement = next;

    if (m_lastElement)
        m_lastElement->setNext(next);
    m_lastElement = next;

    return next;
}

inline void PathData::moveTo(const FloatPoint& end)
{
    if (m_lastElement && m_lastElement->isMoveTo()) {
        m_lastElement->setEnd(end);
        return;
    }

    // Automatically close subpath.
    if (m_shapeFirstElement && m_shapeFirstElement->end() != m_lastElement->end())
        new (alloc(sizeof(AutoCloseSubpathElement))) AutoCloseSubpathElement(m_shapeFirstElement->end());

    m_shapeFirstElement = new (alloc(sizeof(MoveToElement))) MoveToElement(end);
}

inline void PathData::addLineTo(const FloatPoint& end)
{
    if (!m_lastElement || (!m_lastElement->isMoveTo() && m_lastElement->end() == end))
        return;

    appendBoundingBox(end);
    m_isEmpty = false;
    new (alloc(sizeof(LineToElement))) LineToElement(end);
}

inline void PathData::addQuadCurveTo(const FloatPoint& control, const FloatPoint& end)
{
    if (!m_lastElement)
        return;

    appendBoundingBox(control, end);
    m_isEmpty = false;
    new (alloc(sizeof(QuadCurveToElement))) QuadCurveToElement(control, end);
}

inline void PathData::addBezierCurveTo(const FloatPoint& control1, const FloatPoint& control2, const FloatPoint& end)
{
    if (!m_lastElement)
        return;

    appendBoundingBox(control1, control2, end);
    m_isEmpty = false;
    new (alloc(sizeof(CurveToElement))) CurveToElement(control1, control2, end);
}

void PathData::addArcTo(const FloatPoint& control, const FloatPoint& end, float radius)
{
    if (!m_lastElement)
        return;

    FloatPoint start(m_lastElement->end());

    FloatPoint delta1(start - control);
    FloatPoint delta2(end - control);
    float delta1Length = sqrtf(delta1.lengthSquared());
    float delta2Length = sqrtf(delta2.lengthSquared());

    // Normalized dot product.
    double cosPhi = delta1.dot(delta2) / (delta1Length * delta2Length);

    // All three points are on the same straight line (HTML5, 4.8.11.1.8).
    if (abs(cosPhi) >= 0.9999) {
        addLineTo(control);
        return;
    }

    float tangent = radius / tan(acos(cosPhi) / 2);
    float delta1Factor = tangent / delta1Length;
    FloatPoint arcStartPoint((control.x() + delta1Factor * delta1.x()), (control.y() + delta1Factor * delta1.y()));

    FloatPoint orthoStartPoint(delta1.y(), -delta1.x());
    float orthoStartPointLength = sqrt(orthoStartPoint.lengthSquared());
    float radiusFactor = radius / orthoStartPointLength;

    double cosAlpha = (orthoStartPoint.x() * delta2.x() + orthoStartPoint.y() * delta2.y()) / (orthoStartPointLength * delta2Length);
    if (cosAlpha < 0.f)
        orthoStartPoint = FloatPoint(-orthoStartPoint.x(), -orthoStartPoint.y());

    FloatPoint center((arcStartPoint.x() + radiusFactor * orthoStartPoint.x()), (arcStartPoint.y() + radiusFactor * orthoStartPoint.y()));

    // Calculate angles for addArc.
    orthoStartPoint = FloatPoint(-orthoStartPoint.x(), -orthoStartPoint.y());
    float startAngle = acos(orthoStartPoint.x() / orthoStartPointLength);
    if (orthoStartPoint.y() < 0.f)
        startAngle = 2 * piFloat - startAngle;

    bool anticlockwise = false;

    float delta2Factor = tangent / delta2Length;
    FloatPoint arcEndPoint((control.x() + delta2Factor * delta2.x()), (control.y() + delta2Factor * delta2.y()));
    FloatPoint orthoEndPoint(arcEndPoint - center);
    float orthoEndPointLength = sqrtf(orthoEndPoint.lengthSquared());
    float endAngle = acos(orthoEndPoint.x() / orthoEndPointLength);
    if (orthoEndPoint.y() < 0)
        endAngle = 2 * piFloat - endAngle;
    if ((startAngle > endAngle) && ((startAngle - endAngle) < piFloat))
        anticlockwise = true;
    if ((startAngle < endAngle) && ((endAngle - startAngle) > piFloat))
        anticlockwise = true;

    addArc(center, FloatSize(radius, radius), startAngle, endAngle, anticlockwise);
}

inline void PathData::closeSubpath()
{
    if (!m_shapeFirstElement)
        return;

    addLineTo(m_shapeFirstElement->end());
    m_shapeFirstElement = m_lastElement;
}

void PathData::addArc(const FloatPoint& center, const FloatSize& radius, float startAngle, float endAngle, bool anticlockwise)
{
    if(!m_lastElement)
        return;

    if (!radius.width() || !radius.height() || startAngle == endAngle) {
        addLineTo(center);
        return;
    }

    if (anticlockwise && startAngle - endAngle >= 2 * piFloat) {
        startAngle = fmod(startAngle, 2 * piFloat);
        endAngle = startAngle - 2 * piFloat;
    } else if(!anticlockwise && endAngle - startAngle >= 2 * piFloat) {
        startAngle = fmod(startAngle, 2 * piFloat);
        endAngle = startAngle + 2 * piFloat;
    } else {
        startAngle = fmod(startAngle, 2 * piFloat);
        if (startAngle < 2 * piFloat)
            startAngle += 2 * piFloat;

        endAngle = fmod(endAngle, 2 * piFloat);
        if (endAngle < 2 * piFloat)
            endAngle += 2 * piFloat;

        if (anticlockwise) {
            if (startAngle < endAngle)
                endAngle -= 2 * piFloat;
        } else {
            if (startAngle > endAngle)
                endAngle += 2 * piFloat;
        }
    }

    m_isEmpty = false;
    new (alloc(sizeof(ArcToElement))) ArcToElement(center, radius, startAngle, endAngle, anticlockwise);
}

void PathData::appendBoundingBox(const FloatPoint& end)
{
    if (m_lastElement && m_lastElement->isMoveTo())
        m_boundingBox.extend(m_lastElement->end());

    m_boundingBox.extend(end);
}

static inline float root(float a, float b, float sqrtD)
{
    return (-b + sqrtD) / (2 * a);
}

static inline void calculateRoots(float* t, float a, float b, float c)
{
    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0)
        return;

    if (!a) {
        t[0] = -c / b;
        return;
    }

    if (!discriminant) {
        t[0] = root(a, b, 0);
        return;
    }

    t[0] = root(a, b, sqrt(discriminant));
    t[1] = root(a, b, -sqrt(discriminant));
}

inline FloatPoint bezierCurve(float t, const FloatPoint& startPoint, const FloatPoint& controlPoint, const FloatPoint& endPoint)
{
    if (t > 1.0)
        t = 1.0;
    return FloatPoint(startPoint.x() * (1.0 - t) * (1.0 - t)
                      + 2.0 * controlPoint.x() * (1.0 - t) * t
                      + endPoint.x() * t * t
                    , startPoint.y() * (1.0 - t) * (1.0 - t)
                      + 2.0 * controlPoint.y() * (1.0 - t) * t
                      + endPoint.y() * t * t
                    );
}


static inline FloatRect bezierBoundingBox(const FloatPoint& startPoint, const FloatPoint& controlPoint, const FloatPoint& endPoint)
{
    FloatRect result = FloatRect(startPoint, endPoint - startPoint);

    float t = (startPoint.x() - controlPoint.x()) / (startPoint.x() - 2 * controlPoint.x() + endPoint.x());

    if (t >= 0 && t <= 1)
        result.extend(bezierCurve(t, startPoint, controlPoint, endPoint));

    t = (startPoint.y() - controlPoint.y()) / (startPoint.y() - 2 * controlPoint.y() + endPoint.y());

    if (t >= 0 && t <= 1)
        result.extend(bezierCurve(t, startPoint, controlPoint, endPoint));

    return result;
}

void PathData::appendBoundingBox(const FloatPoint& control, const FloatPoint& end)
{
    if (m_lastElement && m_lastElement->isMoveTo())
        m_boundingBox.extend(m_lastElement->end());

    m_boundingBox.unite(bezierBoundingBox(m_lastElement->end(), control, end));
}

inline FloatPoint bezierCurve(float t, const FloatPoint& startPoint, const FloatPoint& controlPoint1, const FloatPoint& controlPoint2, const FloatPoint& endPoint)
{
    if (t > 1.0)
        t = 1.0;
    return FloatPoint(startPoint.x() * (1.0 - t) * (1.0 - t)  * (1.0 - t)
                      + 3.0 * controlPoint1.x() * (1.0 - t) * (1.0 - t) * t
                      + 3.0 * controlPoint2.x() * (1.0 - t) * t * t
                      + endPoint.x() * t * t * t
                    , startPoint.y() * (1.0 - t) * (1.0 - t)  * (1.0 - t)
                      + 3.0 * controlPoint1.y() * (1.0 - t) * (1.0 - t)  * t
                      + 3.0 * controlPoint2.y() * (1.0 - t) * t * t
                      + endPoint.y() * t * t * t
                    );
}

static inline FloatRect bezierBoundingBox(const FloatPoint& startPoint, const FloatPoint& controlPoint1, const FloatPoint& controlPoint2, const FloatPoint& endPoint)
{
    FloatRect result = FloatRect(startPoint, endPoint - startPoint);

    float t[4] = {-1, -1, -1, -1};

    // The x roots.
    float a = -startPoint.x() + 3 * controlPoint1.x() - 3 * controlPoint2.x() + endPoint.x();
    float b = 2 * startPoint.x() - 4 * controlPoint1.x() + 2 * controlPoint2.x();
    float c = controlPoint1.x() - startPoint.x();


    calculateRoots(t, a, b, c);

    // The y roots.
    a = -startPoint.y() + 3 * controlPoint1.y() - 3 * controlPoint2.y() + endPoint.y();
    b = 2 * startPoint.y() - 4 * controlPoint1.y() + 2 * controlPoint2.y();
    c = controlPoint1.y() - startPoint.y();

    calculateRoots(t + 2, a, b, c);

    for (int i = 0; i < 4; i++) {
        if (t[i] >= 0 && t[i] <= 1)
            result.extend(bezierCurve(t[i], startPoint, controlPoint1, controlPoint2, endPoint));
    }

    return result;
}

void PathData::appendBoundingBox(const FloatPoint& control1, const FloatPoint& control2, const FloatPoint& end)
{
    if (m_lastElement && m_lastElement->isMoveTo())
        m_boundingBox.extend(m_lastElement->end());

    m_boundingBox.unite(bezierBoundingBox(m_lastElement->end(), control1, control2, end));
}

PassOwnPtr<TrapezoidList> PathData::fillTrapezoidList(const GL2D::ClipRect& clipRect, const GL2D::AffineTransform& transform, WindRule fillRule) const
{
    TrapezoidBuilder builder(this, clipRect, transform, fillRule);
    return builder.trapezoidList();
}

PassOwnPtr<TrapezoidList> PathData::strokeTrapezoidList(const GL2D::ClipRect& clipRect, const GL2D::AffineTransform& transform) const
{
    TrapezoidBuilder builder(this, clipRect, transform);
    return builder.trapezoidList();
}

void PathData::addRect(const FloatRect& rect)
{
    moveTo(rect.location());
    FloatPoint point(rect.x() + rect.width(), rect.y());
    addLineTo(point);
    point.setY(rect.y() + rect.height());
    addLineTo(point);
    point.setX(rect.x());
    addLineTo(point);
    closeSubpath();
}

PathData* PathData::clone()
{
    CRASH();
    return 0;
}

void PathData::transform(const WebCore::AffineTransform &webCoreTransform)
{
    PathElement* element = firstElement();
    GL2D::AffineTransform::Transform matrix = {
        webCoreTransform.a(), webCoreTransform.b(), webCoreTransform.c(),
        webCoreTransform.d(), webCoreTransform.e(), webCoreTransform.f()
    };
    GL2D::AffineTransform transform(matrix);

    while (element) {
        switch (element->type()) {
            case PathElement::MoveTo :
            case PathElement::CloseSubpath :
            case PathElement::LineTo : {
                element->setEnd(transform.apply(element->end()));
                break;
            }
            case PathElement::QuadCurveTo : {
                QuadCurveToElement* quadToElement = reinterpret_cast<QuadCurveToElement*>(element);
                quadToElement->setEnd(transform.apply(quadToElement->end()));
                quadToElement->setControl(transform.apply(quadToElement->control()));
                break;
            }
            case PathElement::CurveTo : {
                CurveToElement* curveToElement = reinterpret_cast<CurveToElement*>(element);
                curveToElement->setEnd(transform.apply(curveToElement->end()));
                curveToElement->setControl1(transform.apply(curveToElement->control1()));
                curveToElement->setControl2(transform.apply(curveToElement->control2()));
                break;
            }
            case PathElement::ArcTo : {
                ArcToElement* arcToElement = reinterpret_cast<ArcToElement*>(element);
                arcToElement->setEnd(transform.apply(arcToElement->end()));
                arcToElement->multiply(transform);
                break;
            }
        }

        element = element->next();
    }
}

} // namespace GL2D

Path::Path()
    : m_path(new GL2D::PathData())
{
}

Path::Path(const Path& other)
{
    m_path = other.m_path->clone();
}

Path::~Path()
{
    delete m_path;
}

Path& Path::operator=(const Path& other)
{
    m_path = other.m_path->clone();
}

bool Path::isEmpty() const
{
    return m_path->isEmpty();
}

bool Path::contains(const FloatPoint& point, WindRule rule) const
{
    notImplemented();
}

bool Path::hasCurrentPoint() const
{
    notImplemented();
}

FloatPoint Path::currentPoint() const
{
    notImplemented();
}

bool Path::strokeContains(StrokeStyleApplier* applier, const FloatPoint& point) const
{
    notImplemented();
}

FloatRect Path::boundingRect() const
{
    return m_path->boundingBox();
}

FloatRect Path::strokeBoundingRect(StrokeStyleApplier* applier) const
{
    notImplemented();
}

void Path::moveTo(const FloatPoint& point)
{
    m_path->moveTo(point);
}

void Path::addRect(const FloatRect& rect)
{
    m_path->addRect(rect);
}

void Path::addLineTo(const FloatPoint& point)
{
    m_path->addLineTo(point);
}

void Path::addQuadCurveTo(const FloatPoint& controlPoint, const FloatPoint& point)
{
    m_path->addQuadCurveTo(controlPoint, point);
}

void Path::addBezierCurveTo(const FloatPoint& p1, const FloatPoint& p2, const FloatPoint& ep)
{
    m_path->addBezierCurveTo(p1, p2, ep);
}

void Path::addEllipse(const FloatRect& rect)
{
    FloatSize radius(0.5 * rect.width(), 0.5 * rect.height());
    FloatPoint center(rect.x() + radius.width(), rect.y() + radius.height());

    m_path->addArc(center, radius, 0, 2 * piFloat, false);
}

void Path::addArc(const FloatPoint& p, float r, float sa, float ea, bool anticlockwise)
{
    m_path->addArc(p, FloatSize(r, r), sa, ea, anticlockwise);
}

void Path::addArcTo(const FloatPoint& control, const FloatPoint& end, float radius)
{
    m_path->addArcTo(control, end, radius);
}

void Path::apply(void* info, PathApplierFunction function) const
{

}

void Path::closeSubpath()
{
    m_path->closeSubpath();
}

void Path::clear()
{
    delete m_path;
    m_path = new GL2D::PathData();
}

void Path::transform(const AffineTransform& xform)
{
    m_path->transform(xform);
}

void Path::translate(const FloatSize& p)
{

}

}
