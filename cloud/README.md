# Cloud Services

## Purpose

This directory defines the cloud-side services used for signaling, authentication, media coordination, playback authorization, recording index management, OTA, and auditing.

## What Belongs Here

* Service boundaries and service-specific documentation.
* API ownership for control-plane and playback flows.
* Storage and deployment notes relevant to compliance and data locality.

## What Must Not Be Placed Here

* Device hardware integrations.
* Browser-only UI logic.
* Monolithic catch-all services with unclear ownership.

## Security and Compliance Notes

Cloud services must use TLS, enforce strong authentication and authorization, document user-data flows, and support applicable data-locality requirements including Vietnam configuration where needed.

## Cross-Platform Notes

Cloud logic should remain independent of device vendor details and interact with devices through portable protocols and documented contracts.
