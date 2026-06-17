# Live View

## Purpose

This module will define the user-facing live-view experience built on top of authenticated WebRTC sessions.

## What Belongs Here

* Live session UX requirements.
* Viewer-side signaling and session lifecycle expectations.
* Access-control and observability requirements for live playback.

## What Must Not Be Placed Here

* Playback-only logic.
* Direct device admin controls unrelated to viewing.
* Secrets or certificates stored in browser code.

## Security and Compliance Notes

Live view must only attach to authorized sessions and should avoid leaking device metadata, credentials, or raw network details into the browser.

## Cross-Platform Notes

Design for browser portability and variable network conditions while keeping transport assumptions aligned with the primary WebRTC architecture.
