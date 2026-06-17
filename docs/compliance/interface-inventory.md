# Interface Inventory

Every new interface or protocol must be added here before implementation is considered complete.

| Interface | Protocol | Default State | Purpose | Auth Required | Encryption Required | WAN Exposure | Owner Module | Evidence Status |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| WebRTC media | WebRTC | Enabled | Primary live and playback media transport. | Yes | DTLS-SRTP | Controlled | `device-agent/core/webrtc_publisher` | Draft |
| WebRTC signaling API | HTTPS / WebSocket over TLS | Enabled | Session negotiation, ICE exchange, and viewer authorization handoff. | Yes | Yes | Controlled | `cloud/signaling-service` | Draft |
| Admin HTTP API | HTTPS | Enabled | Administrative control and device or tenant management. | Yes | Yes | Controlled | `cloud/auth-service` / `web/admin` | Draft |
| Playback API | HTTPS | Enabled | Recording search, authorization, and playback session creation. | Yes | Yes | Controlled | `cloud/playback-service` | Draft |
| OTA API | HTTPS | Enabled | Signed firmware or software manifest retrieval and update coordination. | Yes | Yes | Controlled | `cloud/ota-service` | Draft |
| RTSP adapter | RTSP / RTSPS | Disabled by default | Legacy NVR or VMS compatibility path. | Yes | Yes where supported | No by default | `device-agent/adapters/rtsp` | Draft |
| RTMP/RTMPS adapter | RTMP / RTMPS | Disabled by default | Optional ingest or push compatibility path. | Yes | Yes for RTMPS | No by default | `device-agent/adapters/rtmp` | Draft |
| ONVIF adapter | ONVIF over HTTP(S) / SOAP | Disabled by default | Discovery, device management, PTZ, and event compatibility. | Yes | Yes where supported | No by default | `device-agent/adapters/onvif` | Draft |
| Debug shell | Shell / serial / SSH | Disabled by default | Engineering diagnostics in non-production contexts only. | Yes | Yes for remote access | No by default | `device-agent/platform/common` | Draft |
| Local storage access | Filesystem / block storage | Enabled | Recording segments, indexes, audit data, and secure local state. | Yes | Required for sensitive data at rest | Local only | `device-agent/core/recording` / `cloud/recording-index` | Draft |
