# Platform Implementation Contract

## Purpose

This document states how every directory under `device-agent/platform/*` must implement the shared HAL contracts defined in `device-agent/core/hal/`.

## Mandatory Rules

* Implement only the contract families your platform advertises in its capability snapshot.
* Return normalized capability fields, error categories, and health events exactly as defined by the HAL.
* Keep vendor SDK types, callbacks, and error codes private to the platform implementation.
* Reject unsupported requests explicitly; do not silently downgrade security or media settings.
* Do not expose network listeners, debug endpoints, or factory interfaces unless they are separately documented and approved.

## Required Deliverables Per Platform

Each platform directory should eventually contain:

* capability notes for secure storage, crypto, media, update, and debug controls
* a mapping note from vendor primitives to HAL families
* test notes for initialization, failure handling, and secure-default behavior
* compliance notes for any restricted or unsupported security capability

## Review Gate

No WebRTC publisher work or compatibility adapter work should depend directly on a vendor platform until the target platform has:

* a documented capability snapshot
* a documented mapping to the HAL families
* an explicit statement of unsupported or restricted security primitives
