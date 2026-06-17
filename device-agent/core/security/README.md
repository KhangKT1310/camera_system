# Security

## Purpose

This module will define the shared security contracts and enforcement logic used by the device agent.

## What Belongs Here

* Identity, credential, secure-storage, and crypto abstractions.
* Policy checks for interface state, transport security, and secret handling.
* Security-focused validation and audit hooks.

## What Must Not Be Placed Here

* Plaintext secrets, default credentials, or baked-in certificates.
* UI-specific auth workflows.
* Vendor-specific secure-element APIs outside the platform layer.

## Security and Compliance Notes

QCVN 11:2026/BCA requirements around unique credentials, authentication changes, encryption, and protected parameter storage should be anchored here and consumed consistently across modules.

## Cross-Platform Notes

Define stable interfaces that allow each platform to provide the strongest available secure storage, key generation, and entropy mechanisms without changing core policy.
