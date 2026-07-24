# HAL Interfaces

This document defines the normative contract families that `device-agent/platform/*` must implement for `device-agent/core`.

## 1. Platform Identity

Purpose:
Expose device identity, boot state, and immutable platform facts used by security and OTA logic.

Required operations:

* `get_platform_info()`
* `get_device_identity()`
* `get_boot_integrity_state()`
* `get_provisioning_state()`

Rules:

* Identity values must be stable across reboot unless factory-reset policy explicitly says otherwise.
* Core must never read vendor-specific serial APIs directly.

## 2. Camera Capture

Purpose:
Provide discovery and control of video and optional audio sources.

Required operations:

* `list_video_inputs()`
* `open_video_stream(input_id, mode)`
* `read_video_frame(stream_id)`
* `close_video_stream(stream_id)`

Optional operations:

* `list_audio_inputs()`
* `open_audio_stream(input_id, mode)`

Rules:

* Timestamps must identify whether they are monotonic or wall-clock derived.
* Platform must surface dropped-frame and sensor-fault events.
* Raw capture access must stay inside the HAL boundary.

## 3. Encoder and Media Processing

Purpose:
Convert captured media into approved formats for livestream and recording.

Required operations:

* `list_encode_profiles()`
* `open_encoder(profile_id, params)`
* `submit_frame(encoder_id, frame)`
* `read_encoded_packet(encoder_id)`
* `request_keyframe(encoder_id)`
* `close_encoder(encoder_id)`

Rules:

* Encoded packet metadata must include codec, timestamps, keyframe marker, and duration if known.
* Platform must reject unsupported profiles explicitly rather than silently changing parameters.

## 4. Time and Clock

Purpose:
Support ordering, retention, replay safety, and auditability.

Required operations:

* `now_monotonic()`
* `now_utc()`
* `get_clock_sync_state()`

Rules:

* Recording and playback ordering must prefer monotonic time for local sequencing.
* UTC trust level must be exposed so core can detect unsynchronized clocks.

## 5. Secure Storage and Crypto

Purpose:
Protect long-term secrets and provide approved cryptographic helpers.

Required operations:

* `store_secret(handle, value, policy)`
* `load_secret(handle)`
* `delete_secret(handle)`
* `generate_random(bytes_len)`
* `verify_signature(artifact, signature, key_ref)`
* `get_trust_store_info()`

Optional operations:

* `generate_keypair(policy)`
* `sign_digest(key_ref, digest)`
* `seal_blob(policy, plaintext)`
* `unseal_blob(blob_ref)`

Rules:

* Secret material must not traverse logs or crash diagnostics.
* If no acceptable protected storage exists, secret-dependent features must remain disabled pending reviewed fallback.

## 6. Persistent Storage

Purpose:
Store recordings, metadata, config, and staged updates.

Required operations:

* `open_volume(volume_id, purpose)`
* `write_object(path, stream)`
* `read_object(path)`
* `delete_object(path)`
* `atomic_replace(path, stream)`
* `stat_capacity(volume_id)`
* `fsync_scope(scope_id)`

Rules:

* The interface must expose durability expectations, not raw filesystem details.
* Core owns retention policy; platform owns safe persistence semantics.

## 7. Network Primitives

Purpose:
Expose normalized transport helpers without coupling core to OS or vendor networking stacks.

Required operations:

* `list_network_interfaces()`
* `get_interface_state(interface_id)`
* `resolve_dns(name, policy)`
* `open_client_transport(params)`
* `get_tls_context(policy_ref)`

Rules:

* TLS trust decisions must be policy-driven and use the platform trust-store source.
* Core may not use this family to expose undocumented listener interfaces.

## 8. Update Installer and Recovery

Purpose:
Bridge portable OTA policy to platform-specific staging and activation mechanisms.

Required operations:

* `get_update_state()`
* `stage_update(artifact_ref, manifest_ref)`
* `validate_staged_update()`
* `activate_staged_update()`
* `get_recovery_status()`

Optional operations:

* `rollback_to_previous_slot()`
* `set_anti_rollback_marker(version)`

Rules:

* Platform must not activate an update that has not passed signature and integrity checks.
* Recovery paths must report whether rollback or manual intervention is required.

## 9. Device Control

Purpose:
Support bounded control operations needed by security and lifecycle management.

Required operations:

* `reboot(reason)`
* `factory_reset(mode)`
* `feed_watchdog()`

Optional operations:

* `set_status_indicator(state)`
* `move_ptz(command)`

Rules:

* Factory reset semantics must be documented and reviewed against credential and user-data deletion requirements.
* PTZ, if supported, must remain behind authorization in core services.

## 10. Observability

Purpose:
Provide structured health, audit, and metrics hooks.

Required operations:

* `emit_health_event(event)`
* `emit_audit_event(event)`
* `record_metric(sample)`
* `get_last_reset_reason()`

Rules:

* Event payloads must exclude secrets and minimize personal data.
* Audit sinks must remain available for security-relevant failures even when optional features are disabled.
