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

#ifndef RTCPeerConnectionHandlerWebRTC_h
#define RTCPeerConnectionHandlerWebRTC_h

#if ENABLE(MEDIA_STREAM) && USE(WEBRTCLIB)

#include "RTCPeerConnectionHandler.h"
#include "RTCSessionDescriptionDescriptor.h"
#include "libwebrtc.h"
#include "observers/CreateSessionDescriptionObserver.h"
#include "observers/GetStatsObserver.h"
#include "observers/RTCPeerConnectionObserver.h"
#include "observers/SetSessionDescriptionObserver.h"
#include "talk/app/webrtc/peerconnectioninterface.h"

namespace WebCore {

class RTCPeerConnectionHandlerWebRTC : public RTCPeerConnectionHandler {
public:
    virtual bool initialize(PassRefPtr<RTCConfiguration>, PassRefPtr<MediaConstraints>) OVERRIDE;
    virtual void createOffer(PassRefPtr<RTCSessionDescriptionRequest>, PassRefPtr<MediaConstraints>) OVERRIDE;
    virtual void createAnswer(PassRefPtr<RTCSessionDescriptionRequest>, PassRefPtr<MediaConstraints>) OVERRIDE;
    virtual void setLocalDescription(PassRefPtr<RTCVoidRequest>, PassRefPtr<RTCSessionDescriptionDescriptor>) OVERRIDE;
    virtual void setRemoteDescription(PassRefPtr<RTCVoidRequest>, PassRefPtr<RTCSessionDescriptionDescriptor>) OVERRIDE;
    virtual PassRefPtr<RTCSessionDescriptionDescriptor> localDescription() OVERRIDE;
    virtual PassRefPtr<RTCSessionDescriptionDescriptor> remoteDescription() OVERRIDE;
    virtual bool updateIce(PassRefPtr<RTCConfiguration>, PassRefPtr<MediaConstraints>) OVERRIDE;
    virtual bool addIceCandidate(PassRefPtr<RTCVoidRequest>, PassRefPtr<RTCIceCandidateDescriptor>) OVERRIDE;
    virtual bool addStream(PassRefPtr<MediaStreamDescriptor>, PassRefPtr<MediaConstraints>) OVERRIDE;
    virtual void removeStream(PassRefPtr<MediaStreamDescriptor>) OVERRIDE;
    virtual void getStats(PassRefPtr<RTCStatsRequest>) OVERRIDE;
    virtual PassOwnPtr<RTCDataChannelHandler> createDataChannel(const String&, const RTCDataChannelInit&) OVERRIDE;
    virtual PassOwnPtr<RTCDTMFSenderHandler> createDTMFSender(PassRefPtr<MediaStreamSource>) OVERRIDE;
    virtual void stop() OVERRIDE;
    RTCPeerConnectionHandlerWebRTC(RTCPeerConnectionHandlerClient*);
    ~RTCPeerConnectionHandlerWebRTC() { }
private:
    bool createPeerConnection(const webrtc::PeerConnectionInterface::IceServers&, const webrtc::MediaConstraintsInterface&);
    webrtc::MediaStreamTrackInterface* getWebRTCMediaStreamTrack(PassRefPtr<RTCStatsRequest>);
    webrtc::SessionDescriptionInterface* getWebRTCSessionDescription(PassRefPtr<RTCVoidRequest>, PassRefPtr<RTCSessionDescriptionDescriptor>);
    void addWebRTCStream(webrtc::MediaStreamInterface*, MediaStreamSource*);
    webrtc::MediaStreamInterface* getWebRTCMediaStream(const std::string&);

    RTCPeerConnectionObserver m_connectionObserver;
    talk_base::scoped_refptr<webrtc::PeerConnectionInterface> m_webRTCPeerConnection;
    talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> m_pcFactory;
    talk_base::scoped_refptr<CreateSessionDescriptionObserver> m_createSessionObserver;
    talk_base::scoped_refptr<SetSessionDescriptionObserver> m_setSessionObserver;
    talk_base::scoped_refptr<GetStatsObserver> m_getStatsObserver;

    RefPtr<RTCSessionDescriptionDescriptor> m_localSessionDescription;
    RefPtr<RTCSessionDescriptionDescriptor> m_remoteSessionDescription;
};

} // namespace WebCore

#endif // #if ENABLE(MEDIA_STREAM) && USE(WEBRTCLIB)

#endif // RTCPeerConnectionHandlerWebRTC_h
