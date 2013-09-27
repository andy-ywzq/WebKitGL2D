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
#include "fontconfigGL2D.h"

namespace WTF {
template<> void refIfNotNull(FcPattern* ptr)
{
    if (LIKELY(ptr != 0))
        FcPatternReference(ptr);
}

template<> void derefIfNotNull(FcPattern* ptr)
{
    if (LIKELY(ptr != 0))
        FcPatternDestroy(ptr);
}
}

namespace WebCore {
ScaledFont::ScaledFont()
{
    adopted(this);
}

ScaledFont::ScaledFont(FcPattern* pattern)
{
    FT_Library library;
    FT_Face face;

    if(!FT_Init_FreeType( &library ) && !FT_New_Face( library, "/usr/share/fonts/truetype/freefont/FreeMono.ttf", 0, &face ))
        m_face = face;
    else
        m_face = 0;
    adopted(this);
}

FT_Face ScaledFont::fontFace()
{
    return m_face;
}

}