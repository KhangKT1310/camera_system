# Playback Reader

## Purpose

This module will read authorized recording segments and present them to playback sessions without exposing raw storage internals to clients.

## What Belongs Here

* Segment read contracts.
* Seek, range, and timebase normalization logic.
* Interfaces that bridge stored media into playback sessions.

## What Must Not Be Placed Here

* Viewer authentication or tenant policy that belongs in cloud services.
* Storage backend credentials or vendor SDK file readers.
* Direct public API exposure.

## Security and Compliance Notes

Playback access must respect authorization, retention, and deletion state. Reader logic must not leak raw paths, storage keys, or stale data outside approved interfaces.

## Cross-Platform Notes

Use portable file and media-read abstractions so playback behavior is stable even when local storage or codec helpers differ across targets.
