# Pull Request

## Summary

Describe what this PR changes.

## Type of Change

- [ ] Documentation
- [ ] Architecture
- [ ] HAL/interface contract
- [ ] Platform implementation
- [ ] WebRTC livestream
- [ ] Playback
- [ ] RTSP adapter
- [ ] RTMP/RTMPS adapter
- [ ] ONVIF adapter
- [ ] Security
- [ ] Compliance evidence
- [ ] Test only
- [ ] Refactor
- [ ] CI/build

## Compliance Impact

- [ ] No compliance impact
- [ ] Updates QCVN 11:2026/BCA evidence
- [ ] Adds or changes an interface/protocol
- [ ] Affects authentication or authorization
- [ ] Affects encryption or key management
- [ ] Affects secure storage
- [ ] Affects OTA/update behavior
- [ ] Affects debug interfaces
- [ ] Affects user data collection/storage/deletion
- [ ] Affects playback access or recording retention
- [ ] Affects factory reset or recovery behavior

Compliance docs updated:

- [ ] `docs/compliance/qcvn11-control-matrix.md`
- [ ] `docs/compliance/interface-inventory.md`
- [ ] `docs/compliance/threat-model.md`
- [ ] `docs/compliance/ixit-ics-evidence/`
- [ ] Not applicable

## Interface Inventory

Does this PR add, remove, enable, disable, or change any interface?

- [ ] No
- [ ] Yes, and `docs/compliance/interface-inventory.md` was updated

Affected interfaces:

```text
List interfaces here.
Security Checklist

No hardcoded secrets, credentials, keys, tokens, certificates, or default passwords.

No unauthenticated sensitive endpoint added.

No production debug interface enabled.

No certificate validation bypass.

No plaintext sensitive communication.

No fake security implementation.

Input validation considered for all new external inputs.

User data impact reviewed.

Logs do not expose secrets or sensitive user data.

Cross-Platform Checklist

Core code does not call vendor SDK APIs directly.

Vendor-specific logic is isolated under device-agent/platform/<vendor>/.

HAL contract changes are documented under device-agent/core/hal/.

Platform limitations are documented.

Mock or test implementation considered.

Testing Evidence

Commands run:

Paste commands here.

Results:

Paste result summary here.

If tests were not run, explain why:

Explain here.
Risk and Rollback

Risk level:

Low

Medium

High

Rollback plan:

Describe rollback plan.
Human Review Required

Human security/compliance review is required if this PR affects:

Authentication
Authorization
Encryption
Key management
Secure storage
OTA/update
Debug interface
Factory reset
User data
Recording/playback access
RTSP/RTMP/ONVIF exposure
HAL security capability

Also update `AGENTS.md` with this section:

```text
## Pull Request Rules

Every PR must follow these rules:

1. Use the PR template.
2. Keep PRs small and reviewable.
3. Do not mix unrelated changes.
4. Do not bypass the compliance checklist.
5. Any protocol/interface change must update `docs/compliance/interface-inventory.md`.
6. Any security or user-data change must update compliance evidence.
7. Any HAL contract change must update `device-agent/core/hal/CONTRACT.md`.
8. Any platform-specific implementation must document platform limitations.
9. A PR must not claim QCVN compliance unless supporting evidence is present.
10. PRs affecting authentication, encryption, OTA, secure storage, debug interfaces, user data, playback, or protocol exposure require human security/compliance review.