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

#ifndef TrapezoidBuilderGL2D_h
#define TrapezoidBuilderGL2D_h

#include "ArenaGL2D.h"
#include "FloatPoint.h"
#include "IntSize.h"
#include "PathGL2D.h"
#include "TransformGL2D.h"
#include "TrapezoidListGL2D.h"
#include "WindRule.h"
#include <wtf/OwnPtr.h>
#include <wtf/Vector.h>

namespace WebCore {
namespace GL2D {

class TrapezoidBuilder {
private:
    static const int antiAliasing = 16;
public:
    TrapezoidBuilder(const PathData* path, const GL2D::ClipRect& clipRect, const GL2D::AffineTransform& transform, WindRule fillRule = RULE_NONZERO)
        : m_path(path)
        , m_fillRule(fillRule)
        , m_arena(Arena::create())
        , m_scanStripes(0)
        , m_clipTop(clipRect.topInteger() * antiAliasing)
        , m_clipBottom(clipRect.bottomInteger() * antiAliasing)
        , m_clipLeft(clipRect.leftInteger())
        , m_clipRight(clipRect.rightInteger())
        , m_transform(transform)
    {
    }

    PassOwnPtr<TrapezoidList> trapezoidList();

private:

    static inline int roundToInt(float f)
    {
        return static_cast<int>(floor(f));
    }

    struct Line {
        enum CompareLinesResult {
            CompareLessEqual,
            CompareGreater,
            CompareIntersection,
        };

        Line(float x0, float x1, float slope, int direction)
            : m_x0Precise(x0)
            , m_x1Precise(x1)
            , m_slope(slope)
            , m_x0Rounded((roundToInt(x0) << 1) + ((direction > 0) ? 1 : 0))
            , m_x1Rounded(roundToInt(x1) << 1)
            , m_next(0)
        {
        }

        Line(Line* next)
            : m_next(next)
        {
            // Fake head for adding a new line. The other members are intentionally
            // not initialized (they are unused). Cannot be used for any other purpose!
        }

        static inline Line* create(Arena* arena, float x0, float x1, float slope, int direction)
        {
            return new (reinterpret_cast<Line*>(arena->alloc(sizeof(Line)))) Line(x0, x1, slope, direction);
        }

        int direction() const { return ((m_x0Rounded & 1) << 1) - 1; }
        float x0Precise() const { return m_x0Precise; }
        float x1Precise() const { return m_x1Precise; }
        float slope() const { return m_slope; }
        int32_t x0Rounded() const { return m_x0Rounded >> 1; }
        int32_t x1Rounded() const { return m_x1Rounded >> 1; }
        Line* next() { return m_next; }

        void setX0(float x0f)
        {
            m_x0Precise = x0f;
            m_x0Rounded = (roundToInt(x0f) << 1) + (m_x0Rounded & 1);
        }

        void setX1(float x1f)
        {
            m_x1Precise = x1f;
            m_x1Rounded = roundToInt(x1f) << 1;
        }

        void setNext(Line* next) { m_next = next; }
        inline CompareLinesResult compareLines(Line*);

        float m_x0Precise;
        float m_x1Precise;
        float m_slope;
        int32_t m_x0Rounded;
        int32_t m_x1Rounded;
        Line* m_next;
    };

    struct Trapezoid {
        Trapezoid()
            : m_top(0)
            , m_next(0)
        {
        }

        static inline Trapezoid* create(Arena* arena)
        {
            return new (reinterpret_cast<Trapezoid*>(arena->alloc(sizeof(Trapezoid)))) Trapezoid();
        }

        float* slopes() { return m_slopes; }
        Trapezoid* top() { return m_top; }
        Trapezoid* next() { return m_next; }

        TrapezoidList::Trapezoid& trapezoid() { return m_trapezoid; }

        void fill(float* points)
        {
            m_trapezoid.setPoints(points);
            m_slopes[0] = points[6];
            m_slopes[1] = points[7];
        }
        Trapezoid* firstTop(Trapezoid* top) { return (top->top()) ? firstTop(top->top()) : top; }
        void setTop(Trapezoid* top) { m_top = top; }
        void setNext(Trapezoid* next) { m_next = next; }

        bool isCombined(Trapezoid* topTrapezoid);

        TrapezoidList::Trapezoid m_trapezoid;
        float m_slopes[2];
        Trapezoid* m_top;
        Trapezoid* m_next;
    };

    struct ScanStripe {
        ScanStripe(int y)
            : m_y(y)
            , m_lines(0)
            , m_trapezoids(0)
            , m_next(0)
        {
        }

        static inline ScanStripe* create(Arena* arena, int y)
        {
            return new (reinterpret_cast<ScanStripe*>(arena->alloc(sizeof(ScanStripe)))) ScanStripe(y);
        }

        int y() const { return m_y; }
        Line* lines() { return m_lines; }
        Trapezoid* trapezoids() { return m_trapezoids; }
        ScanStripe* next() { return m_next; }

        inline int addLine(Line*);

        bool addOrCombineTrapezoid(Arena* arena, float* result, ScanStripe* previousScanStripe);

        void setY(int y) { m_y = y; }
        void setNext(ScanStripe* next) { m_next = next; }

        int m_y;
        Line* m_lines;
        Trapezoid* m_trapezoids;
        ScanStripe* m_next;
    };

    ScanStripe* createScanStripe(int y) { return ScanStripe::create(m_arena.get(), y); }
    ScanStripe* splitScanStripe(ScanStripe*, int);

    inline void trapezoidIteratorInit();
    bool trapezoidIteratorNext(float*);

    void insertLine(FloatPoint, FloatPoint);
    void insertQuadCurve(const FloatPoint&, const FloatPoint&, const FloatPoint&);
    void insertCubeCurve(const FloatPoint&, const FloatPoint&, const FloatPoint&, const FloatPoint&);
    inline void insertArc(const FloatPoint&, const ArcToElement&);

    const PathData* m_path;
    WindRule m_fillRule;

    OwnPtr<Arena> m_arena;

    ScanStripe* m_scanStripes;

    int m_clipTop;
    int m_clipBottom;
    int m_clipLeft;
    int m_clipRight;
    const GL2D::AffineTransform& m_transform;

    ScanStripe* m_currentScanStripe;
    ScanStripe* m_previousScanStripe;
    Line* m_currentLine;
    Trapezoid* m_currentTrapezoid;
};

} // namespace GL2D
} // namespace WebCore

#endif  // TrapezoidBuilderGL2D_h
