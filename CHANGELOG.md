# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Comprehensive HAL public interface specifications (44 sections covering 10 core contract families).
- C11 HAL public headers definitions (20 headers under `device-agent/core/hal/include/hal/*.h`).
- Mock platform implementation (`device-agent/platform/mock/`) for capability, platform identity, and video stream testing.
- Open-source foundation files under MIT License.
- CMake build infrastructure with cross-compilation toolchain support (ARM32/ARM64) and CMake package export (`camera_system::hal_interface`).
