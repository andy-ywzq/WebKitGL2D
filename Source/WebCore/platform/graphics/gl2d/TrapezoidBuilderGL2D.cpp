/*
 * Copyright (C) 2013 University of Szeged
 * Copyright (C) 2013 Zoltan Herczeg
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
#include "TrapezoidBuilderGL2D.h"

#include "FloatPoint.h"
#include "FloatRect.h"
#include "FloatSize.h"
#include "PathGL2D.h"
#include "TrapezoidListGL2D.h"
#include "WindRule.h"
#include "wtf/MathExtras.h"
#include <wtf/OwnPtr.h>
#include <wtf/PassOwnPtr.h>

namespace WebCore {
namespace GL2D {

bool TrapezoidBuilder::ScanStripe::addOrCombineTrapezoid(Arena *arena, float *result, TrapezoidBuilder::ScanStripe* previousScanStripe)
{
    TrapezoidBuilder::Trapezoid* trapezoid = TrapezoidBuilder::Trapezoid::create(arena);

    trapezoid->fill(result);
    trapezoid->setNext(m_trapezoids);
    m_trapezoids = trapezoid;

    if (!previousScanStripe)
        return true;

    TrapezoidBuilder::Trapezoid* current = previousScanStripe->trapezoids();
    while (current) {
        if (trapezoid->isCombined(current)) {
            if (current->top())
                current = current->top();
            current->trapezoid()[3] = trapezoid->trapezoid()[3];
            current->trapezoid()[4] = trapezoid->trapezoid()[4];
            current->trapezoid()[5] = trapezoid->trapezoid()[5];
            trapezoid->setTop(current);
            return false;
        }
        current = current->next();
    }
    return true;
}

inline void TrapezoidBuilder::trapezoidIteratorInit()
{
    m_currentScanStripe = m_scanStripes;
    m_previousScanStripe = 0;
    m_currentLine = m_currentScanStripe ? m_currentScanStripe->lines() : 0;
}

bool TrapezoidBuilder::trapezoidIteratorNext(float* result)
{
searchNext:

    if ((!m_currentScanStripe) || (!m_currentScanStripe->next()))
        return false;

    int fill = 0;
    result[0] = m_currentScanStripe->y() / static_cast<float>(antiAliasing);
    result[3] = m_currentScanStripe->next()->y() / static_cast<float>(antiAliasing);

    Line* firstLine;
    if (m_currentLine) {
        firstLine = m_currentLine;
        fill += m_currentLine->direction();
        m_currentLine = m_currentLine->next();
    }

    if (!m_currentLine) {
        m_previousScanStripe = m_currentScanStripe;
        m_currentScanStripe = m_currentScanStripe->next();
        m_currentLine = m_currentScanStripe->lines();
        fill = 0;
        goto searchNext;
    }

    Line* targetLine = m_currentLine;
    fill += m_currentLine->direction();

    if (m_fillRule == RULE_NONZERO) {
        while (m_currentLine->next()) {
            if (fill == 0) {
                if (!((m_currentLine->x0Rounded() == m_currentLine->next()->x0Rounded()) &&
                    (m_currentLine->x1Rounded() == m_currentLine->next()->x1Rounded())))
                {
                    m_currentLine = m_currentLine->next();
                    break;
                }
            }

            m_currentLine = m_currentLine->next();
            if (m_currentLine) {
                targetLine = m_currentLine;
                fill += m_currentLine->direction();
            }
        }
    } else {
        fill = (fill == 0);
        m_currentLine = m_currentLine->next();
    }

    result[1] = firstLine->x0Precise() / static_cast<float>(antiAliasing);
    result[4] = firstLine->x1Precise() / static_cast<float>(antiAliasing);

    result[2] = targetLine->x0Precise() / static_cast<float>(antiAliasing);
    result[5] = targetLine->x1Precise() / static_cast<float>(antiAliasing);

    result[6] = firstLine->slope();
    result[7] = targetLine->slope();

    return true;
}

PassOwnPtr<TrapezoidList> TrapezoidBuilder::trapezoidList()
{
    // Convert path to scanstripes.
    PathElement* element = m_path->firstElement();
    FloatPoint start;

    while (element) {
        FloatPoint end = m_transform.apply(element->end());

        switch (element->type()) {
            case PathElement::CloseSubpath :
            case PathElement::LineTo : {
                insertLine(start, end);
                break;
            }
            case PathElement::QuadCurveTo : {
                QuadCurveToElement* quadToElement = reinterpret_cast<QuadCurveToElement*>(element);
                insertQuadCurve(start, m_transform.apply(quadToElement->control()), end);
                break;
            }
            case PathElement::CurveTo : {
                CurveToElement* curveToElement = reinterpret_cast<CurveToElement*>(element);
                insertCubeCurve(start, m_transform.apply(curveToElement->control1()), m_transform.apply(curveToElement->control2()), end);
                break;
            }
            case PathElement::ArcTo : {
                ArcToElement* arcToElement = reinterpret_cast<ArcToElement*>(element);
                insertArc(start, *arcToElement);
                break;
            }
        }

        start = end;
        element = element->next();
    }

    // Convert scanstripes to trapezoids.
    int trapezoidCount = 0;
    float result[8];

    int minX = m_clipRight;
    int minY = m_clipTop / antiAliasing;
    int maxX = m_clipLeft;
    int maxY = m_clipBottom / antiAliasing;

    trapezoidIteratorInit();
    while (trapezoidIteratorNext(result)) {
        int left1 = floor(result[1]);
        int left2 = floor(result[4]);
        int min = left1 <= left2 ? left1 : left2;

        int right1 = ceil(result[2]);
        int right2 = ceil(result[5]);
        int max = right1 >= right2 ? right1 : right2;

        if (max <= m_clipLeft || min >= m_clipRight)
            continue;

        if (min < m_clipLeft)
            min = m_clipLeft;
        if (minX > min)
            minX = min;
        if (max > m_clipRight)
            max = m_clipRight;
        if (maxX < max)
            maxX = max;

        min = floor(result[0]);
        if (minY > min)
            minY = min;
        max = ceil(result[3]);
        if (maxY < max)
            maxY = max;

        if (m_currentScanStripe->addOrCombineTrapezoid(m_arena.get(), result, m_previousScanStripe))
            ++trapezoidCount;
    }

    if (!trapezoidCount) {
        IntRect boundingBox(m_clipLeft, m_clipBottom, 0, 0);
        OwnPtr<TrapezoidList> trapezoidList = TrapezoidList::create(trapezoidCount, boundingBox);
        return trapezoidList.release();
    }

    ASSERT(maxX >= minX && maxY >= minY);
    IntRect boundingBox(minX, minY, maxX - minX, maxY - minY);
    OwnPtr<TrapezoidList> trapezoidList = TrapezoidList::create(trapezoidCount, boundingBox);

    m_currentScanStripe = m_scanStripes;
    float x = minX;
    float y = minY;
    while (m_currentScanStripe) {
        m_currentTrapezoid = m_currentScanStripe->trapezoids();
        while (m_currentTrapezoid) {
            if (!m_currentTrapezoid->top()) {
                TrapezoidList::Trapezoid& trapezoid = m_currentTrapezoid->trapezoid();
                trapezoid[0] -= y;
                trapezoid[1] -= x;
                trapezoid[2] -= x;
                trapezoid[3] -= y;
                trapezoid[4] -= x;
                trapezoid[5] -= x;
                trapezoidList.get()->insertTrapezoid(m_currentTrapezoid->trapezoid());
            }
            m_currentTrapezoid = m_currentTrapezoid->next();
        }
        m_currentScanStripe = m_currentScanStripe->next();
    }

    return trapezoidList.release();
}

TrapezoidBuilder::ScanStripe* TrapezoidBuilder::splitScanStripe(TrapezoidBuilder::ScanStripe* scanStripe, int y)
{
    ScanStripe* newScanStripe = createScanStripe(y);

    int deltaY = y - scanStripe->y();
    Line* line = scanStripe->lines();
    Line* lastLine = 0;

    while (line) {
        float x0 = line->slope() * deltaY + line->x0Precise();
        Line *newLine = Line::create(m_arena.get(), x0, line->x1Precise(), line->slope(), line->direction());
        if (lastLine)
            lastLine->setNext(newLine);
        else
            newScanStripe->m_lines = newLine;
        lastLine = newLine;
        line->setX1(x0);
        line = line->next();
    }

    newScanStripe->setNext(scanStripe->next());
    scanStripe->setNext(newScanStripe);

    return newScanStripe;
}

static inline float calculateX(float y, float dxdy, float x0, float y0)
{
    return dxdy * (y - y0) + x0;
}

void TrapezoidBuilder::insertLine(FloatPoint start, FloatPoint end)
{
    int yDirection = 1;

    if (start.y() > end.y()) {
        FloatPoint temp(start);
        start = end;
        end = temp;
        yDirection = -1;
    }

    start.scale(antiAliasing, antiAliasing);
    end.scale(antiAliasing, antiAliasing);

    int yStart = floor(start.y());
    int yEnd = floor(end.y());

    // Discard empty lines.
    if (yEnd <= m_clipBottom || yStart >= m_clipTop)
        return;

    if (yStart < m_clipBottom)
        yStart = m_clipBottom;

    if (yEnd > m_clipTop)
        yEnd = m_clipTop;

    // Discard horizontal lines.
    if (yStart == yEnd)
        return;

    float slope = ((end.x() - start.x()) / (end.y() - start.y()));
    float x0 = calculateX(yStart, slope, start.x(), start.y());

    ScanStripe* scanStripe = m_scanStripes;

    if (!scanStripe) {
        m_scanStripes = createScanStripe(yStart);
        float x1 = calculateX(yEnd, slope, start.x(), start.y());
        m_scanStripes->addLine(Line::create(m_arena.get(), x0, x1, slope, yDirection));
        m_scanStripes->setNext(createScanStripe(yEnd));
        return;
    }

    if (yStart < scanStripe->y()) {
        scanStripe = createScanStripe(yStart);
        scanStripe->setNext(m_scanStripes);
        m_scanStripes = scanStripe;
    } else {
        // Search current scanstripe.
        while ((scanStripe->next()) && (scanStripe->next()->y() <= yStart))
            scanStripe = scanStripe->next();

        if (yStart > scanStripe->y()) {
            if (scanStripe->next())
                scanStripe = splitScanStripe(scanStripe, yStart);
            else {
                scanStripe->setNext(createScanStripe(yStart));
                scanStripe = scanStripe->next();
            }
        }
    }

    float x1;
    Line* newLine = 0;
    while (scanStripe->y() < yEnd) {
        if (!newLine) {
            if (!scanStripe->next())
                scanStripe->setNext(createScanStripe(yEnd));
            else if (scanStripe->next()->y() > yEnd)
                splitScanStripe(scanStripe, yEnd);

            x1 = calculateX(scanStripe->next()->y(), slope, start.x(), start.y());
            newLine = Line::create(m_arena.get(), x0, x1, slope, yDirection);
        }

        int y = scanStripe->addLine(newLine);

        if (y >= 0) {
            y += scanStripe->y();
            ASSERT(y > scanStripe->y() && y < scanStripe->next()->y());
            splitScanStripe(scanStripe, y);
            x1 = calculateX(y, slope, start.x(), start.y());
            newLine->setX1(x1);
        } else {
            x0 = x1;
            scanStripe = scanStripe->next();
            newLine = 0;
        }
    }
}

void TrapezoidBuilder::insertQuadCurve(const FloatPoint& start, const FloatPoint& control, const FloatPoint& end)
{
    insertCubeCurve(start, FloatPoint(2 / 3 * (control.x() - start.x()) + start.x(), 2 / 3 * (control.y() - start.y()) + start.y()), FloatPoint(2 / 3 * (control.x() - end.x()) + end.x(), 2 / 3 * (control.y() - end.y()) + end.y()), end);
}

static inline float abs(const float &t) { return (t >= 0) ? t : -t; }

static inline bool isCurveLinear(FloatPoint* p)
{
    float x0 = p[0].x();
    float y0 = p[0].y();
    float x3 = p[3].x();
    float y3 = p[3].y();

    float x1 = p[1].x();
    float y1 = p[1].y();
    float dt1 = abs((x3 - x0) * (y0 - y1) - (x0 - x1) * (y3 - y0));

    float x2 = p[2].x();
    float y2 = p[2].y();
    float dt2 = abs((x3 - x0) * (y0 - y2) - (x0 - x2) * (y3 - y0));

    if (dt1 > dt2)
        dt2 = dt1;

    return (dt2 < 1);
}

static inline void splitCubicCurve(FloatPoint* p)
{
    float a, b, c, d;
    float ab, bc, cd;
    float abbc, bccd;
    float mid;

    a = p[0].x();
    b = p[1].x();
    c = p[2].x();
    d = p[3].x();
    ab = (a + b) / 2.0;
    bc = (b + c) / 2.0;
    cd = (c + d) / 2.0;
    abbc = (ab + bc) / 2.0;
    bccd = (bc + cd) / 2.0;
    mid = (abbc + bccd) / 2.0;

    p[0].setX(a);
    p[1].setX(ab);
    p[2].setX(abbc);
    p[3].setX(mid);
    p[4].setX(bccd);
    p[5].setX(cd);
    p[6].setX(d);

    a = p[0].y();
    b = p[1].y();
    c = p[2].y();
    d = p[3].y();
    ab = (a + b) / 2.0;
    bc = (b + c) / 2.0;
    cd = (c + d) / 2.0;
    abbc = (ab + bc) / 2.0;
    bccd = (bc + cd) / 2.0;
    mid = (abbc + bccd) / 2.0;

    p[0].setY(a);
    p[1].setY(ab);
    p[2].setY(abbc);
    p[3].setY(mid);
    p[4].setY(bccd);
    p[5].setY(cd);
    p[6].setY(d);
}

void TrapezoidBuilder::insertCubeCurve(const FloatPoint& start, const FloatPoint& control1, const FloatPoint& control2, const FloatPoint& end)
{
    // De Casteljau algorithm.
    const int max = 16 * 3 + 1;
    FloatPoint buffer[max];
    FloatPoint* points = buffer;

    points[0] = start;
    points[1] = control1;
    points[2] = control2;
    points[3] = end;

    do {
        if (isCurveLinear(points)) {
            insertLine(points[0], points[3]);
            points -= 3;
            continue;
        }

        splitCubicCurve(points);
        points += 3;

        if (points >= buffer + max - 4) {
            // This recursive code path is rarely executed.
            insertCubeCurve(points[0], points[1], points[2], points[3]);
            points -= 3;
        }
    } while (points >= buffer);
}

static inline int calculateSegments(float angle, float radius)
{
    float tolerance = 0.001 / radius;
    float angleSegment, error;

    int i = 1;
    do {
        angleSegment = piFloat / i++;
        error = 2.0 / 27.0 * pow(sin(angleSegment / 4.0), 6) / pow(cos(angleSegment / 4.0), 2);
    } while (error > tolerance);

    return ceil(abs(angle) / angleSegment);
}

static inline void arcToCurve(FloatPoint* result, float startAngle, float endAngle)
{
    float sinStartAngle = sin(startAngle);
    float cosStartAngle = cos(startAngle);
    float sinEndAngle = sin(endAngle);
    float cosEndAngle = cos(endAngle);

    float height = 4.0 / 3.0 * tan((endAngle - startAngle) / 4.0);

    result[0].set(cosStartAngle - height * sinStartAngle, sinStartAngle + height * cosStartAngle);
    result[1].set(cosEndAngle + height * sinEndAngle, sinEndAngle - height * cosEndAngle);
    result[2].set(cosEndAngle, sinEndAngle);
}

void TrapezoidBuilder::insertArc(const FloatPoint& lastEndPoint, const ArcToElement& arcToElement)
{
    float startAngle = arcToElement.startAngle();
    float endAngle = arcToElement.endAngle();
    bool anticlockwise = arcToElement.anticlockwise();

    GL2D::AffineTransform arcToTransform(arcToElement.transform());
    GL2D::AffineTransform::Transform arcAxes = { { arcToElement.radius().width(), 0, 0, arcToElement.radius().height(), arcToElement.center().x(), arcToElement.center().y() } };
    arcToTransform *= arcAxes;
    GL2D::AffineTransform transform(m_transform);
    transform *= arcToTransform;

    FloatPoint startPoint = FloatPoint(cos(startAngle), sin(startAngle));
    insertLine(lastEndPoint, transform.apply(startPoint));

    ASSERT(startAngle != endAngle);

    float deltaAngle;
    if (anticlockwise)
        deltaAngle = startAngle - endAngle;
    else
        deltaAngle = endAngle - startAngle;

    float radiusX = transform.transform().m_matrix[0];
    float radiusY = transform.transform().m_matrix[3];
    int segments = calculateSegments(deltaAngle, radiusX >= radiusY ? radiusX : radiusY);
    float step = deltaAngle / segments;
    segments -= 1;

    if (anticlockwise)
        step = -step;

    FloatPoint points[3];
    for (int i = 0; i < segments; i++, startAngle += step) {
        arcToCurve(points, startAngle, startAngle + step);
        insertCubeCurve(transform.apply(startPoint), transform.apply(points[0]), transform.apply(points[1]), transform.apply(points[2]));
        startPoint = points[2];
    }
    arcToCurve(points, startAngle, endAngle);
    insertCubeCurve(transform.apply(startPoint), transform.apply(points[0]), transform.apply(points[1]), transform.apply(points[2]));
}

TrapezoidBuilder::Line::CompareLinesResult TrapezoidBuilder::Line::compareLines(TrapezoidBuilder::Line *other)
{
    if (x0Rounded() <= other->x0Rounded() && x1Rounded() <= other->x1Rounded())
        return CompareLessEqual;
    if (x0Rounded() >= other->x0Rounded() && x1Rounded() >= other->x1Rounded())
        return CompareGreater;
    return CompareIntersection;
}

int TrapezoidBuilder::ScanStripe::addLine(TrapezoidBuilder::Line* line)
{
    if (!m_lines) {
        m_lines = line;
        return -1;
    }

    Line first(m_lines);
    Line* current = &first;
    int height = next()->y() - y();
    float x0 = line->x0Precise();
    float x1 = line->x1Precise();

    while (current->next()) {
        Line *next = current->next();
        Line::CompareLinesResult compareResult = line->compareLines(next);
        if (compareResult == Line::CompareIntersection) {
            int intersectionPoint = roundf(fabsf((line->x0Precise() - next->x0Precise()) / (next->slope() - line->slope())));

            if (intersectionPoint == 0) {
                x0 = next->x0Precise();
                compareResult = (line->x1Precise() > next->x1Precise()) ? Line::CompareGreater : Line::CompareLessEqual;
            } else if (intersectionPoint == height) {
                x1 = next->x1Precise();
                compareResult = (line->x0Precise() > next->x0Precise()) ? Line::CompareGreater : Line::CompareLessEqual;
            } else
                return intersectionPoint;
        }

        if (compareResult == Line::CompareLessEqual) {
            line->setNext(next);
            break;
        }
        current = current->next();
    }

    current->setNext(line);
    m_lines = first.next();
    if (x0 != line->x0Precise())
        line->setX0(x0);
    if (x1 != line->x1Precise())
        line->setX1(x1);
    return -1;
}

static inline bool fuzzyCompare(float f1, float f2)
{
    float absF1F2 = abs(f1 - f2);
    f1 = abs(f1);
    f2 = abs(f2);
    return (absF1F2 <= 0.00001f * (f1 <= f2 ? f1 : f2));
}

bool TrapezoidBuilder::Trapezoid::isCombined(TrapezoidBuilder::Trapezoid* topTrapezoid)
{
    return (fuzzyCompare(this->trapezoid()[1], topTrapezoid->trapezoid()[4])
            && fuzzyCompare(this->trapezoid()[2], topTrapezoid->trapezoid()[5])
            && fuzzyCompare(this->slopes()[0], topTrapezoid->slopes()[0])
            && fuzzyCompare(this->slopes()[1], topTrapezoid->slopes()[1]));
}

} // namespace GL2D
} // namespace WebCore
