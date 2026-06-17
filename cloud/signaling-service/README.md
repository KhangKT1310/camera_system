# Signaling Service

## Purpose

This service will coordinate authenticated WebRTC session negotiation between viewers and device or playback publishers.

## What Belongs Here

* Session negotiation APIs.
* ICE and SDP exchange orchestration.
* Authorization integration points and session-state policy.

## What Must Not Be Placed Here

* Long-term media storage logic.
* Vendor-specific transport code.
* Unauthenticated control flows.

## Security and Compliance Notes

Signaling is a sensitive control-plane interface and must use TLS, authenticated requests, bounded inputs, and audit-friendly session handling consistent with QCVN 11:2026/BCA.

## Cross-Platform Notes

The service should treat devices uniformly and avoid assumptions tied to a single hardware platform or SDK.
