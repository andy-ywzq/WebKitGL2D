/*
 * Copyright (C) 2000 Lars Knoll (knoll@kde.org)
 *           (C) 2000 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2003, 2005, 2006, 2007, 2008 Apple Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef StyleImage_h
#define StyleImage_h

#include "CSSValue.h"
#include "Image.h"
#include "IntSize.h"
#include "LayoutSize.h"
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>
#include <wtf/RefPtr.h>

namespace WebCore {

class CachedImage;
class CSSValue;
class RenderElement;
class RenderObject;

typedef void* WrappedImagePtr;

class StyleImage : public RefCounted<StyleImage> {
public:
    virtual ~StyleImage() { }

    bool operator==(const StyleImage& other) const
    {
        return data() == other.data();
    }

    virtual PassRefPtr<CSSValue> cssValue() const = 0;

    virtual bool canRender(const RenderObject*, float /*multiplier*/) const { return true; }
    virtual bool isLoaded() const { return true; }
    virtual bool errorOccurred() const { return false; }
    virtual LayoutSize imageSize(const RenderElement*, float multiplier) const = 0;
    virtual void computeIntrinsicDimensions(const RenderElement*, Length& intrinsicWidth, Length& intrinsicHeight, FloatSize& intrinsicRatio) = 0;
    virtual bool imageHasRelativeWidth() const = 0;
    virtual bool imageHasRelativeHeight() const = 0;
    virtual bool usesImageContainerSize() const = 0;
    virtual void setContainerSizeForRenderer(const RenderElement*, const IntSize&, float) = 0;
    virtual void addClient(RenderElement*) = 0;
    virtual void removeClient(RenderElement*) = 0;
    virtual PassRefPtr<Image> image(RenderElement*, const IntSize&) const = 0;
    virtual WrappedImagePtr data() const = 0;
    virtual float imageScaleFactor() const { return 1; }
    virtual bool knownToBeOpaque(const RenderElement*) const = 0;
    virtual CachedImage* cachedImage() const { return 0; }

    ALWAYS_INLINE bool isCachedImage() const { return m_isCachedImage; }
    ALWAYS_INLINE bool isPendingImage() const { return m_isPendingImage; }
    ALWAYS_INLINE bool isGeneratedImage() const { return m_isGeneratedImage; }
    ALWAYS_INLINE bool isCachedImageSet() const { return m_isCachedImageSet; }
    
    static bool imagesEquivalent(StyleImage* image1, StyleImage* image2)
    {
        return image1 == image2 || (image1 && image2 && *image1 == *image2);
    }

protected:
    StyleImage()
        : m_isCachedImage(false)
        , m_isPendingImage(false)
        , m_isGeneratedImage(false)
        , m_isCachedImageSet(false)
    {
    }
    bool m_isCachedImage : 1;
    bool m_isPendingImage : 1;
    bool m_isGeneratedImage : 1;
    bool m_isCachedImageSet : 1;
};

}
#endif
