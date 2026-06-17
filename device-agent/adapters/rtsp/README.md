# RTSP Adapter

## Purpose

This adapter will provide legacy RTSP compatibility for integrations that cannot use the primary WebRTC path.

## What Belongs Here

* RTSP-specific session handling and translation logic.
* Configuration for controlled enablement and exposure.
* Mapping between core media outputs and RTSP transport.

## What Must Not Be Placed Here

* Primary live-view implementation.
* Public-by-default streaming configuration.
* Security policy duplicated outside the shared core layer.

## Security and Compliance Notes

RTSP is disabled by default and must remain a compatibility path only. If secure variants or authenticated modes are used, they must be preferred; insecure fallback modes must never become the default or WAN-exposed baseline.

## Cross-Platform Notes

The adapter should rely on common media and network contracts so the same RTSP logic can operate across supported device platforms.
