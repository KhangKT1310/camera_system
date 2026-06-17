# Common Platform Utilities

## Purpose

This directory holds platform utilities and shared adapter helpers that are still platform-facing but reusable across multiple targets.

## What Belongs Here

* Shared HAL helper code patterns and documentation.
* Capability probing conventions.
* Common wrappers for secure storage, timing, or diagnostics that remain below the core boundary.

## What Must Not Be Placed Here

* Product business logic.
* Vendor-specific SDK bindings that belong in vendor directories.
* Production-enabled debug backdoors.

## Security and Compliance Notes

Shared helpers must preserve the default-disabled state of debug interfaces and provide consistent enforcement hooks for secure storage, update verification, and interface controls.

## Cross-Platform Notes

Utilities here should reduce duplication across Linux and SoC targets without introducing assumptions that prevent target-specific specialization.
