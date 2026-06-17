# Device Agent

## Purpose

The device agent hosts portable edge logic for capture orchestration, WebRTC publishing, recording, playback access, security policy enforcement, and OTA coordination.

## What Belongs Here

* Core modules with platform-agnostic behavior.
* Platform adapters that implement hardware-specific contracts.
* Optional compatibility adapters and device-focused tests.

## What Must Not Be Placed Here

* Cloud-only business logic.
* Web client UI code.
* Direct vendor SDK calls outside `platform/`.

## Security and Compliance Notes

Device code is responsible for enforcing unique identity, secure transport, protected parameter storage, signed updates, and disabled-by-default optional interfaces in line with QCVN 11:2026/BCA.

## Cross-Platform Notes

Portable code belongs under `core/`, while OS, SoC, and SDK-specific behavior belongs under `platform/`. The boundary between them should be expressed through HAL-style interfaces before implementation begins.
