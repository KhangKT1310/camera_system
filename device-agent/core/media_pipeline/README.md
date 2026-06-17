# Media Pipeline

## Purpose

This module will define the portable media pipeline contract for capture, encode handoff, buffering, and stream preparation before publishing or recording.

## What Belongs Here

* Stream graph abstractions.
* Buffer ownership rules and timing contracts.
* Policy-neutral interfaces for video and optional audio flow.

## What Must Not Be Placed Here

* Direct V4L2, vendor MPP, or codec SDK calls.
* RTSP, RTMP, or ONVIF protocol handling.
* UI, cloud, or tenant-specific business rules.

## Security and Compliance Notes

The pipeline must preserve stream isolation, support secure teardown, and avoid leaking raw frames or metadata to unauthorized interfaces. Any sensing capability exposed here must be documented for compliance review.

## Cross-Platform Notes

Keep the contract generic enough to map onto Linux V4L2 and vendor media stacks without changing upstream core behavior.
