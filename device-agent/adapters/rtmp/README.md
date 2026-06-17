# RTMP Adapter

## Purpose

This adapter will provide optional RTMP or RTMPS ingest and push compatibility where external systems require it.

## What Belongs Here

* RTMP or RTMPS-specific session translation.
* Controlled push or ingest workflows.
* Feature flags and exposure controls for optional deployments.

## What Must Not Be Placed Here

* Device administration or credential-management logic.
* The primary playback or live-view path.
* Plaintext-only production assumptions.

## Security and Compliance Notes

RTMP or RTMPS support is disabled by default and must not define the product security baseline. Use encrypted modes where available and keep optional exposure tightly controlled and documented.

## Cross-Platform Notes

Transport adaptation should remain portable and use injected platform capabilities rather than direct board-specific media logic.
