# Playback Web

## Purpose

This module will define the user-facing playback experience for searching, selecting, and viewing authorized recordings.

## What Belongs Here

* Playback UX requirements and state flow.
* Search, seek, and retention-aware client expectations.
* Integration notes for playback APIs and WebRTC playback sessions.

## What Must Not Be Placed Here

* Raw storage identifiers or file-path assumptions.
* Admin-only account management behavior.
* Browser-side logic that bypasses server authorization.

## Security and Compliance Notes

Playback UI must respect authorization, deletion, and privacy constraints, and should avoid exposing sensitive recording metadata beyond what the user is entitled to access.

## Cross-Platform Notes

The client should remain portable across browsers and rely on stable service contracts rather than device-specific recording formats.
