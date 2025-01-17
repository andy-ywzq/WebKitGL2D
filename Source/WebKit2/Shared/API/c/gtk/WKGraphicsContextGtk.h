/*
 * Copyright (C) 2011 Igalia SL All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WKGraphicsContextGtk_h
#define WKGraphicsContextGtk_h

#include <WebKit2/WKBase.h>

#if USE(CAIRO)
#include <cairo.h>
#elif USE(GL2D)
#include <PlatformContextGL2D.h>
typedef WebCore::PlatformContextGL2D gl2dcontext;
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if USE(CAIRO)
WK_EXPORT cairo_t* WKGraphicsContextGetGtkContext(WKGraphicsContextRef);
#elif USE(GL2D)
WK_EXPORT gl2dcontext* WKGraphicsContextGetGtkContext(WKGraphicsContextRef);
#endif

#ifdef __cplusplus
}
#endif

#endif /* WKGraphicsContextGtk_h */
