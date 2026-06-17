# Compatibility Adapters

## Purpose

This directory isolates optional compatibility protocols that are not part of the primary WebRTC architecture.

## What Belongs Here

* RTSP, RTMP, and ONVIF adapters.
* Shared adapter-facing policies and capability checks.
* Documentation for default-disabled behavior and exposure constraints.

## What Must Not Be Placed Here

* Core media-pipeline ownership.
* Vendor SDK bindings that belong in `platform/`.
* Any assumption that these protocols are required for primary product operation.

## Security and Compliance Notes

Compatibility adapters are optional and must remain disabled by default unless explicitly enabled. They cannot lower the authentication, encryption, or logging standards enforced by the core system.

## Cross-Platform Notes

Adapters should consume portable core and platform contracts so they remain reusable across hardware targets without embedding vendor assumptions.
