/*
 * Copyright (C) 2013 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef GraphicsLayerCARemote_h
#define GraphicsLayerCARemote_h

#if USE(ACCELERATED_COMPOSITING)

#include <WebCore/GraphicsLayerCA.h>
#include <WebCore/PlatformLayer.h>

namespace WebKit {

class RemoteLayerTreeContext;

class GraphicsLayerCARemote FINAL : public WebCore::GraphicsLayerCA {
public:
    GraphicsLayerCARemote(WebCore::GraphicsLayerClient* client, RemoteLayerTreeContext* context)
        : GraphicsLayerCA(client)
        , m_context(context)
    {
    }

    virtual ~GraphicsLayerCARemote();

#if ENABLE(CSS_FILTERS)
    virtual bool filtersCanBeComposited(const WebCore::FilterOperations& filters) OVERRIDE;
#endif

private:
    virtual PassRefPtr<WebCore::PlatformCALayer> createPlatformCALayer(WebCore::PlatformCALayer::LayerType, WebCore::PlatformCALayerClient* owner) OVERRIDE;
    virtual PassRefPtr<WebCore::PlatformCALayer> createPlatformCALayer(PlatformLayer*, WebCore::PlatformCALayerClient* owner) OVERRIDE;

    // No accelerated animations for now.
    virtual bool addAnimation(const WebCore::KeyframeValueList&, const WebCore::IntSize&, const WebCore::Animation*, const String&, double) OVERRIDE { return false; }

    RemoteLayerTreeContext* m_context;
};

} // namespace WebKit

#endif // USE(ACCELERATED_COMPOSITING)

#endif // GraphicsLayerCARemote_h
