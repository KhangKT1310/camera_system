# Media Gateway

## Purpose

This service will relay or terminate media sessions for live and playback scenarios, acting as the network-facing media layer above signaling.

## What Belongs Here

* WebRTC relay or SFU responsibilities.
* Session resource management and routing rules.
* Observability relevant to media health and policy enforcement.

## What Must Not Be Placed Here

* Device credential storage.
* Business logic for account management.
* Primary ownership of long-term recording metadata.

## Security and Compliance Notes

Media exposure must be tied to authorized sessions only, with encrypted transport and bounded metadata handling. This service must not create bypass paths around auth or playback policy.

## Cross-Platform Notes

Gateway behavior should remain independent of device vendor capabilities and depend only on portable WebRTC session contracts.
