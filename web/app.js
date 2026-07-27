/**
 * WebRTC Live Viewer Client Application
 */
class WebRtcViewer {
  constructor() {
    this.ws = null;
    this.pc = null;
    this.statsInterval = null;
    this.pendingCandidates = [];

    // DOM Elements
    this.remoteVideo = document.getElementById('remoteVideo');
    this.videoOverlay = document.getElementById('videoOverlay');
    this.overlayStatus = document.getElementById('overlayStatus');
    this.spinner = this.videoOverlay.querySelector('.spinner');

    this.serverUriInput = document.getElementById('serverUri');
    this.sessionIdInput = document.getElementById('sessionId');
    this.btnConnect = document.getElementById('btnConnect');
    this.btnDisconnect = document.getElementById('btnDisconnect');

    this.connectionBadge = document.getElementById('connectionBadge');
    this.valIceState = document.getElementById('valIceState');
    this.valWsState = document.getElementById('valWsState');
    this.valPacketsRecv = document.getElementById('valPacketsRecv');
    this.valFramesDecoded = document.getElementById('valFramesDecoded');
    this.valResolution = document.getElementById('valResolution');
    this.logConsole = document.getElementById('logConsole');

    this.initEvents();
  }

  initEvents() {
    this.btnConnect.addEventListener('click', () => this.connect());
    this.btnDisconnect.addEventListener('click', () => this.disconnect());

    this.remoteVideo.addEventListener('loadedmetadata', () => {
      this.valResolution.textContent = `${this.remoteVideo.videoWidth}x${this.remoteVideo.videoHeight}`;
      this.log(`Video metadata loaded: ${this.remoteVideo.videoWidth}x${this.remoteVideo.videoHeight}`);
    });
  }

  log(msg) {
    const ts = new Date().toISOString().split('T')[1].slice(0, 8);
    const entry = document.createElement('div');
    entry.textContent = `[${ts}] ${msg}`;
    this.logConsole.appendChild(entry);
    this.logConsole.scrollTop = this.logConsole.scrollHeight;
  }

  updateBadge(status, text) {
    this.connectionBadge.className = `badge badge-${status}`;
    this.connectionBadge.innerHTML = `<span class="status-dot"></span> ${text}`;
  }

  async connect() {
    const wsUri = this.serverUriInput.value.trim();
    const sessionId = this.sessionIdInput.value.trim();

    if (!wsUri || !sessionId) {
      alert('Please enter WebSocket URI and Session ID');
      return;
    }

    this.btnConnect.disabled = true;
    this.btnDisconnect.disabled = false;
    this.spinner.style.display = 'block';
    this.overlayStatus.textContent = 'Connecting to signaling server...';
    this.updateBadge('connecting', 'CONNECTING');

    const fullWsUrl = `${wsUri}?role=viewer&session_id=${sessionId}`;
    this.log(`Connecting WS: ${fullWsUrl}`);

    try {
      this.ws = new WebSocket(fullWsUrl);

      this.ws.onopen = () => {
        this.log('WebSocket signaling connected');
        this.valWsState.textContent = 'CONNECTED';
        this.createPeerConnection(sessionId);
      };

      this.ws.onmessage = async (event) => {
        try {
          const envelope = JSON.parse(event.data);
          this.handleSignalingMessage(envelope);
        } catch (err) {
          this.log(`Error parsing WebSocket message: ${err.message}`);
        }
      };

      this.ws.onclose = () => {
        this.log('WebSocket signaling disconnected');
        this.valWsState.textContent = 'DISCONNECTED';
      };

      this.ws.onerror = (err) => {
        this.log(`WebSocket error: ${err.message || 'connection failed'}`);
      };
    } catch (err) {
      this.log(`Connection error: ${err.message}`);
      this.disconnect();
    }
  }

  createPeerConnection(sessionId) {
    this.pendingCandidates = [];
    const rtcConfig = {
      iceServers: [
        { urls: 'stun:stun.l.google.com:19302' }
      ]
    };

    this.pc = new RTCPeerConnection(rtcConfig);

    this.pc.onicecandidate = (event) => {
      if (event.candidate) {
        this.sendEnvelope('CANDIDATE', sessionId, {
          candidate: event.candidate.candidate,
          sdpMid: event.candidate.sdpMid
        });
      }
    };

    this.pc.onconnectionstatechange = () => {
      this.log(`ICE Connection state: ${this.pc.connectionState}`);
      this.valIceState.textContent = this.pc.connectionState;

      if (this.pc.connectionState === 'connected') {
        this.updateBadge('connected', 'LIVE STREAM');
        this.videoOverlay.style.display = 'none';
        this.startStatsMonitoring();
      } else if (this.pc.connectionState === 'failed' || this.pc.connectionState === 'closed') {
        this.updateBadge('failed', 'DISCONNECTED');
        this.videoOverlay.style.display = 'flex';
        this.overlayStatus.textContent = 'Stream Disconnected';
      }
    };

    this.pc.ontrack = (event) => {
      this.log(`Remote track received: ${event.track.kind}`);
      if (event.streams && event.streams[0]) {
        this.remoteVideo.srcObject = event.streams[0];
      } else {
        const stream = new MediaStream();
        stream.addTrack(event.track);
        this.remoteVideo.srcObject = stream;
      }
    };

    // Create SDP Offer from Viewer
    this.createOffer(sessionId);
  }

