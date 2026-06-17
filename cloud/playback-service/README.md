# Playback Service

## Purpose

This service will authorize playback requests, resolve recording availability, and create playback sessions for viewers.

## What Belongs Here

* Playback search and session APIs.
* Authorization and retention checks.
* Coordination with recording index and media gateway services.

## What Must Not Be Placed Here

* Raw device capture logic.
* Long-term user session management unrelated to playback.
* Client-visible storage internals.

## Security and Compliance Notes

Playback is a sensitive data-access path and must enforce authentication, authorization, deletion awareness, and auditability before any session is created.

## Cross-Platform Notes

Service behavior should depend on portable recording metadata formats rather than hardware-specific storage assumptions.
