# AGENTS.md — Mandatory Rules for WebRTC Surveillance Camera System

## Absolute Priority

This project builds a surveillance camera system using WebRTC as the primary stack. The system MUST comply with QCVN 11:2026/BCA / 48-bca.pdf in every implementation decision.

If a user request, shortcut, bug fix, prototype, test bypass, dependency, or architectural choice conflicts with QCVN 11:2026/BCA, STOP immediately and explain the conflict. Do not implement the change until the requirement is clarified and a compliant design is selected.

Compliance is not optional. Security requirements must not be postponed as "later", "temporary", "prototype only", "debug only", or "internal only" unless the code is impossible to ship in production and is clearly isolated behind test-only build flags.

## Source of Truth

* Treat `docs/compliance/48-bca.pdf` or the provided QCVN 11:2026/BCA document as the highest-priority compliance source.
* When uncertain, choose the stricter security interpretation.
* If the requirement is ambiguous, ask for clarification before coding.
* Every protocol, interface, credential, update flow, storage flow, and user-data flow must be traceable to compliance evidence.

## Architecture Rules

* WebRTC is the primary live-streaming and playback stack.
* RTSP, RTMP, and ONVIF must be implemented only as optional compatibility adapters.
* Compatibility adapters must be isolated from the core media pipeline.
* Unused network interfaces, logical interfaces, debug interfaces, ports, APIs, and protocol endpoints must be disabled by default.
* Do not expose any management, debug, or sensitive endpoint without authentication.
* Do not add public WAN exposure for RTSP, RTMP, ONVIF, SSH, Telnet, debug HTTP, or vendor shell endpoints.

## Commit Rules

Every commit must follow these rules:

1. Use small, focused commits.
   - One logical change per commit.
   - Do not mix unrelated refactors, feature work, ormatting, and compliance changes.

2. Use Conventional Commit style:

   - `docs:`
   - `chore:`
   - `feat:`
   - `fix:`
   - `refactor:`
   - `test:`
   - `build:`
   - `ci:`
   - `security:`
   - `compliance:`

   Examples:

   - `docs: add HAL contract draft`
   - `compliance: add interface inventory template`
   - `security: document secure storage requirements`
   - `feat: add mock camera capture HAL`
   - `test: add HAL capability detection tests`

3. Commit message subject:
   - Must be clear and imperative.
   - Prefer 72 characters or less.
   - Must not be vague, for example: `update`, `fix stuff`, `wip`, `misc`.

4. Security-sensitive commits must include a body explaining:
   - What security area changed.
   - Which interface/protocol/user-data flow is affected.
   - Which compliance document was updated.
   - What tests or checks were performed.

5. Do not commit:
   - Production credentials.
   - Private keys.
   - Tokens.
   - Certificates with private material.
   - Default passwords.
   - Recorded video samples containing real users.
   - Sensitive logs.
   - Build artifacts.
   - Temporary debug binaries.
   - Vendor SDK blobs unless explicitly approved.

6. Any commit adding or changing a protocol/interface must update:

   - `docs/compliance/interface-inventory.md`

7. Any commit affecting security, privacy, user data, authentication, encryption, OTA, factory reset, debug interface, storage, or playback access must update at least one relevant compliance evidence document.

8. Do not mark a commit as compliance-complete unless evidence exists.
   Use `Draft`, `TODO`, or `Not Started` where evidence is incomplete.
