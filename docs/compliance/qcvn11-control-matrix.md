# QCVN 11 Control Matrix

Status values in this table are placeholders for planning and evidence tracking only.

| Control Area | Requirement Summary | Project Decision | Evidence Location | Status |
| --- | --- | --- | --- | --- |
| Unique credentials / no default password | Each device must avoid shared default authentication values. | Provision per-device identity or unique credentials; reject shared production defaults. | `AGENTS.md`, `docs/compliance/threat-model.md`, `docs/compliance/ixit-ics-evidence/` | Not Started |
| Authentication value change | Authorized users must be able to rotate passwords or equivalent authentication values. | Expose authenticated credential-management flows and document reset behavior. | `AGENTS.md`, `docs/compliance/threat-model.md` | Not Started |
| Secure communication | Sensitive communications must be encrypted in transit. | Use DTLS-SRTP for WebRTC media and TLS for APIs, signaling, OTA, and cloud links. | `AGENTS.md`, `docs/architecture/protocol-policy.md`, `docs/compliance/interface-inventory.md` | Not Started |
| Sensitive security parameter storage | Security parameters must use protected storage. | Abstract secure storage behind platform interfaces and use the strongest available mechanism per target. | `AGENTS.md`, `device-agent/platform/README.md`, `docs/compliance/ixit-ics-evidence/` | Not Started |
| Secure OTA update | Updates must be signed and verified before install. | Centralize manifest verification, signature validation, and recovery handling in OTA flows. | `AGENTS.md`, `device-agent/core/ota_client/README.md`, `tools/firmware-sign/README.md` | Not Started |
| Interface minimization | Unused interfaces must be disabled by default. | Keep compatibility adapters and local debug access opt-in and documented in the inventory. | `AGENTS.md`, `docs/compliance/interface-inventory.md` | Not Started |
| Debug interface disabled | Production builds must not expose debug interfaces by default. | Exclude debug shell and service hooks from production profiles unless explicitly approved. | `AGENTS.md`, `docs/compliance/interface-inventory.md`, `device-agent/tests/README.md` | Not Started |
| User data deletion | User data must be deletable. | Define deletion flows for recordings, indexes, snapshots, and credentials as first-class requirements. | `AGENTS.md`, `docs/compliance/threat-model.md`, `cloud/recording-index/README.md` | Not Started |
| Data storage location configuration | Applicable deployments must support Vietnam data-storage configuration. | Keep storage and service placement configurable and document locality-sensitive data paths. | `AGENTS.md`, `cloud/README.md`, `docs/compliance/threat-model.md` | Not Started |
| Input validation | External inputs must be validated and bounded. | Require validation layers for signaling, playback queries, OTA manifests, adapters, and admin APIs. | `AGENTS.md`, `device-agent/core/protocol_router/README.md`, `cloud/auth-service/README.md` | Not Started |
| Recovery after failure | Security-relevant failures must recover safely. | Design recording, OTA, and identity flows to fail closed and support controlled recovery. | `device-agent/core/ota_client/README.md`, `docs/compliance/threat-model.md` | Not Started |
| Vulnerability disclosure | The product needs a trackable disclosure and remediation process. | Reserve audit and evidence structures for intake, triage, patch tracking, and release evidence. | `cloud/audit-service/README.md`, `docs/compliance/ixit-ics-evidence/README.md` | Not Started |
| IXIT/ICS evidence | Evidence must be organized for implementation conformance reporting. | Maintain structured evidence folders and update them with each security-relevant change. | `docs/compliance/ixit-ics-evidence/README.md` | Not Started |
