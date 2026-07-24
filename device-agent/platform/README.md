# Platform Layer

## Purpose

The platform layer isolates OS, board, SoC, and vendor SDK integration behind stable contracts consumed by the device-agent core.

## What Belongs Here

* Hardware and OS-specific implementations of capture, codec, storage, crypto, secure storage, reboot, and provisioning interfaces.
* Capability notes for each supported target.
* Platform-specific compliance constraints and gaps.
* Implementations of the contracts defined in `device-agent/core/hal/`.

## What Must Not Be Placed Here

* Core business logic.
* Cloud service concerns.
* Compatibility adapters that can live independently of hardware integration.

## Security and Compliance Notes

This directory contains open-source platform implementations (`mock`, `linux_v4l2`, `common`) and integration notes for vendor targets. Proprietary vendor SDK bindings must be developed out-of-tree or linked dynamically via HAL C vtables to maintain MIT licensing compliance. Each platform target must document available secure-storage, crypto, debug-control, and update primitives.

## Cross-Platform Notes

Targets may vary widely, but they must all satisfy the same core contracts so the security model does not change when new hardware is added.
