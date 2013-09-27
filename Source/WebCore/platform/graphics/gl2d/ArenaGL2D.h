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

#ifndef ArenaGL2D_h
#define ArenaGL2D_h

#include <wtf/PassOwnPtr.h>

#include <stdio.h>

namespace WebCore {
namespace GL2D {

#define ARENA_BLOCK_SIZE (2048 - (int)sizeof(void*))

class Arena {
public:
    ~Arena();
    static PassOwnPtr<Arena> create() { return adoptPtr(new Arena()); }

    void* alloc(int size);

private:
    Arena();

    struct ArenaElements {
        ArenaElements* m_next;
        uint8_t m_value[ARENA_BLOCK_SIZE];

        ArenaElements()
            : m_next(0)
        {
        }
    };

    ArenaElements *m_first;
    ArenaElements *m_last;
    int m_fill;
};

} // namespace GL2D
} // namespace WebCore

#endif // ArenaGL2D_h
