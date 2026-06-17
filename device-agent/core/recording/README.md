# Recording

## Purpose

This module will coordinate recording segment lifecycle, retention policy hooks, metadata boundaries, and secure handoff to indexing or storage layers.

## What Belongs Here

* Segment lifecycle contracts.
* Retention, deletion, and metadata rules.
* Interfaces for encrypted-at-rest or integrity-protected storage backends.

## What Must Not Be Placed Here

* Raw cloud object-store credentials.
* Playback authorization logic that belongs in higher services.
* Vendor-specific filesystem or block-device code.

## Security and Compliance Notes

Recordings and related metadata are sensitive user data. Deletion support, access control, integrity, and storage locality requirements must be considered from the start and reflected in compliance evidence.

## Cross-Platform Notes

Storage and filesystem operations should use abstract interfaces so retention and recovery behavior remain consistent across Linux distributions and SoC platforms.
