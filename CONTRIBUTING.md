# Contributing to Camera System Framework

Thank you for your interest in contributing! This project provides an open-source framework and reference HAL contracts for WebRTC surveillance camera systems under the MIT License.

## Vendor SDK and Licensing Policy

> [!CAUTION]
> **STRICT POLICY REGARDING VENDOR SDKs**:
> Do NOT submit Pull Requests containing proprietary SoC vendor SDK binaries, headers copied from vendor SDKs under NDA/proprietary licenses, or derived bindings.
> Vendor platforms must be integrated out-of-tree or using public HAL vtables.

## Development Workflow

1. Fork and clone the repository.
2. Ensure you have CMake 3.20+, GCC 11+ / Clang 12+, and standard build utilities installed.
3. Follow Conventional Commits format (`feat:`, `fix:`, `docs:`, `compliance:`, `refactor:`, `test:`, `build:`).
4. Run code formatting (`clang-format -i ...`) and ensure tests pass locally before opening a Pull Request.

## C/C++ Coding Guidelines

- Standard: C11 for HAL public headers (`include/hal/*.h`), C++17 for core modules if C++ is used.
- Headers must be standard C-compatible (`extern "C"` wrappers for C++ inclusion).
- No unhandled status codes — always return `hal_status_t`.
- Strict warning settings: `-Wall -Wextra -Werror` enabled on CI.
