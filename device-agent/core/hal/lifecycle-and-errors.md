# HAL Lifecycle and Error Semantics

## Lifecycle Phases

1. Bootstrap
   Core loads one platform implementation and reads its capability snapshot.
2. Initialize
   Core requests initialization of only the contract families required by enabled features.
3. Activate
   Media, storage, security, and update services transition to active state and begin event delivery.
4. Degrade or Recover
   Platform surfaces explicit health changes; core decides whether to retry, pause, or disable features.
5. Shutdown
   Core requests ordered teardown so keys, buffers, sockets, and file handles are released safely.

## Initialization Rules

* Initialization must be idempotent at the contract-family level.
* Platform code must validate configuration before touching vendor SDK state.
* Partial initialization must return structured failure with rollback of any sensitive side effects.

## Error Model

Platform implementations must normalize failures into these categories:

| Error Category | Meaning | Core Reaction |
| --- | --- | --- |
| `invalid_argument` | Core sent malformed or unsupported input. | Treat as programming or validation defect; do not retry blindly. |
| `unsupported` | Platform cannot satisfy the requested capability. | Disable dependent feature and record capability gap. |
| `permission_denied` | Access blocked by policy, ownership, or platform state. | Fail closed and emit audit evidence if security-relevant. |
| `temporarily_unavailable` | Resource may recover without reboot. | Retry under bounded backoff if feature policy allows. |
| `corruption_detected` | Integrity issue in storage, config, or update artifact. | Halt affected flow, quarantine state, require review or recovery path. |
| `security_precondition_failed` | Required secure primitive missing or not in compliant state. | Block feature activation; do not degrade silently. |
| `io_failure` | Hardware or filesystem operation failed. | Follow module-specific recovery strategy. |
| `internal_error` | Unexpected platform failure. | Surface health fault and prefer safe shutdown of affected feature. |

## Health Events

The platform layer must emit structured health changes for:

* sensor offline or unstable
* encoder failure
* storage full or degraded
* secure storage unavailable
* trust-store or certificate validation failure
* update staging or verification failure
* watchdog or reboot reason change

## Security Rules

* Sensitive buffers must be zeroized on teardown when feasible.
* Debug diagnostics must not leak credentials, keys, or plaintext sensitive parameters.
* Recovery paths must not enable insecure fallback transports or unsigned updates.
