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
#include "ImageDecoder.h"

#include "NativeImageGL2D.h"

namespace WebCore {

NativeImagePtr ImageFrame::asNewNativeImage() const
{
    Vector<PixelData> bytes = m_backingStore;
    uint8_t* ptr = reinterpret_cast<uint8_t*>(bytes.data());
    uint8_t* end = ptr + m_size.width() * m_size.height() * 4;

    if (!m_premultiplyAlpha) {
        while (ptr < end) {
            uint8_t blue = ptr[0];
            ptr[0] = ptr[2];
            ptr[2] = blue;
            ptr += 4;
        }
    } else {
        while (ptr < end) {
            int alpha = ptr[3];
            if (alpha) {
                uint8_t blue = static_cast<int>(ptr[0]) * 255 / alpha;
                ptr[0] = static_cast<int>(ptr[2]) * 255 / alpha;
                ptr[1] = static_cast<int>(ptr[1]) * 255 / alpha;
                ptr[2] = blue;
            }
            ptr += 4;
        }
    }

    return NativeImageGL2D::createPtr(m_size.width(), m_size.height(), reinterpret_cast<void*>(bytes.data()), m_hasAlpha);
}

} //namespace WebCore
