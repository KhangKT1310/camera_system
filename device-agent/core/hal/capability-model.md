# HAL Capability Model

## Purpose

Capabilities are the only supported way for `device-agent/core` to discover what a platform can do. Core logic must not infer behavior from vendor, board, or kernel identifiers.

## Capability Advertisement Rules

Each platform adapter must expose a single capability snapshot at startup and refresh any dynamic fields through explicit events.

Required properties:

| Area | Required Capability Fields |
| --- | --- |
| Platform identity | `platform_name`, `platform_version`, `board_model`, `firmware_version` |
| Security | `secure_storage`, `hardware_root_of_trust`, `csrng`, `device_identity_type`, `verified_boot_state` |
| Media input | `video_inputs[]`, `audio_inputs[]`, supported pixel formats, max resolutions, frame-rate ranges |
| Encoding | supported codecs, profiles, bitrate ranges, keyframe control, timestamp model |
| Storage | local volumes, capacity reporting, atomic rename support, fsync behavior, encryption-at-rest support |
| Network | interface inventory, IPv4 or IPv6 support, TLS trust-store source, MTU hints |
| Update | A/B support, rollback support, anti-rollback support, signed-manifest support |
| Control | watchdog availability, reboot control, factory-reset support, PTZ support if present |
| Diagnostics | crash log support, debug-interface state, metrics support |

## Capability Classification

Use one of these states for each optional capability:

* `required`: must exist or the platform is not eligible for the product profile.
* `supported`: available and usable by the core.
* `unsupported`: intentionally absent or unavailable.
* `restricted`: available only under documented policy constraints.

## Product Profile Rules

Minimum expected capabilities for the first secure livestream profile:

* `csrng`: `required`
* `device_identity_type`: `required`
* `secure_storage`: `supported` or `restricted` with documented fallback review
* `verified_boot_state`: `supported` when hardware allows; otherwise document gap
* video input discovery: `required`
* encoder support for at least one approved livestream format: `required`
* monotonic clock: `required`
* TLS trust-store source: `required`

## Security Constraints

* If `secure_storage` is `unsupported`, the platform must not store long-term secrets until a reviewed fallback is approved.
* If `signed-manifest support` is `unsupported`, OTA must remain disabled.
* If `debug-interface state` cannot confirm disabled-by-default production behavior, the platform cannot be production-qualified.

## Compatibility Rule

Capabilities must be versioned. Any backward-incompatible change to fields or semantics requires a HAL contract version bump and a review of all core consumers.
