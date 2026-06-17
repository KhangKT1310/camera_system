# OTA Client

## Purpose

This module will coordinate signed update retrieval, validation, staged install behavior, and failure recovery on the device.

## What Belongs Here

* Update manifest and artifact validation contracts.
* Install-state transitions, rollback hooks, and recovery policies.
* Coordination points with cloud OTA services and platform installers.

## What Must Not Be Placed Here

* Unsigned update shortcuts.
* Production bypass flags that skip integrity or authenticity checks.
* Vendor flashing logic outside platform installers.

## Security and Compliance Notes

Updates must be signed and verified before installation, transported over TLS, and designed to fail safely. Recovery evidence and anti-downgrade expectations should be documented as the implementation matures.

## Cross-Platform Notes

Keep policy, manifest handling, and verification portable while delegating partition layout, reboot coordination, and platform-specific flashing steps to adapters.
