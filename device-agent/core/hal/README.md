# Device HAL Contracts

## Purpose

This directory defines the mandatory HAL boundary between `device-agent/core` and `device-agent/platform/*` so platform integrations can be implemented without leaking vendor SDK details into core logic.

## What Belongs Here

* Interface contracts consumed by portable core modules.
* Capability schemas, lifecycle rules, and error semantics.
* Security constraints that every platform implementation must honor.

## What Must Not Be Placed Here

* Vendor SDK headers, bindings, or ioctl constants.
* WebRTC stack details or compatibility protocol behavior.
* Product-specific feature flags that bypass shared contracts.

## Security and Compliance Notes

The HAL is a control boundary. It must preserve interface minimization, secure storage requirements, signed-update enforcement, and the rule that optional protocols cannot bypass the core policy layer. Platform implementations may expose only approved capabilities and must fail closed when a required security primitive is unavailable.

## Cross-Platform Notes

All platform adapters must implement the same contract families, even if some capabilities are reported as unsupported. Feature detection belongs in capabilities, not in platform-specific branches inside core logic.

## Contract Set

* `contract-overview.md`: module map and dependency rules.
* `capability-model.md`: capability advertisement and compatibility requirements.
* `lifecycle-and-errors.md`: startup, health, teardown, and fail-closed behavior.
* `interfaces.md`: normative interface families and required operations.
