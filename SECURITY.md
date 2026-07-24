# Security Policy

## Security Baseline

The Camera System framework adheres to secure-by-default principles:
- WebRTC media streams are encrypted using DTLS-SRTP.
- APIs and signaling connections require TLS.
- Unused interfaces and legacy protocols (RTSP, ONVIF) are disabled by default.
- No default hardcoded credentials or shared keys.

## Reporting a Vulnerability

If you discover a security vulnerability within this framework, please report it privately:

- **Email**: security@example.com (or open a private security advisory on GitHub)
- **Response Window**: We will acknowledge reports within 48 hours and aim to provide a mitigation plan within 14 days.

Do not publicly disclose vulnerabilities until a patch or advisory has been published.
