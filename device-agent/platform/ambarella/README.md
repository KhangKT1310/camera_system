# Ambarella Platform

## Purpose

This adapter will host Ambarella-specific implementations for capture, media acceleration, storage access, and device security capabilities.

## What Belongs Here

* Ambarella SDK bindings and wrappers.
* Board capability notes and integration constraints.
* Compliance evidence pointers for hardware-backed security features.

## What Must Not Be Placed Here

* Portable business logic.
* Protocol compatibility code.
* Temporary production bypasses for development convenience.

## Security and Compliance Notes

Capture how Ambarella platforms satisfy or limit secure credential storage, signed-update verification, and debug-interface disablement so the compliance posture is explicit.

## Cross-Platform Notes

Implementations should honor the same HAL contracts as other targets, even when hardware acceleration features differ.
