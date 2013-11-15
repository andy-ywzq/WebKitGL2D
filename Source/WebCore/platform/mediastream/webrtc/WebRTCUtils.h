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

#ifndef WebRTCUtils_h
#define WebRTCUtils_h

#if ENABLE(MEDIA_STREAM) && USE(WEBRTCLIB)

#include "MediaConstraints.h"
#include "MediaStreamSource.h"
#include "RTCDataChannelHandlerClient.h"
#include "RTCPeerConnectionHandlerClient.h"
#include "libwebrtc.h"
#include <wtf/Vector.h>

namespace WebCore {

class RTCConfiguration;

namespace WebRTCUtils {

void toMediaConstraintsWebRTC(const WTF::Vector<MediaConstraint>, webrtc::MediaConstraintsInterface::Constraints*);
void toWebRTCIceServers(PassRefPtr<RTCConfiguration>, webrtc::PeerConnectionInterface::IceServers*);
RTCDataChannelHandlerClient::ReadyState toWebKitDataChannelReadyState(webrtc::DataChannelInterface::DataState);
RTCPeerConnectionHandlerClient::SignalingState toWebKitSignalingState(webrtc::PeerConnectionInterface::SignalingState);
RTCPeerConnectionHandlerClient::IceGatheringState toWebKitIceGatheringState(webrtc::PeerConnectionInterface::IceGatheringState);
RTCPeerConnectionHandlerClient::IceConnectionState toWebKitIceConnectionState(webrtc::PeerConnectionInterface::IceConnectionState);
webrtc::MediaStreamTrackInterface::TrackState toWebRTCTrackState(MediaStreamSource::ReadyState);
MediaStreamSource::ReadyState toWebKitReadyState(webrtc::MediaStreamTrackInterface::TrackState);

} // namespace WebRTCUtils

} // namespace WebCore
#endif // ENABLE(MEDIA_STREAM) && USE(WEBRTCLIB)

#endif // WebRTCUtils_h
