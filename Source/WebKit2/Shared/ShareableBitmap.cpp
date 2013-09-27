/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
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

#include "config.h"
#include "ShareableBitmap.h"

#include "SharedMemory.h"
#include "WebCoreArgumentCoders.h"
#include <WebCore/GraphicsContext.h>

using namespace WebCore;

namespace WebKit {

ShareableBitmap::Handle::Handle()
    : m_flags(0)
{
}

void ShareableBitmap::Handle::encode(CoreIPC::ArgumentEncoder& encoder) const
{
#if USE(GL2D)
    encoder << m_imageHandle;
#else
    encoder << m_handle;
#endif
    encoder << m_size;
    encoder << m_flags;
}

bool ShareableBitmap::Handle::decode(CoreIPC::ArgumentDecoder& decoder, Handle& handle)
{
#if USE(GL2D)
    if (!decoder.decode(handle.m_imageHandle))
        return false;
#else
    if (!decoder.decode(handle.m_handle))
        return false;
#endif
    if (!decoder.decode(handle.m_size))
        return false;
    if (!decoder.decode(handle.m_flags))
        return false;
    return true;
}

PassRefPtr<ShareableBitmap> ShareableBitmap::create(const IntSize& size, Flags flags)
{
#if USE(GL2D)
    return adoptRef(new ShareableBitmap(size, flags, WebCore::NativeImageGL2D::create(size.width(), size.height())));
#else
    size_t numBytes = numBytesForSize(size);
    
    void* data = 0;
    if (!tryFastMalloc(numBytes).getValue(data))
        return 0;

    return adoptRef(new ShareableBitmap(size, flags, data));
#endif
}

PassRefPtr<ShareableBitmap> ShareableBitmap::createShareable(const IntSize& size, Flags flags)
{
#if USE(GL2D)
    return adoptRef(new ShareableBitmap(size, flags, WebCore::NativeImageGL2D::createShared(size)));
#else
    size_t numBytes = numBytesForSize(size);

    RefPtr<SharedMemory> sharedMemory = SharedMemory::create(numBytes);
    if (!sharedMemory)
        return 0;

    return adoptRef(new ShareableBitmap(size, flags, sharedMemory));
#endif
}

#if !USE(GL2D)
PassRefPtr<ShareableBitmap> ShareableBitmap::create(const IntSize& size, Flags flags, PassRefPtr<SharedMemory> sharedMemory)
{
    ASSERT(sharedMemory);

    size_t numBytes = numBytesForSize(size);
    ASSERT_UNUSED(numBytes, sharedMemory->size() >= numBytes);
    
    return adoptRef(new ShareableBitmap(size, flags, sharedMemory));
}
#endif

PassRefPtr<ShareableBitmap> ShareableBitmap::create(const Handle& handle, SharedMemory::Protection protection)
{
#if USE(GL2D)
    return adoptRef(new ShareableBitmap(handle.m_size, handle.m_flags,
        WebCore::NativeImageGL2D::createShared(handle.m_size, handle.m_imageHandle)));
#else
    // Create the shared memory.
    RefPtr<SharedMemory> sharedMemory = SharedMemory::create(handle.m_handle, protection);
    if (!sharedMemory)
        return 0;

    return create(handle.m_size, handle.m_flags, sharedMemory.release());
#endif
}

bool ShareableBitmap::createHandle(Handle& handle, SharedMemory::Protection protection)
{
    ASSERT(isBackedBySharedMemory());

#if USE(GL2D)
    handle.m_imageHandle = m_sharedImage->sharedImageHandle();
#else
    if (!m_sharedMemory->createHandle(handle.m_handle, protection))
        return false;
#endif
    handle.m_size = m_size;
    handle.m_flags = m_flags;
    return true;
}

#if USE(GL2D)
ShareableBitmap::ShareableBitmap(const WebCore::IntSize& size, Flags flags, PassOwnPtr<WebCore::NativeImageGL2D> nativeImage)
    : m_size(size)
    , m_flags(flags)
    , m_data(0)
    , m_sharedImage(nativeImage)
{
}

ShareableBitmap::~ShareableBitmap()
{
}

#else

ShareableBitmap::ShareableBitmap(const IntSize& size, Flags flags, void* data)
    : m_size(size)
    , m_flags(flags)
    , m_data(data)
{
}

ShareableBitmap::ShareableBitmap(const IntSize& size, Flags flags, PassRefPtr<SharedMemory> sharedMemory)
    : m_size(size)
    , m_flags(flags)
    , m_sharedMemory(sharedMemory)
    , m_data(0)
{
}

ShareableBitmap::~ShareableBitmap()
{
    if (!isBackedBySharedMemory())
        fastFree(m_data);
}

bool ShareableBitmap::resize(const IntSize& size)
{
    // We can't resize backing stores that are backed by shared memory.
    ASSERT(!isBackedBySharedMemory());

    if (size == m_size)
        return true;

    size_t newNumBytes = numBytesForSize(size);
    
    // Try to resize.
    char* newData = 0;
    if (!tryFastRealloc(m_data, newNumBytes).getValue(newData)) {
        // We failed, but the backing store is still kept in a consistent state.
        return false;
    }

    m_size = size;
    m_data = newData;

    return true;
}

void* ShareableBitmap::data() const
{
    if (isBackedBySharedMemory())
        return m_sharedMemory->data();

    ASSERT(m_data);
    return m_data;
}

#endif // GL2D

} // namespace WebKit
