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

#include "WebRTCUtils.h"

#include "RTCConfiguration.h"
#include <wtf/text/CString.h>

namespace WebCore {

namespace WebRTCUtils {

void toMediaConstraintsWebRTC(const WTF::Vector<MediaConstraint> constraints, webrtc::MediaConstraintsInterface::Constraints* webRTCConstraints)
{
    for (const MediaConstraint& constraint : constraints) {
        webrtc::MediaConstraintsInterface::Constraint newConstraint;
        newConstraint.key = constraint.m_name.utf8().data();
        newConstraint.value = constraint.m_value.utf8().data();

        // Ignore internal constraints set by JS.
        if (!newConstraint.key.compare(0, 8, "internal"))
            continue;

        webRTCConstraints->push_back(newConstraint);
    }
}

void toWebRTCIceServers(PassRefPtr<RTCConfiguration> configuration, webrtc::PeerConnectionInterface::IceServers* servers)
{
    if (!configuration.get() || !servers)
        return;

    for (size_t i = 0; i < configuration->numberOfServers(); ++i) {
        webrtc::PeerConnectionInterface::IceServer webRTCServer;
        RTCIceServer* iceServer = configuration->server(i);
        webRTCServer.username = iceServer->username().utf8().data();
        webRTCServer.password = iceServer->credential().utf8().data();
        webRTCServer.uri = iceServer->uri().string().utf8().data();
        servers->push_back(webRTCServer);
    }
}

RTCPeerConnectionHandlerClient::SignalingState toWebKitSignalingState(webrtc::PeerConnectionInterface::SignalingState state)
{
    switch (state) {
    case webrtc::PeerConnectionInterface::kStable:
        return RTCPeerConnectionHandlerClient::SignalingStateStable;
    case webrtc::PeerConnectionInterface::kHaveLocalOffer:
        return RTCPeerConnectionHandlerClient::SignalingStateHaveLocalOffer;
    case webrtc::PeerConnectionInterface::kHaveRemoteOffer:
        return RTCPeerConnectionHandlerClient::SignalingStateHaveRemoteOffer;
    case webrtc::PeerConnectionInterface::kHaveLocalPrAnswer:
        return RTCPeerConnectionHandlerClient::SignalingStateHaveLocalPrAnswer;
    case webrtc::PeerConnectionInterface::kHaveRemotePrAnswer:
        return RTCPeerConnectionHandlerClient::SignalingStateHaveRemotePrAnswer;
    case webrtc::PeerConnectionInterface::kClosed:
        return RTCPeerConnectionHandlerClient::SignalingStateClosed;
    default:
        ASSERT_NOT_REACHED();
        return RTCPeerConnectionHandlerClient::SignalingStateClosed;
    }
}

RTCPeerConnectionHandlerClient::IceGatheringState toWebKitIceGatheringState(webrtc::PeerConnectionInterface::IceGatheringState state)
{
    switch (state) {
    case webrtc::PeerConnectionInterface::kIceGatheringNew:
        return RTCPeerConnectionHandlerClient::IceGatheringStateNew;
    case webrtc::PeerConnectionInterface::kIceGatheringGathering:
        return RTCPeerConnectionHandlerClient::IceGatheringStateGathering;
    case webrtc::PeerConnectionInterface::kIceGatheringComplete:
        return RTCPeerConnectionHandlerClient::IceGatheringStateComplete;
    default:
        ASSERT_NOT_REACHED();
        return RTCPeerConnectionHandlerClient::IceGatheringStateNew;
    }
}

RTCPeerConnectionHandlerClient::IceConnectionState toWebKitIceConnectionState(webrtc::PeerConnectionInterface::IceConnectionState iceState)
{
    switch (iceState) {
    case webrtc::PeerConnectionInterface::kIceConnectionNew:
        return RTCPeerConnectionHandlerClient::IceConnectionStateNew;
    case webrtc::PeerConnectionInterface::kIceConnectionChecking:
        return RTCPeerConnectionHandlerClient::IceConnectionStateChecking;
    case webrtc::PeerConnectionInterface::kIceConnectionConnected:
        return RTCPeerConnectionHandlerClient::IceConnectionStateConnected;
    case webrtc::PeerConnectionInterface::kIceConnectionCompleted:
        return RTCPeerConnectionHandlerClient::IceConnectionStateCompleted;
    case webrtc::PeerConnectionInterface::kIceConnectionFailed:
        return RTCPeerConnectionHandlerClient::IceConnectionStateFailed;
    case webrtc::PeerConnectionInterface::kIceConnectionDisconnected:
        return RTCPeerConnectionHandlerClient::IceConnectionStateDisconnected;
    case webrtc::PeerConnectionInterface::kIceConnectionClosed:
        return RTCPeerConnectionHandlerClient::IceConnectionStateClosed;
    default:
        ASSERT_NOT_REACHED();
        return RTCPeerConnectionHandlerClient::IceConnectionStateClosed;
    }
}

webrtc::MediaStreamTrackInterface::TrackState toWebRTCTrackState(MediaStreamSource::ReadyState state)
{
    switch (state) {
    case MediaStreamSource::New:
        return webrtc::MediaStreamTrackInterface::kInitializing;
    case MediaStreamSource::Live:
        return webrtc::MediaStreamTrackInterface::kLive;
    case MediaStreamSource::Ended:
        return webrtc::MediaStreamTrackInterface::kEnded;
    default:
        ASSERT_NOT_REACHED();
        return webrtc::MediaStreamTrackInterface::kEnded;
    }
}

MediaStreamSource::ReadyState toWebKitReadyState(webrtc::MediaStreamTrackInterface::TrackState state)
{
    switch (state) {
    case webrtc::MediaStreamTrackInterface::kInitializing:
        return MediaStreamSource::New;
    case webrtc::MediaStreamTrackInterface::kLive:
        return MediaStreamSource::Live;
    case webrtc::MediaStreamTrackInterface::kEnded:
        return MediaStreamSource::Ended;
    default:
        ASSERT_NOT_REACHED();
        return MediaStreamSource::Ended;
    }
}

RTCDataChannelHandlerClient::ReadyState toWebKitDataChannelReadyState(webrtc::DataChannelInterface::DataState state)
{
    switch (state) {
    case webrtc::DataChannelInterface::kConnecting:
        return RTCDataChannelHandlerClient::ReadyStateConnecting;
    case webrtc::DataChannelInterface::kOpen:
        return RTCDataChannelHandlerClient::ReadyStateOpen;
    case webrtc::DataChannelInterface::kClosing:
        return RTCDataChannelHandlerClient::ReadyStateClosing;
    case webrtc::DataChannelInterface::kClosed:
        return RTCDataChannelHandlerClient::ReadyStateClosed;
    default:
        ASSERT_NOT_REACHED();
        return RTCDataChannelHandlerClient::ReadyStateClosed;
    }
}

} // namespace WebRTCUtils

} // namespace WebCore

#endif // ENABLE(MEDIA_STREAM) && USE(WEBRTCLIB)
