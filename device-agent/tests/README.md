# Device Agent Tests

## Purpose

This directory will hold device-agent focused tests for core contracts, platform adapters, security controls, and compatibility boundaries.

## What Belongs Here

* Unit, integration, and contract tests.
* Security-focused regression coverage for authentication, validation, and interface state.
* Non-production fixtures and mocks used by tests.

## What Must Not Be Placed Here

* Production secrets or signed artifacts.
* Unreviewed manual test notes with no reproducible steps.
* Debug helpers that alter production behavior.

## Security and Compliance Notes

Security-sensitive paths need explicit test coverage, especially around interface defaults, update verification, secure transport requirements, and deletion behavior. Test evidence should feed the compliance evidence structure when relevant.

## Cross-Platform Notes

Prefer contract tests that run across multiple platform adapters so deviations are detected before platform-specific code reaches production.
