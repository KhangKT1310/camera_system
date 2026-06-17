# System Overview

## Livestream Path

`Camera Device -> Device Agent -> WebRTC Publisher -> Signaling Service -> Media Gateway/SFU -> Web/Mobile Viewer`

The device-side agent owns capture, encoding, session control, and security enforcement at the edge. Signaling and gateway services coordinate authenticated session establishment while keeping vendor-specific media details hidden behind the device platform layer.

## Playback Path

`Media Gateway/Recorder -> Recording Storage + Index -> Playback Service -> WebRTC Playback Session -> Viewer`

Playback is handled as an authorized workflow separate from live streaming. Recording storage and recording index services provide time-based retrieval, while playback sessions are delivered through WebRTC rather than exposing raw storage objects to clients.

## Boundary Rules

* `device-agent/core` contains portable logic only.
* `device-agent/platform/*` contains vendor or OS-specific capture, crypto, storage, and secure-element integrations.
* `device-agent/adapters/*` contains optional compatibility protocols that cannot bypass the core policy layer.
* Cloud services enforce identity, authorization, auditability, and OTA coordination.
