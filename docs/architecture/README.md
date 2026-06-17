# Architecture Docs

## Purpose

This directory documents system boundaries, media flows, protocol decisions, and the separation between portable core logic and platform-specific implementations.

## What Belongs Here

* System-overview documents.
* Live and playback flow descriptions.
* Protocol and interface policy decisions.
* Architecture constraints that guide implementation.

## What Must Not Be Placed Here

* Vendor SDK source drops.
* Production code or framework scaffolding.
* Security claims without evidence in compliance docs.

## Security and Compliance Notes

Architecture decisions here must stay consistent with QCVN 11:2026/BCA, especially around interface minimization, transport security, OTA trust, and isolation of optional protocols.

## Cross-Platform Notes

Document abstractions in a way that allows Linux and vendor-specific targets to share the same core contracts while differing only in platform adapters.
