# Web Clients

## Purpose

This directory defines user-facing web surfaces for live view, playback, and administration.

## What Belongs Here

* Feature-level documentation for viewer and admin applications.
* Client boundaries, API ownership notes, and security assumptions.
* UX constraints related to authentication, authorization, and privacy.

## What Must Not Be Placed Here

* Device platform code.
* Cloud-side secret handling.
* Hidden admin or debug routes not reflected in interface documentation.

## Security and Compliance Notes

Web clients must consume only authenticated APIs over TLS, avoid storing sensitive data insecurely, and reflect user-data, deletion, and consent behaviors required by the overall compliance model.

## Cross-Platform Notes

Client behavior should be portable across browsers and operating systems without depending on hardware-specific device details.
