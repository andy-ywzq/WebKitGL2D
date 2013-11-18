/*
 *  Copyright (C) 2013 Nokia Corporation and/or its subsidiary(-ies).
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

#if ENABLE(MEDIA_STREAM) && USE(WEBRTCLIB)

#include "RTCPeerConnectionObserver.h"

#include "MediaStreamAudioSource.h"
#include "MediaStreamDescriptor.h"
#include "MediaStreamSource.h"
#include "MediaStreamTrackWebRTCObserver.h"
#include "NotImplemented.h"
#include "RTCDataChannelHandlerWebRTC.h"
#include "RTCIceCandidateDescriptor.h"
#include "WebRTCUtils.h"
#include <wtf/Functional.h>
#include <wtf/MainThread.h>
#include <wtf/PassOwnPtr.h>

namespace WebCore {

RTCPeerConnectionObserver::RTCPeerConnectionObserver(RTCPeerConnectionHandlerClient* client)
    : m_client(client)
{
}

void RTCPeerConnectionObserver::OnError()
{
    notImplemented();
}

void RTCPeerConnectionObserver::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState newState)
{
    callOnMainThread(bind(&RTCPeerConnectionHandlerClient::didChangeSignalingState, m_client, WebRTCUtils::toWebKitSignalingState(newState)));
}

void RTCPeerConnectionObserver::OnStateChange(webrtc::PeerConnectionObserver::StateType)
{
    notImplemented();
}

void RTCPeerConnectionObserver::processTrack(webrtc::MediaStreamTrackInterface* track, MediaStreamTrackObserverVector& trackObservers, MediaStreamSourceVector& sourceVector)
{
    RefPtr<MediaStreamSource> source;
    if (track->kind() == "audio")
        source = MediaStreamAudioSource::create();

    // FIXME: Handle video.

    RefPtr<MediaStreamTrackWebRTCObserver> trackObserver = adoptRef(new MediaStreamTrackWebRTCObserver(track, source.get()));
    track->RegisterObserver(trackObserver.get());
    sourceVector.append(source);
    trackObservers.append(trackObserver);
}

PassRefPtr<MediaStreamDescriptor> RTCPeerConnectionObserver::mediaStreamDescriptorFromMediaStreamInterface(webrtc::MediaStreamInterface* stream)
{
    MediaStreamSourceVector audioSourceVector;
    MediaStreamSourceVector videoSourceVector;
    MediaStreamTrackObserverVector audioTrackObservers;
    MediaStreamTrackObserverVector videoTrackObservers;
    webrtc::AudioTrackVector audioTracks = stream->GetAudioTracks();
    webrtc::VideoTrackVector videoTracks = stream->GetVideoTracks();
    for (unsigned i = 0; i < audioTracks.size(); i++)
        processTrack(audioTracks.at(i), audioTrackObservers, audioSourceVector);

    // FIXME: Handle video.

    RefPtr<MediaStreamDescriptor> descriptor = MediaStreamDescriptor::create(audioSourceVector, videoSourceVector, MediaStreamDescriptor::IsNotEnded);

    RefPtr<MediaStreamWebRTCObserver> streamObserver = adoptRef(new MediaStreamWebRTCObserver(stream, descriptor.get(), audioTrackObservers, videoTrackObservers));
    stream->RegisterObserver(streamObserver.get());
    m_streamObservers.append(streamObserver);

    return descriptor.release();
}

void RTCPeerConnectionObserver::OnAddStream(webrtc::MediaStreamInterface* stream)
{
    RefPtr<MediaStreamDescriptor> descriptor = mediaStreamDescriptorFromMediaStreamInterface(stream);
    callOnMainThread(bind(&RTCPeerConnectionHandlerClient::didAddRemoteStream, m_client, descriptor.release()));
}

void RTCPeerConnectionObserver::OnRemoveStream(webrtc::MediaStreamInterface* stream)
{
    MediaStreamDescriptor* descriptor = 0;
    for (unsigned i = 0; i < m_streamObservers.size(); ++i) {
        if (m_streamObservers[i]->webRTCStream() != stream)
            continue;

        descriptor = m_streamObservers[i]->descriptor();
        m_streamObservers[i]->webRTCStream()->UnregisterObserver(m_streamObservers[i].get());
        m_streamObservers.remove(i);
        break;
    }

    if (!descriptor)
        return;

    callOnMainThread(bind(&RTCPeerConnectionHandlerClient::didRemoveRemoteStream, m_client, descriptor));
}

void RTCPeerConnectionObserver::OnRenegotiationNeeded()
{
    callOnMainThread(bind(&RTCPeerConnectionHandlerClient::negotiationNeeded, m_client));
}

void RTCPeerConnectionObserver::OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState newState)
{
    if (newState == webrtc::PeerConnectionInterface::kIceGatheringComplete)
        // According to the spec we must call didGenerateIceCandidate with a null parameter
        // to indicate that the gathering is complete.
        callOnMainThread(bind(&RTCPeerConnectionHandlerClient::didGenerateIceCandidate, m_client, (RTCIceCandidateDescriptor*)0));

    callOnMainThread(bind(&RTCPeerConnectionHandlerClient::didChangeIceGatheringState, m_client, WebRTCUtils::toWebKitIceGatheringState(newState)));
}

void RTCPeerConnectionObserver::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState newState)
{
    callOnMainThread(bind(&RTCPeerConnectionHandlerClient::didChangeIceConnectionState, m_client, WebRTCUtils::toWebKitIceConnectionState(newState)));
}

void RTCPeerConnectionObserver::OnIceCandidate(const webrtc::IceCandidateInterface* candidate)
{
    std::string out;
    candidate->ToString(&out);
    RefPtr<RTCIceCandidateDescriptor> ice = RTCIceCandidateDescriptor::create(out.c_str(), candidate->sdp_mid().c_str(), candidate->sdp_mline_index());
    callOnMainThread(bind(&RTCPeerConnectionHandlerClient::didGenerateIceCandidate, m_client, ice.release()));
}

void RTCPeerConnectionObserver::OnDataChannel(webrtc::DataChannelInterface* dataChannel)
{
    callOnMainThread(bind(&RTCPeerConnectionHandlerClient::didAddRemoteDataChannel, m_client, adoptPtr(new RTCDataChannelHandlerWebRTC(dataChannel))));
}

} // namespace WebCore

#endif // ENABLE(MEDIA_STREAM) && USE(WEBRTCLIB)
