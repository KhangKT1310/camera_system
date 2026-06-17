# Protocol Router

## Purpose

This module will route control intents and media-session requests to the correct primary or compatibility path without letting optional protocols bypass shared rules.

## What Belongs Here

* Dispatch rules between WebRTC and optional adapters.
* Shared validation entry points.
* Interface enablement and policy checks.

## What Must Not Be Placed Here

* Protocol-specific parser implementations better isolated in adapters.
* Vendor SDK networking code.
* Hidden debug endpoints or ad hoc migration shortcuts.

## Security and Compliance Notes

This is a policy choke point for disabling unused interfaces by default, enforcing input validation, and ensuring RTSP, RTMP, or ONVIF cannot become alternative security baselines.

## Cross-Platform Notes

Routing logic should remain platform-neutral and use injected adapter capabilities rather than compile-time vendor assumptions.
