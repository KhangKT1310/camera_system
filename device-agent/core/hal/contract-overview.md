# HAL Contract Overview

## Goal

Define one portable contract surface between `device-agent/core` and `device-agent/platform/*` before implementing WebRTC publishing, playback, or optional protocol adapters.

## Dependency Direction

* `device-agent/core/*` may depend on HAL interfaces only.
* `device-agent/platform/*` may depend on vendor SDKs, OS APIs, kernel interfaces, and board support packages.
* `device-agent/platform/*` must implement HAL interfaces and return normalized capabilities, errors, and events.
* `device-agent/adapters/*` must consume core services and may not call platform implementations directly.

## Core-to-Platform Contract Families

| Contract Family | Consumed By Core Modules | Implemented By Platform | Purpose |
| --- | --- | --- | --- |
| Device identity and provisioning | `security`, `ota_client` | `platform/*` | Expose stable device identity, boot state, and ownership bootstrap hooks. |
| Camera capture | `media_pipeline` | `platform/*` | Discover sensors and produce frame streams or compressed elementary streams. |
| Encoder and media processing | `media_pipeline`, `recording` | `platform/*` | Configure encode profiles and transform media into recordable or publishable output. |
| Time and clock | `recording`, `playback_reader`, `security` | `platform/*` | Provide trusted monotonic and wall-clock sources. |
| Secure storage and crypto | `security`, `ota_client` | `platform/*` | Protect keys, secrets, and verification primitives. |
| Persistent storage | `recording`, `ota_client` | `platform/*` | Write segments, indexes, config, and update staging data with integrity-aware semantics. |
| Network primitives | `webrtc_publisher`, `ota_client` | `platform/*` | Provide sockets, interface info, DNS, and transport helpers without exposing vendor details. |
| Update installer and recovery | `ota_client` | `platform/*` | Stage, verify handoff, activate, rollback, and report recovery status. |
| Device control | `security`, future admin hooks | `platform/*` | Reboot, factory reset, LED or GPIO indicators, watchdog, and optional PTZ controls. |
| Observability | all core modules | `platform/*` | Health, metrics, structured logging sinks, and audit event emission hooks. |

## Non-Goals

* No direct HAL contract for RTSP, RTMP, or ONVIF.
* No vendor-specific type names in core-visible APIs.
* No assumption that every platform supports hardware-backed security; unsupported capabilities must be explicit and policy-gated.

## Required Design Rules

* Platform implementations must be replaceable without changing core business logic.
* Core modules must branch on normalized capabilities, not vendor names.
* Any platform feature that weakens the security baseline must be represented as unsupported by default.
* A missing required capability must stop dependent features rather than silently degrading to an insecure mode.
