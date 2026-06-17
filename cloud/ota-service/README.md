# OTA Service

## Purpose

This service will coordinate update manifests, artifact access policy, rollout state, and evidence for device software and firmware updates.

## What Belongs Here

* Signed manifest distribution.
* Rollout, targeting, and support-period policy.
* Device update status intake and recovery coordination.

## What Must Not Be Placed Here

* Unsigned artifact distribution shortcuts.
* Device-side flashing logic.
* Hidden debug endpoints that bypass release policy.

## Security and Compliance Notes

OTA is a security-critical service and must enforce TLS transport, signed artifact workflows, rollback-safe coordination, and auditable rollout decisions.

## Cross-Platform Notes

The service should target devices through portable capability descriptors rather than relying on vendor-specific release assumptions.
