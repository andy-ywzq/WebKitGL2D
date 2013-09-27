/*
 * Copyright (C) 2013 University of Szeged
 * Copyright (C) 2013 Kristof Kosztyo
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
#include "SimpleFontData.h"
#include "UTF16UChar32Iterator.h"

#include "FloatRect.h"
#include "NotImplemented.h"

namespace WebCore{

void SimpleFontData::platformInit()
{
    if (!m_platformData.m_size)
        return;
    ASSERT(m_platformData.scaledFont());
    float ascent = 1.0;
    float descent = 1.0;
    float lineGap = 1.0;

    m_fontMetrics.setAscent(ascent);
    m_fontMetrics.setDescent(descent);
    m_fontMetrics.setLineSpacing(lroundf(ascent) + lroundf(descent) + lroundf(lineGap));
    m_fontMetrics.setLineGap(lineGap);
    m_fontMetrics.setXHeight(1.0);
    m_spaceWidth = 1.0;
    m_syntheticBoldOffset = m_platformData.syntheticBold() ? 1.0f : 0.f;
}

void SimpleFontData::platformDestroy()
{
}

void SimpleFontData::determinePitch()
{
    m_treatAsFixedPitch = m_platformData.isFixedPitch();
}

PassRefPtr<SimpleFontData> SimpleFontData::platformCreateScaledFontData(const FontDescription& fontDescription, float scaleFactor) const
{
    ASSERT(m_platformData.scaledFont());
    return SimpleFontData::create(FontPlatformData(scaleFactor * fontDescription.computedSize(), m_platformData.syntheticBold(), m_platformData.syntheticOblique()), isCustomFont(), false);
}

    
FloatRect SimpleFontData::platformBoundsForGlyph(unsigned short glyph) const
{
    if (!m_platformData.size())
        return FloatRect();
    return FloatRect(1.0, 1.0, 1.0, 1.0);
}

float SimpleFontData::platformWidthForGlyph(Glyph glyph) const
{
    return 1.0;
}

void SimpleFontData::platformCharWidthInit()
{
    m_avgCharWidth = 0.f;
    m_maxCharWidth = 0.f;
    initCharWidths();
}

bool SimpleFontData::containsCharacters(const UChar* characters, int bufferLength) const
{
    ASSERT(m_platformData.scaledFont());
    FT_Face face = m_platformData.scaledFont()->fontFace();
    if (!face)
        return false;

    UTF16UChar32Iterator iterator(characters, bufferLength);
    UChar32 character = iterator.next();
    while (character != iterator.end()) {
        if (!FcFreeTypeCharIndex(face, character)) {
            return false;
        }
        character = iterator.next();
    }

    return true;
}


} // namespace WebCore
