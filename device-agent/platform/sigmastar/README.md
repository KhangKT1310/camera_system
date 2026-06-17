# SigmaStar Platform

## Purpose

This adapter will contain SigmaStar-specific platform integrations required by the core media and security contracts.

## What Belongs Here

* SigmaStar SDK bindings.
* Hardware capability mapping.
* Platform-specific compliance notes for storage, crypto, OTA, and diagnostics.

## What Must Not Be Placed Here

* Shared recording or playback policy logic.
* UI or cloud components.
* Compatibility adapters that should remain independent of hardware.

## Security and Compliance Notes

Document available production controls for secure storage, trusted boot assumptions, update verification support, and debug disablement on SigmaStar targets.

## Cross-Platform Notes

Any SigmaStar optimizations must remain encapsulated so the portable core and external interfaces do not change.