  async createOffer(sessionId) {
    try {
      this.pc.addTransceiver('video', { direction: 'recvonly' });
      const offer = await this.pc.createOffer();
      await this.pc.setLocalDescription(offer);

      this.log('Local SDP Offer created and set');
      this.sendEnvelope('OFFER', sessionId, { sdp: offer.sdp });
    } catch (err) {
      this.log(`Create Offer error: ${err.message}`);
    }
  }

  async handleSignalingMessage(envelope) {
    const { type, payload } = envelope;
    this.log(`Received Signaling: ${type}`);

    if (type === 'ANSWER') {
      let remoteSdp = payload;
      if (typeof payload === 'string') {
        try {
          const parsed = JSON.parse(payload);
          remoteSdp = parsed.sdp || payload;
        } catch(e) {
          remoteSdp = payload;
        }
      } else if (payload && payload.sdp) {
        remoteSdp = payload.sdp;
      }

      try {
        await this.pc.setRemoteDescription(new RTCSessionDescription({
          type: 'answer',
          sdp: remoteSdp
        }));
        this.log('Remote SDP Answer applied successfully');
        this.updateBadge('connected', 'SIGNALING COMPLETE');
        this.overlayStatus.textContent = 'Signaling Negotiation Complete';
        this.spinner.style.display = 'none';

        // Flush queued candidates once remote description is set
        for (const cand of this.pendingCandidates) {
          try {
            await this.pc.addIceCandidate(new RTCIceCandidate(cand));
            this.log('Queued ICE Candidate added successfully');
          } catch (e) {
            this.log(`Candidate note: ${e.message}`);
          }
        }
        this.pendingCandidates = [];
      } catch(err) {
        this.log(`SDP Answer Error: ${err.message}`);
      }

    } else if (type === 'OFFER') {
      let remoteSdp = payload;
      if (typeof payload === 'string') {
        try {
          const parsed = JSON.parse(payload);
          remoteSdp = parsed.sdp || payload;
        } catch(e) {}
      } else if (payload && payload.sdp) {
        remoteSdp = payload.sdp;
      }

      try {
        await this.pc.setRemoteDescription(new RTCSessionDescription({
          type: 'offer',
          sdp: remoteSdp
        }));
        const answer = await this.pc.createAnswer();
        await this.pc.setLocalDescription(answer);
        this.sendEnvelope('ANSWER', envelope.session_id, { sdp: answer.sdp });
      } catch(err) {
        this.log(`SDP Offer Error: ${err.message}`);
      }

    } else if (type === 'CANDIDATE') {
      let candObj = payload;
      if (typeof payload === 'string') {
        try { candObj = JSON.parse(payload); } catch(e) {}
      }
      if (candObj && candObj.candidate) {
        if (!this.pc || !this.pc.remoteDescription) {
          this.pendingCandidates.push(candObj);
          this.log('ICE Candidate queued until remote description is set');
        } else {
          try {
            await this.pc.addIceCandidate(new RTCIceCandidate(candObj));
            this.log('ICE Candidate added successfully');
          } catch(e) {
            this.log(`Candidate note: ${e.message}`);
          }
        }
      }
    }
  }

  sendEnvelope(type, sessionId, payload) {
    if (this.ws && this.ws.readyState === WebSocket.OPEN) {
      const envelope = {
        version: 1,
        type: type,
        session_id: sessionId,
        request_id: `req-${Date.now()}`,
        timestamp: Math.floor(Date.now() / 1000),
        payload: payload
      };
      this.ws.send(JSON.stringify(envelope));
    }
  }

  startStatsMonitoring() {
    if (this.statsInterval) clearInterval(this.statsInterval);
    this.statsInterval = setInterval(async () => {
      if (!this.pc) return;
      const stats = await this.pc.getStats();
      stats.forEach(report => {
        if (report.type === 'inbound-rtp' && report.kind === 'video') {
          this.valPacketsRecv.textContent = report.packetsReceived || 0;
          this.valFramesDecoded.textContent = report.framesDecoded || 0;
        }
      });
    }, 1000);
  }

  disconnect() {
    if (this.statsInterval) clearInterval(this.statsInterval);
    if (this.pc) {
      this.pc.close();
      this.pc = null;
    }
    if (this.ws) {
      this.ws.close();
      this.ws = null;
    }

    this.remoteVideo.srcObject = null;
    this.btnConnect.disabled = false;
    this.btnDisconnect.disabled = true;
    this.spinner.style.display = 'none';
    this.overlayStatus.textContent = 'Disconnected';
    this.updateBadge('idle', 'IDLE');
    this.valIceState.textContent = 'closed';
    this.valWsState.textContent = 'DISCONNECTED';
    this.log('Disconnected stream and signaling');
  }
}

document.addEventListener('DOMContentLoaded', () => {
  window.app = new WebRtcViewer();
});
