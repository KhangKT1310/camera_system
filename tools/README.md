# Tools

## Purpose

This directory holds support tooling for compliance review, interface analysis, and signed-update workflows.

## What Belongs Here

* Lightweight developer or release tools.
* Compliance automation helpers.
* Documentation describing evidence and release support tasks.

## What Must Not Be Placed Here

* Heavy application frameworks.
* Production runtime services.
* Tools that embed real secrets or bypass security checks.

## Security and Compliance Notes

Support tools should strengthen evidence quality and release safety rather than bypassing them. Outputs that affect compliance or security posture must be reviewable and traceable.

## Cross-Platform Notes

Prefer portable tooling that can run consistently across developer and CI environments without depending on a single device vendor.
