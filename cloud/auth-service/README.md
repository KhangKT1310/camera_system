# Auth Service

## Purpose

This service will manage identities, sessions, authorization decisions, and credential lifecycle flows for users, admins, and devices.

## What Belongs Here

* Authentication and authorization APIs.
* Device identity and ownership binding workflows.
* Credential rotation and session management policies.

## What Must Not Be Placed Here

* Media transport logic.
* Plaintext secret storage.
* Direct dependencies on device vendor SDKs.

## Security and Compliance Notes

This service is central to unique credentials, changeable authentication values, and secure session handling. Validation, auditability, and secret handling must be designed as first-class requirements.

## Cross-Platform Notes

Device identities should be modeled in a vendor-neutral way so authentication policy does not depend on the specific hardware family.
