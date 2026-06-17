# HiSilicon Platform

## Purpose

This adapter will host HiSilicon-specific implementations for media, storage, and security capabilities required by the portable core.

## What Belongs Here

* HiSilicon SDK integrations and platform notes.
* Capability discovery for device and board variants.
* Security primitive documentation relevant to compliance evidence.

## What Must Not Be Placed Here

* Protocol-policy logic.
* Cloud service code.
* Unreviewed factory shortcuts that bypass production security requirements.

## Security and Compliance Notes

Record how secure storage, update verification, and debug controls are implemented or constrained on HiSilicon targets so gaps can be reviewed explicitly against QCVN 11:2026/BCA.

## Cross-Platform Notes

Keep implementations behind shared interfaces and avoid special-casing core logic for a specific SoC family.
