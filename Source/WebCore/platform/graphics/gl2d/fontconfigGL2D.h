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

#ifndef FontconfigGL2D_h
#define FontconfigGL2D_h

#include <fontconfig/fontconfig.h>
#include <fontconfig/fcfreetype.h>
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>

typedef struct _FcPattern FcPattern;

namespace WTF {
template<> void refIfNotNull(FcPattern*);
template<> void derefIfNotNull(FcPattern*);
}

namespace WebCore {

class ScaledFont : public RefCounted<ScaledFont> {
    WTF_MAKE_FAST_ALLOCATED;
public:
    static ScaledFont* create() { return new ScaledFont(); }
    static ScaledFont* create(FcPattern* pattern) { return new ScaledFont(pattern); }

    FT_Face fontFace();

private:
    ScaledFont();
    ScaledFont(FcPattern*);

    float m_size;
    FT_Face m_face;
};

} // namespace WebCore

#endif
