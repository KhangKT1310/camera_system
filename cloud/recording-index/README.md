# Recording Index

## Purpose

This service or module will manage searchable metadata for recorded segments, retention state, and deletion tracking.

## What Belongs Here

* Segment index models and lookup APIs.
* Retention and deletion markers.
* Metadata consistency and recovery notes.

## What Must Not Be Placed Here

* Raw media relay logic.
* Public exposure of object-store keys or local file paths.
* Device-specific storage driver code.

## Security and Compliance Notes

Index metadata is sensitive because it reveals recording existence, timing, and ownership. Deletion handling, access control, and storage-integrity expectations should be explicit from the beginning.

## Cross-Platform Notes

Keep index formats and APIs device-agnostic so recordings from different platforms can be queried consistently.
