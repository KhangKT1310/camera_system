# Camera System

## Overview

This repository defines the architecture and compliance scaffolding for a surveillance camera system with WebRTC as the primary transport for livestream and playback. The current focus is architecture boundaries, compliance preparation, and cross-platform module layout rather than production business logic.

## Architecture Summary

The system is split into a device-side agent, cloud services, web clients, compliance documentation, and supporting tools. Core media, recording, security, and control logic are kept separate from vendor and chip-specific code so new hardware targets can be added without changing the security model.

## Livestream Flow Summary

Camera capture and encoding run on the device, the device agent publishes media through the WebRTC publisher, signaling coordinates session setup, and a media gateway or SFU relays streams to web or mobile viewers.

## Playback Flow Summary

Recorded segments are indexed separately from live transport, playback requests are authorized through cloud services, and playback is delivered to the client as a WebRTC session sourced from recording storage and index metadata.

## Protocol Policy

* WebRTC is primary for livestream and playback.
* RTSP is legacy and NVR compatibility only.
* RTMP or RTMPS is optional ingest or push only.
* ONVIF is discovery, device-management, PTZ, and event compatibility only.
* Compatibility protocols must stay isolated from the core media pipeline and cannot define the security baseline.

## Cross-Platform Design Rules

* Core code must remain portable and hardware-agnostic.
* Main repository is 100% open-source (MIT) and contains NO proprietary vendor SDK binaries, NDA headers, or closed-source libraries.
* Vendor integrations are maintained out-of-tree or as standalone HAL plugins linking via public C HAL vtables (`include/hal/*.h`).
* `device-agent/platform/` contains open-source reference implementations (`mock`, `linux_v4l2`, `common`) and vendor integration documentation.
* Platform capabilities and limitations must be documented alongside each platform target.

## Security-First Principles

* Review every implementation decision against QCVN 11:2026/BCA and [docs/compliance/48-bca.pdf](/home/khangkt/Workspace/Personal_Project/camera_system/docs/compliance/48-bca.pdf).
* Require unique device identity or credentials, changeable authentication values, encrypted transport, and signed updates.
* Keep debug and optional interfaces disabled by default.
* Track interfaces, threat assumptions, evidence, and user-data flows before a feature is considered complete.

## Recommended Phases

1. Secure WebRTC livestream MVP.
2. Playback using recording segments and index.
3. RTSP, ONVIF, and RTMP compatibility adapters.
4. OTA, compliance hardening, interface inventory, and evidence collection.
