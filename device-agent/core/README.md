# Device Agent Core

## Purpose

This directory contains the portable core of the device agent: media orchestration, protocol-neutral policy, recording control, security rules, and OTA coordination.

## What Belongs Here

* Interfaces and contracts shared across platforms.
* Business logic for live, playback, recording, and update orchestration.
* Validation, policy, and control-plane logic that must behave consistently on every target.

## What Must Not Be Placed Here

* Vendor SDK calls or chip-tuned media primitives.
* Board-specific file paths, ioctl details, or proprietary APIs.
* Optional protocol implementations that bypass shared policy.

## Security and Compliance Notes

Core logic must centralize policy checks for authentication, encryption requirements, interface state, deletion behavior, and update validation so that platform code cannot weaken them accidentally.

## Cross-Platform Notes

Every dependency on media, storage, crypto, network, and reboot behavior should be abstracted behind contracts consumed by the platform layer.
