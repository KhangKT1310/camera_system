# Platform Layer

## Purpose

The platform layer isolates OS, board, SoC, and vendor SDK integration behind stable contracts consumed by the device-agent core.

## What Belongs Here

* Hardware and OS-specific implementations of capture, codec, storage, crypto, secure storage, reboot, and provisioning interfaces.
* Capability notes for each supported target.
* Platform-specific compliance constraints and gaps.

## What Must Not Be Placed Here

* Core business logic.
* Cloud service concerns.
* Compatibility adapters that can live independently of hardware integration.

## Security and Compliance Notes

This is the only place where vendor SDK calls are allowed. Each platform adapter must document available secure-storage, crypto, debug-control, and update primitives so the project can assess QCVN 11:2026/BCA coverage per target.

## Cross-Platform Notes

Targets may vary widely, but they must all satisfy the same core contracts so the security model does not change when new hardware is added.
