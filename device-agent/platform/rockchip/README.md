# Rockchip Platform

## Purpose

This adapter will host Rockchip-specific integrations needed to satisfy the portable device-agent contracts.

## What Belongs Here

* Rockchip media, codec, storage, or security bindings.
* Capability detection and board-variant notes.
* Compliance-relevant documentation for available platform primitives.

## What Must Not Be Placed Here

* Shared portable business logic.
* Compatibility adapters.
* Hardcoded product credentials or keys tied to a single board image.

## Security and Compliance Notes

Document whether Rockchip targets provide hardware-backed key storage, secure boot dependencies, trusted execution support, and how production debug access is disabled.

## Cross-Platform Notes

Expose Rockchip functionality through the same contracts used by other platform directories so the core system remains unchanged.
