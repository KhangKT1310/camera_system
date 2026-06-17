# Linux V4L2 Platform

## Purpose

This adapter will map portable capture and media contracts onto a Linux-first implementation built around V4L2 and related kernel interfaces.

## What Belongs Here

* V4L2 capture integration.
* Linux-specific device enumeration and capability detection.
* OS-level implementations for storage, timing, and secure-process interactions where appropriate.

## What Must Not Be Placed Here

* Vendor-only media SDK bindings.
* WebRTC business logic.
* Compatibility protocol behavior.

## Security and Compliance Notes

Document what Linux facilities are used for credential storage, process isolation, and debug-interface control. Avoid assuming desktop-style defaults are acceptable for production surveillance devices.

## Cross-Platform Notes

This adapter should serve as the reference Linux target without constraining vendor-backed platforms that offer different hardware acceleration or secure-storage mechanisms.
