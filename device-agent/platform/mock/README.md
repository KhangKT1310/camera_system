# Mock Platform

## Purpose

This adapter will provide a non-production mock implementation of platform contracts for local testing, documentation, and interface development.

## What Belongs Here

* Test doubles for capture, storage, crypto, and update hooks.
* Deterministic fixtures that help validate core contracts.
* Explicitly non-production behavior used in tests.

## What Must Not Be Placed Here

* Shipping production code.
* Fake security logic presented as compliant behavior.
* Hidden shortcuts that bypass validation in normal builds.

## Security and Compliance Notes

Mock behavior must be clearly marked as non-production and should never claim compliance. Tests should use this layer to validate policy flows, not to weaken or skip security requirements.

## Cross-Platform Notes

Keep mocks aligned with the same interfaces expected from real platforms so contract design can be tested before hardware integration is complete.