```

## Credential and Authentication Rules

* Never implement default shared passwords.
* Never hardcode production credentials, keys, tokens, certificates, private keys, salts, or shared secrets.
* Each device must use unique per-device credentials or identity material.
* The user/admin must be able to change authentication values.
* Any security-sensitive change must require prior authentication and authorization.
* Passwords and credentials must never be logged, printed, included in crash dumps, stored in plaintext, or sent over plaintext channels.
* Factory reset must remove user credentials, user configuration, pairing tokens, local user data, and session material. Immutable device identity may be preserved only if this is part of the secure provisioning design and is documented.

## Cryptography and Secure Communication Rules

* Use secure cryptography suitable for the use case.
* WebRTC media must use DTLS-SRTP.
* HTTP APIs, signaling, OTA, provisioning, cloud communication, and user sessions must use TLS.
* RTSP/ONVIF/RTMP compatibility must use authenticated and secure modes where supported. If a legacy insecure mode is allowed for compatibility, it must be disabled by default, clearly documented, visibly warned, and not exposed to WAN.
* Use a cryptographically secure random number generator for tokens, nonces, passwords, keys, and pairing codes.
* Do not invent custom cryptography.
* Do not weaken certificate validation.
* Do not accept self-signed or pinned certificates unless the trust model is explicit, reviewed, and documented.

## Sensitive Security Parameter Storage

* Store sensitive security parameters in a protected key store, secure element, TEE, TPM, SoC secure storage, or the strongest available platform mechanism.
* Platform-specific secure storage must be abstracted behind a cross-platform security interface.
* Do not store secrets in source code, config files, world-readable files, unencrypted SQLite databases, localStorage, browser logs, or plain environment dumps.
* Security-critical public parameters must be protected against unauthorized modification.
* Integrity checks must be applied to firmware, updates, security configuration, and playback index metadata where applicable.

## Update and OTA Rules

* All firmware and software updates must be signed.
* The device must verify update authenticity and integrity before applying.
* The update process must support safe failure handling and rollback or recovery.
* The user must be notified when security updates are available or installed.
* The project must maintain a clear support period per device model.
* Update logic must not allow downgrade attacks unless explicitly approved with anti-rollback protection.
* Do not implement OTA over plaintext transport.

## Interface and Debug Rules

* Maintain an interface inventory for every network, logical, physical, and debug interface.
* Every interface must have: purpose, default state, authentication model, exposed data, protocol, port, and enable/disable control.
* Debug interfaces must be disabled in production builds by default.
* Debug access must not be reachable over normal user networks unless explicitly designed, authenticated, authorized, audited, and documented.
* Do not add Telnet, unauthenticated shell, hidden debug HTTP routes, test APIs, or vendor backdoors.
* Any temporary debug code must be guarded by non-production build flags and removed before release.

## User Data and Privacy Rules

* Document all sensing capabilities: video, audio, motion, telemetry, location, biometric, or any other environmental sensing.
* The user must be informed clearly about what data is collected, why it is collected, how it is processed, where it is stored, and how long it is retained.
* The system must support user consent where required.
* The system must support revoking consent where applicable.
* The system must support deletion of user data from the device.
* Remote telemetry must be minimal, documented, and disable-able unless essential for security or operation.
* The system must support a configuration option for storing required user data in Vietnam.
* Recorded video, playback index, snapshots, and sensitive metadata must be access-controlled and encrypted at rest when stored outside volatile memory.

## Input Validation Rules

* Validate all input from users, APIs, WebRTC signaling, RTSP, RTMP, ONVIF, IPC, files, firmware update packages, playback timeline queries, device provisioning, and cloud messages.
* Reject malformed, oversized, unauthenticated, unauthorized, replayed, expired, or inconsistent input.
* Do not trust device-side values received from the network.
* Do not parse protocol messages without size limits, timeout limits, and error handling.
* Add fuzz, unit, and integration tests for parsers and security-sensitive inputs.

## Playback and Recording Rules

* Playback access must require authentication and authorization.
* Playback seek requests must check camera ownership, user permissions, time range, retention policy, and deletion status.
* Recording segments and indexes must not leak sensitive metadata without authorization.
* Retention and deletion must be enforced consistently across segment files, indexes, thumbnails, and cloud copies.
* Do not expose raw file paths or storage keys to clients.

## Cross-Platform Rules

* Keep the core system portable.
* All chip/vendor-specific logic must go through HAL/adapters.
* Do not call vendor SDK APIs directly from business logic.
* Required abstraction layers: camera capture, encoder, audio, storage, network, secure storage, crypto, time, update, reboot/recovery, GPIO/PTZ if any.
* Each platform adapter must include capability detection and a compliance note describing what security primitives are available.
* New chip support must not change the core security model.

## Protocol Policy

* WebRTC is the default live and playback transport.
* RTSP is for legacy NVR/VMS compatibility or migration only.
* ONVIF is for discovery, device management, PTZ, time, event, and profile compatibility only.
* RTMP/RTMPS is optional external ingest/push only; do not use it for device administration.
* Every enabled protocol must be documented in the interface inventory and must pass authentication, encryption, logging, and disable-by-default checks.

## Evidence and Documentation Rules

For every feature or protocol change, update the compliance evidence:

* Interface inventory.
* Threat model.
* Data-flow diagram.
* Credential and key management notes.
* Update and rollback notes.
* User-data and sensing documentation.
* IXIT/ICS evidence mapping.
* Test cases and test results.
* Security review notes.

Do not mark a task done if the evidence is missing.

## Definition of Done

A change is done only when:

* It builds successfully.
* Lint/static checks pass.
* Unit tests pass.
* Integration tests pass for affected protocol flows.
* Security-sensitive paths have tests.
* No new hardcoded secret is introduced.
* No new unauthenticated sensitive endpoint is introduced.
* All new interfaces are documented.
* All user-data flows are documented.
* QCVN 11:2026/BCA compliance evidence is updated.
* The diff is reviewed for privacy, authentication, encryption, update safety, and recovery impact.

## Mandatory Stop Conditions

Stop and ask for human review if any of the following appears:

* Requirement asks for a default/shared password.
* Requirement asks to bypass authentication.
* Requirement asks to expose debug interface in production.
* Requirement asks to disable certificate verification.
* Requirement asks to store secrets in plaintext.
* Requirement asks to ship unsigned update logic.
* Requirement asks to collect user data without documentation.
* Requirement asks to keep deleted recordings or indexes accessible.
* Requirement asks to enable RTSP/RTMP/ONVIF publicly without security controls.
* Requirement is unclear but affects authentication, encryption, user data, update, recovery, or compliance.
