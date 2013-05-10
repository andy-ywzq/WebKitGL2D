/*
 * Copyright (C) 2012-2013 Nokia Corporation and/or its subsidiary(-ies).
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef NIXViewAutoPtr_h
#define NIXViewAutoPtr_h

#include "NIXView.h"

class NIXViewAutoPtr {
public:
    explicit NIXViewAutoPtr(WKViewRef view = 0) : m_view(view) { }
    ~NIXViewAutoPtr() { WKRelease(m_view); }

    WKViewRef get() const { return m_view; }
    bool operator!() const { return !m_view; }

    // This conversion operator allows implicit conversion to bool but not to other integer types.
    typedef WKViewRef NIXViewAutoPtr::*UnspecifiedBoolType;
    operator UnspecifiedBoolType() const { return m_view ? &NIXViewAutoPtr::m_view : 0; }

    void reset(WKViewRef view = 0) {
        WKViewRef old = m_view;
        m_view = view;
        if (old)
            WKRelease(old);
    }

private:
    // Disable copy.
    NIXViewAutoPtr(NIXViewAutoPtr&);
    NIXViewAutoPtr& operator=(const NIXViewAutoPtr&);

    WKViewRef m_view;
};

#endif // NIXViewAutoPtr_h
