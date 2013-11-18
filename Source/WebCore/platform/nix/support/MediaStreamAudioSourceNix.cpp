/*
 * Copyright (C) 2013 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#if ENABLE(MEDIA_STREAM)

#include <public/MediaStreamAudioSource.h>


#include "AudioBus.h"
#include "MediaStreamAudioSource.h"
#include <public/AudioDestinationConsumer.h>
#include <wtf/MainThread.h>
#include <wtf/PassOwnPtr.h>
#include <wtf/Vector.h>
#include <wtf/text/CString.h>


namespace Nix {

MediaStreamAudioSource::MediaStreamAudioSource()
    : m_private(WebCore::MediaStreamAudioSource::create().release().leakRef())
{
}

MediaStreamAudioSource::MediaStreamAudioSource(const PassRefPtr<WebCore::MediaStreamAudioSource>& MediaStreamAudioSource)
    : m_private(MediaStreamAudioSource)
{
}

MediaStreamAudioSource& MediaStreamAudioSource::operator=(WebCore::MediaStreamAudioSource* MediaStreamAudioSource)
{
    m_private = MediaStreamAudioSource;
    return *this;
}

void MediaStreamAudioSource::assign(const MediaStreamAudioSource& other)
{
    m_private = other.m_private;
}

void MediaStreamAudioSource::reset()
{
    m_private.reset();
}

MediaStreamAudioSource::operator PassRefPtr<WebCore::MediaStreamAudioSource>() const
{
    return m_private.get();
}

MediaStreamAudioSource::operator WebCore::MediaStreamAudioSource*() const
{
    return m_private.get();
}

const char* MediaStreamAudioSource::deviceId() const
{
    ASSERT(!m_private.isNull());
    return m_private->deviceId().utf8().data();
}

void MediaStreamAudioSource::setDeviceId(const char* deviceId)
{
    ASSERT(!m_private.isNull());
    m_private->setDeviceId(deviceId);
}

class ConsumerWrapper : public WebCore::AudioDestinationConsumer {
public:
    static PassRefPtr<ConsumerWrapper> create(Nix::AudioDestinationConsumer* consumer)
    {
        return adoptRef(new ConsumerWrapper(consumer));
    }

    virtual void setFormat(size_t numberOfChannels, float sampleRate) OVERRIDE;
    virtual void consumeAudio(WebCore::AudioBus*, size_t numberOfFrames) OVERRIDE;

    Nix::AudioDestinationConsumer* consumer() { return m_consumer; }

private:
    explicit ConsumerWrapper(Nix::AudioDestinationConsumer* consumer) : m_consumer(consumer) { }

    // m_consumer is not owned by this class.
    Nix::AudioDestinationConsumer* m_consumer;
};

void ConsumerWrapper::setFormat(size_t numberOfChannels, float sampleRate)
{
    m_consumer->setFormat(numberOfChannels, sampleRate);
}

void ConsumerWrapper::consumeAudio(WebCore::AudioBus* bus, size_t numberOfFrames)
{
    if (!bus)
        return;

    // Wrap AudioBus.
    size_t numberOfChannels = bus->numberOfChannels();
    std::vector<const float*> busVector(numberOfChannels);
    for (size_t i = 0; i < numberOfChannels; ++i)
        busVector[i] = bus->channel(i)->data();

    m_consumer->consumeAudio(busVector, numberOfFrames);
}

void MediaStreamAudioSource::addAudioConsumer(AudioDestinationConsumer* consumer)
{
    ASSERT(isMainThread());
    ASSERT(!m_private.isNull() && consumer);

    m_private->addAudioConsumer(ConsumerWrapper::create(consumer));
}

bool MediaStreamAudioSource::removeAudioConsumer(AudioDestinationConsumer* consumer)
{
    ASSERT(isMainThread());
    ASSERT(!m_private.isNull() && consumer);

    const WTF::Vector<RefPtr<WebCore::AudioDestinationConsumer> >& consumers = m_private->audioConsumers();
    for (WTF::Vector<RefPtr<WebCore::AudioDestinationConsumer> >::const_iterator it = consumers.begin(); it != consumers.end(); ++it) {
        ConsumerWrapper* wrapper = static_cast<ConsumerWrapper*>((*it).get());
        if (wrapper->consumer() == consumer) {
            m_private->removeAudioConsumer(wrapper);
            return true;
        }
    }
    return false;
}

} // namespace Nix

#endif // ENABLE(MEDIA_STREAM)

