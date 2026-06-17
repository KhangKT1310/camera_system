# Audit Service

## Purpose

This service will collect and expose audit records for security-relevant actions across device, cloud, playback, and update workflows.

## What Belongs Here

* Audit event schemas and storage policy.
* Review and export interfaces for authorized operators.
* Vulnerability and security-event evidence references where appropriate.

## What Must Not Be Placed Here

* Raw secrets or full sensitive payload dumps.
* Unbounded telemetry collection.
* Business logic unrelated to audit and evidence.

## Security and Compliance Notes

Audit records should support incident review, evidence preparation, and change traceability without storing unnecessary sensitive content. Retention and access controls must be explicit.

## Cross-Platform Notes

Audit schemas should be portable enough to represent events uniformly across different device platforms and service deployments.
