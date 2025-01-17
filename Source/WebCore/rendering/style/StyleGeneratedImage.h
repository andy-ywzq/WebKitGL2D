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

#ifndef StyleGeneratedImage_h
#define StyleGeneratedImage_h

#include "StyleImage.h"

namespace WebCore {

class CSSValue;
class CSSImageGeneratorValue;

class StyleGeneratedImage FINAL : public StyleImage {
public:
    static PassRefPtr<StyleGeneratedImage> create(CSSImageGeneratorValue* value)
    {
        return adoptRef(new StyleGeneratedImage(value));
    }

    CSSImageGeneratorValue* imageValue() const { return m_imageGeneratorValue.get(); }

private:
    virtual WrappedImagePtr data() const OVERRIDE { return m_imageGeneratorValue.get(); }

    virtual PassRefPtr<CSSValue> cssValue() const OVERRIDE;

    virtual LayoutSize imageSize(const RenderElement*, float multiplier) const OVERRIDE;
    virtual bool imageHasRelativeWidth() const OVERRIDE { return !m_fixedSize; }
    virtual bool imageHasRelativeHeight() const OVERRIDE { return !m_fixedSize; }
    virtual void computeIntrinsicDimensions(const RenderElement*, Length& intrinsicWidth, Length& intrinsicHeight, FloatSize& intrinsicRatio) OVERRIDE;
    virtual bool usesImageContainerSize() const OVERRIDE { return !m_fixedSize; }
    virtual void setContainerSizeForRenderer(const RenderElement*, const IntSize& containerSize, float) OVERRIDE { m_containerSize = containerSize; }
    virtual void addClient(RenderElement*) OVERRIDE;
    virtual void removeClient(RenderElement*) OVERRIDE;
    virtual PassRefPtr<Image> image(RenderElement*, const IntSize&) const OVERRIDE;
    virtual bool knownToBeOpaque(const RenderElement*) const OVERRIDE;

    StyleGeneratedImage(PassRefPtr<CSSImageGeneratorValue>);
    
    RefPtr<CSSImageGeneratorValue> m_imageGeneratorValue;
    IntSize m_containerSize;
    bool m_fixedSize;
};

inline StyleGeneratedImage* toStyleGeneratedImage(StyleImage* image)
{
    ASSERT_WITH_SECURITY_IMPLICATION(!image || image->isGeneratedImage());
    return static_cast<StyleGeneratedImage*>(image);
}

}
#endif
