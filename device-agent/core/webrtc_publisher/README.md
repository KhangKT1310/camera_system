# WebRTC Publisher

## Purpose

This module will own the primary live and playback transport path from the device side using WebRTC.

## What Belongs Here

* Session setup contracts.
* Publisher state management.
* ICE, DTLS-SRTP, and media-session policy integration points.

## What Must Not Be Placed Here

* Legacy compatibility protocol logic.
* Hardcoded TURN, credential, or certificate material.
* Vendor-specific network primitives that belong in `platform/`.

## Security and Compliance Notes

DTLS-SRTP is mandatory for media, signaling dependencies must use TLS, and session establishment must rely on authenticated and authorized control flows consistent with QCVN 11:2026/BCA.

## Cross-Platform Notes

Transport logic should stay portable while socket tuning, hardware offload, or certificate storage details are delegated to platform adapters where needed.
