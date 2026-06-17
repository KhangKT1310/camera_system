# Threat Model

## Assets

* Device identity, credentials, and signing trust anchors.
* Live media, recordings, playback indexes, and audit evidence.
* User accounts, admin actions, and privacy-sensitive metadata.

## Trust Boundaries

* Camera hardware and vendor SDK boundary.
* Device-agent core versus platform and compatibility adapters.
* Device-to-cloud transport boundary.
* Cloud control plane versus viewer applications.
* Local storage and secure storage boundary.

## Main Actors

* Authorized end users and administrators.
* Device provisioning and support operators.
* Cloud services that coordinate signaling, auth, playback, OTA, and audit.
* Local or remote attackers targeting interfaces, data, or update paths.

## Attack Surfaces

* WebRTC signaling and media setup.
* Admin, playback, and OTA APIs.
* Optional RTSP, RTMP, and ONVIF adapters.
* Local debug, storage, and provisioning interfaces.
* Firmware, manifests, and platform integration points.

## Threat Scenarios

* Unauthorized stream access through weak identity, session handling, or exposed compatibility adapters.
* Credential theft from plaintext storage, logs, crash data, or provisioning shortcuts.
* Malicious update delivery through unsigned artifacts, downgraded trust, or recovery abuse.
* Sensitive recording leakage through index exposure, stale segments, or incomplete deletion.
* Platform escape where vendor SDK code bypasses core security controls.

## Required Mitigations

* Unique device identity, changeable authentication values, and strict authorization checks.
* DTLS-SRTP for media and TLS for signaling, APIs, OTA, and cloud communication.
* Secure storage abstraction with platform capability documentation.
* Signed-update verification, rollback strategy, and fail-closed recovery design.
* Disabled-by-default optional interfaces and production-disabled debug paths.

## Open Security Questions

* Which platform targets provide hardware-backed key storage versus software-only storage.
* How device provisioning will bind identity, ownership, and certificate lifecycle.
* Which retention and deletion guarantees apply per deployment and storage backend.
* What Vietnam data-locality options are required per customer or service tier.

## Review History

| Date | Change | Owner | Status |
| --- | --- | --- | --- |
| 2026-06-17 | Initial template created for architecture and compliance planning. | Codex | Draft |
