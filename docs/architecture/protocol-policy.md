# Protocol Policy

## Primary Transport

WebRTC is the primary protocol for live viewing and playback. It is the default path for user-facing media delivery and should receive the first implementation and security-hardening effort.

## Compatibility Layers

RTSP, RTMP, and ONVIF are compatibility layers only. They exist to support legacy integration, ingest or push workflows, and device-management interoperability, but they must not define the primary trust model or core media architecture.

## Isolation Rules

* Compatibility protocols must remain isolated from the core media pipeline.
* Optional adapters must be disabled by default and explicitly inventoried.
* Authentication, encryption, logging, and authorization policy must be enforced by shared core controls rather than duplicated per vendor SDK.
