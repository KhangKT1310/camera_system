# ONVIF Adapter

## Purpose

This adapter will provide optional ONVIF compatibility for discovery, device management, PTZ, and event integration.

## What Belongs Here

* ONVIF profile and capability mapping.
* Discovery and management translation layers.
* Policy-aware enablement controls for optional deployments.

## What Must Not Be Placed Here

* Primary media transport ownership.
* Hidden service endpoints outside the interface inventory.
* Vendor-specific camera logic that belongs under `platform/`.

## Security and Compliance Notes

ONVIF is disabled by default and should be treated as a compatibility surface with strict authentication, encryption where supported, and explicit documentation of exposed operations.

## Cross-Platform Notes

Keep ONVIF translation logic portable and express any PTZ or device-management hardware details through shared platform contracts.
