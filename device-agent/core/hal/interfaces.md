# Camera HAL Public Interfaces

This document defines the normative public contract that implementations under:

```text
device-agent/platform/*
```

must provide to:

```text
device-agent/core
```

The HAL must isolate the portable core from:

* Linux-specific APIs
* SoC vendor SDKs
* Camera sensor drivers
* ISP SDKs
* V4L2
* ALSA
* GPIO APIs
* hardware codec APIs
* TPM / Secure Element APIs
* flash / MTD / UBI implementation details
* Wi-Fi vendor APIs
* bootloader-specific OTA mechanisms

The core must depend only on the interfaces defined here.

---

# 0. Common HAL Foundation

## Purpose

Define lifecycle, ABI/API versioning, capability discovery, error handling, handles, events, and common types used by every HAL family.

## Required operations

```c
hal_status_t hal_init(const hal_init_params_t *params);

void hal_shutdown(void);

hal_version_t hal_get_api_version(void);

hal_status_t hal_get_platform_capabilities(
    hal_platform_capabilities_t *caps);

hal_status_t hal_query_feature(
    hal_feature_id_t feature,
    hal_feature_info_t *info);

const char *hal_status_string(
    hal_status_t status);
```

## Recommended capability model

```c
typedef struct {
    bool video_capture;
    bool audio_capture;
    bool audio_output;

    bool hardware_encoder;
    bool hardware_decoder;

    bool isp;
    bool autofocus;
    bool hdr;
    bool wdr;

    bool osd;

    bool ptz;

    bool ir_cut;
    bool ir_led;
    bool white_led;

    bool secure_storage;
    bool hardware_crypto;
    bool secure_boot;

    bool ota_ab_partition;
    bool ota_rollback;

    bool wifi;
    bool ethernet;

    bool gpio;
    bool pir;
    bool temperature_sensor;

    bool npu;

    uint32_t max_video_inputs;
    uint32_t max_video_streams;
    uint32_t max_encoders;
    uint32_t max_audio_streams;
} hal_platform_capabilities_t;
```

---

# 0.1 Common Error Model

All HAL APIs should return a normalized error code.

```c
typedef enum {
    HAL_OK = 0,

    HAL_ERR_UNKNOWN,
    HAL_ERR_INVALID_ARGUMENT,
    HAL_ERR_NOT_SUPPORTED,
    HAL_ERR_NOT_FOUND,
    HAL_ERR_ALREADY_EXISTS,

    HAL_ERR_BUSY,
    HAL_ERR_TIMEOUT,
    HAL_ERR_AGAIN,

    HAL_ERR_NO_MEMORY,
    HAL_ERR_NO_SPACE,
    HAL_ERR_IO,

    HAL_ERR_PERMISSION,
    HAL_ERR_AUTHENTICATION,
    HAL_ERR_INTEGRITY,

    HAL_ERR_NOT_READY,
    HAL_ERR_INVALID_STATE,

    HAL_ERR_DEVICE_FAILURE,
    HAL_ERR_SENSOR_FAILURE,
    HAL_ERR_ENCODER_FAILURE,
    HAL_ERR_STORAGE_FAILURE,
    HAL_ERR_NETWORK_FAILURE,

    HAL_ERR_CLOCK_UNSYNCED,

    HAL_ERR_REBOOT_REQUIRED
} hal_status_t;
```

Vendor-specific error numbers must not leak into core.

The implementation may internally record vendor error codes for diagnostics.

---

# 0.2 Opaque Handles

Hardware resources must use opaque handles.

Example:

```c
typedef uint64_t hal_video_stream_t;
typedef uint64_t hal_audio_stream_t;
typedef uint64_t hal_encoder_t;
typedef uint64_t hal_buffer_t;
typedef uint64_t hal_volume_t;
typedef uint64_t hal_transport_t;
typedef uint64_t hal_secret_t;
typedef uint64_t hal_npu_context_t;
```

Core must not depend on:

```text
file descriptor
V4L2 fd
MI_VENC_CHN
HI_MPI_VENC_CHN
IMPEncoderCHN
vendor pointer
physical address
DMA address
```

These stay inside the platform implementation.

---

# 0.3 Timestamp Model

```c
typedef enum {
    HAL_CLOCK_MONOTONIC,
    HAL_CLOCK_MONOTONIC_RAW,
    HAL_CLOCK_UTC,
    HAL_CLOCK_SENSOR
} hal_clock_source_t;

typedef struct {
    uint64_t value_ns;
    hal_clock_source_t source;
    bool synchronized;
} hal_timestamp_t;
```

Every media frame should identify its clock source explicitly.

---

# 0.4 Event Subscription

Events should not require polling every HAL subsystem.

## Required operations

```c
hal_status_t hal_subscribe_events(
    uint64_t event_mask,
    hal_event_callback_t callback,
    void *user_context,
    hal_subscription_t *subscription);

hal_status_t hal_unsubscribe_events(
    hal_subscription_t subscription);
```

Recommended events:

```text
VIDEO_FRAME_DROPPED
VIDEO_SIGNAL_LOST
VIDEO_SIGNAL_RESTORED
SENSOR_FAULT

ENCODER_FAULT
ENCODER_OVERFLOW

AUDIO_SIGNAL_LOST

NETWORK_LINK_UP
NETWORK_LINK_DOWN
NETWORK_ADDRESS_CHANGED

CLOCK_SYNCED
CLOCK_UNSYNCED
CLOCK_JUMP_DETECTED

STORAGE_LOW
STORAGE_FULL
STORAGE_IO_ERROR
STORAGE_REMOVED

THERMAL_WARNING
THERMAL_CRITICAL

WATCHDOG_WARNING

UPDATE_READY
UPDATE_FAILED

TAMPER_DETECTED

PIR_TRIGGERED

PTZ_FAULT
```

---

# 1. Platform Identity

## Purpose

Expose immutable or semi-immutable device/platform information.

## Required operations

```c
get_platform_info()
get_device_identity()
get_boot_integrity_state()
get_provisioning_state()
```

## Recommended APIs

```c
hal_status_t hal_get_platform_info(
    hal_platform_info_t *info);

hal_status_t hal_get_device_identity(
    hal_device_identity_t *identity);

hal_status_t hal_get_boot_integrity_state(
    hal_boot_integrity_t *state);

hal_status_t hal_get_provisioning_state(
    hal_provisioning_state_t *state);

hal_status_t hal_get_hardware_revision(
    hal_hw_revision_t *revision);

hal_status_t hal_get_firmware_version(
    hal_firmware_version_t *version);
```

## Recommended information

```text
manufacturer
product_name
product_model
hardware_revision
board_revision
soc_name
soc_revision
serial_number
device_uuid
manufacturing_date
firmware_version
bootloader_version
secure_boot_enabled
factory_provisioned
```

The core must never obtain these values directly from vendor SDKs.

---

# 2. Media Buffer Management

This is one of the most important missing interfaces in many camera HAL designs.

## Purpose

Normalize ownership of DMA/video/audio/encoder buffers.

## Required operations

```c
hal_status_t hal_buffer_retain(
    hal_buffer_t buffer);

hal_status_t hal_buffer_release(
    hal_buffer_t buffer);

hal_status_t hal_buffer_get_info(
    hal_buffer_t buffer,
    hal_buffer_info_t *info);
```

## Optional CPU access

```c
hal_status_t hal_buffer_map(
    hal_buffer_t buffer,
    hal_map_mode_t mode,
    void **address);

hal_status_t hal_buffer_unmap(
    hal_buffer_t buffer);
```

## Buffer metadata

```c
typedef struct {
    size_t size;
    uint32_t plane_count;

    hal_memory_type_t memory_type;

    bool cpu_accessible;
    bool physically_contiguous;

    hal_buffer_plane_t planes[HAL_MAX_PLANES];
} hal_buffer_info_t;
```

Potential memory types:

```text
SYSTEM_MEMORY
DMA_MEMORY
VIDEO_MEMORY
SECURE_MEMORY
VENDOR_PRIVATE
```

Core must never assume a buffer is represented by a Linux pointer.

---

# 3. Camera / Video Input Discovery

## Purpose

Discover available physical and logical camera inputs.

## Required operations

```c
hal_status_t hal_list_video_inputs(
    hal_video_input_info_t *inputs,
    size_t *count);

hal_status_t hal_get_video_input_capabilities(
    hal_video_input_id_t input,
    hal_video_input_caps_t *caps);

hal_status_t hal_list_video_modes(
    hal_video_input_id_t input,
    hal_video_mode_t *modes,
    size_t *count);
```

## Video input information

Should include:

```text
input_id
name
type
sensor_name
sensor_id
connected
orientation
supported_modes
max_width
max_height
max_fps
HDR support
WDR support
pixel formats
```

Input types could include:

```text
MIPI_CSI
USB
BT656
BT1120
HDMI
VIRTUAL
```

---

# 4. Camera Capture

## Required operations

```c
hal_status_t hal_open_video_stream(
    hal_video_input_id_t input,
    const hal_video_mode_t *mode,
    hal_video_stream_t *stream);

hal_status_t hal_start_video_stream(
    hal_video_stream_t stream);

hal_status_t hal_read_video_frame(
    hal_video_stream_t stream,
    uint32_t timeout_ms,
    hal_video_frame_t *frame);

hal_status_t hal_stop_video_stream(
    hal_video_stream_t stream);

hal_status_t hal_close_video_stream(
    hal_video_stream_t stream);
```

Explicit `start()` / `stop()` is recommended instead of embedding hardware activation inside `open()`.

## Video frame metadata

```c
typedef struct {
    hal_buffer_t buffer;

    uint64_t sequence;

    uint32_t width;
    uint32_t height;

    hal_pixel_format_t format;
    hal_color_space_t color_space;

    hal_timestamp_t capture_timestamp;

    uint32_t exposure_us;
    float analog_gain;
    float digital_gain;

    bool corrupted;

    uint32_t dropped_since_previous;
} hal_video_frame_t;
```

Recommended pixel formats:

```text
NV12
NV21
YUV420P
YUYV
UYVY
RGB888
BGR888
RAW8
RAW10
RAW12
RAW14
```

RAW access should normally remain within HAL unless the core explicitly requires it.

---

# 5. ISP and Image Control

This should be a separate interface from raw capture.

## Purpose

Normalize camera ISP functionality across vendors.

## Required basic operations

```c
hal_status_t hal_get_image_controls(
    hal_video_input_id_t input,
    hal_image_controls_t *controls);

hal_status_t hal_set_image_controls(
    hal_video_input_id_t input,
    const hal_image_controls_t *controls);
```

## Recommended basic controls

```text
brightness
contrast
saturation
sharpness
hue
gamma
rotation
mirror
flip
```

---

# 5.1 Exposure Control

```c
hal_status_t hal_get_exposure_mode(...);

hal_status_t hal_set_exposure_mode(...);

hal_status_t hal_set_manual_exposure(
    input,
    exposure_us,
    analog_gain,
    digital_gain);

hal_status_t hal_set_exposure_compensation(...);

hal_status_t hal_set_anti_flicker(...);
```

Modes:

```text
AUTO
MANUAL
SHUTTER_PRIORITY
GAIN_PRIORITY
```

Anti-flicker:

```text
OFF
50HZ
60HZ
AUTO
```

---

# 5.2 White Balance

```c
hal_status_t hal_get_white_balance(...);

hal_status_t hal_set_white_balance_mode(...);

hal_status_t hal_set_manual_white_balance(
    input,
    red_gain,
    blue_gain);
```

Modes:

```text
AUTO
MANUAL
INDOOR
OUTDOOR
DAYLIGHT
CLOUDY
FLUORESCENT
INCANDESCENT
```

---

# 5.3 HDR / WDR

```c
hal_status_t hal_get_hdr_capabilities(...);

hal_status_t hal_set_hdr_mode(...);

hal_status_t hal_set_wdr_level(...);
```

Example modes:

```text
OFF
WDR
HDR_2_FRAME
HDR_3_FRAME
AUTO
```

---

# 5.4 Noise Reduction

```c
hal_status_t hal_set_2d_noise_reduction(...);

hal_status_t hal_set_3d_noise_reduction(...);
```

---

# 5.5 Focus

For motorized lenses:

```c
hal_status_t hal_get_focus_state(...);

hal_status_t hal_set_focus_mode(...);

hal_status_t hal_start_autofocus(...);

hal_status_t hal_set_focus_position(...);

hal_status_t hal_get_focus_position(...);
```

---

# 5.6 Day / Night Mode

Important for security cameras.

```c
hal_status_t hal_get_daynight_state(...);

hal_status_t hal_set_daynight_mode(...);

hal_status_t hal_set_daynight_threshold(...);
```

Modes:

```text
DAY
NIGHT
AUTO
SCHEDULE
```

---

# 5.7 IR-Cut Filter

```c
hal_status_t hal_set_ir_cut_state(
    hal_ir_cut_state_t state);

hal_status_t hal_get_ir_cut_state(
    hal_ir_cut_state_t *state);
```

States:

```text
DAY_POSITION
NIGHT_POSITION
TRANSITIONING
FAULT
```

---

# 5.8 Supplemental Lighting

```c
hal_status_t hal_list_lights(...);

hal_status_t hal_set_light_state(...);

hal_status_t hal_set_light_intensity(...);

hal_status_t hal_get_light_state(...);
```

Light types:

```text
INFRARED
WHITE
LASER
STATUS
```

---

# 6. Video Encoder

## Required operations

```c
hal_status_t hal_list_encode_profiles(...);

hal_status_t hal_get_encoder_capabilities(...);

hal_status_t hal_open_encoder(
    const hal_encoder_config_t *config,
    hal_encoder_t *encoder);

hal_status_t hal_start_encoder(
    hal_encoder_t encoder);

hal_status_t hal_submit_frame(
    hal_encoder_t encoder,
    const hal_video_frame_t *frame);

hal_status_t hal_read_encoded_packet(
    hal_encoder_t encoder,
    uint32_t timeout_ms,
    hal_encoded_packet_t *packet);

hal_status_t hal_request_keyframe(
    hal_encoder_t encoder);

hal_status_t hal_stop_encoder(
    hal_encoder_t encoder);

hal_status_t hal_close_encoder(
    hal_encoder_t encoder);
```

---

# 6.1 Encoder Configuration

```c
typedef struct {
    hal_video_codec_t codec;

    uint32_t width;
    uint32_t height;

    hal_rational_t frame_rate;

    uint32_t bitrate;

    hal_rate_control_t rate_control;

    uint32_t gop_size;

    uint32_t max_bitrate;

    hal_codec_profile_t profile;
    hal_codec_level_t level;

    bool low_latency;

    uint32_t max_reference_frames;
} hal_encoder_config_t;
```

Recommended codecs:

```text
H264
H265
MJPEG
JPEG
AV1 optional
```

Rate control:

```text
CBR
VBR
CVBR
FIXQP
AVBR
```

---

# 6.2 Dynamic Encoder Control

```c
hal_status_t hal_encoder_set_bitrate(...);

hal_status_t hal_encoder_set_framerate(...);

hal_status_t hal_encoder_set_gop(...);

hal_status_t hal_encoder_set_rate_control(...);
```

Changes that cannot safely happen dynamically must return:

```text
HAL_ERR_RESTART_REQUIRED
```

rather than silently restarting the encoder.

---

# 6.3 ROI Encoding

Optional but highly useful.

```c
hal_status_t hal_encoder_set_roi(
    hal_encoder_t encoder,
    const hal_roi_region_t *regions,
    size_t count);
```

Typical region:

```text
x
y
width
height
priority
qp_delta
```

---

# 6.4 Encoder Statistics

```c
hal_status_t hal_get_encoder_stats(
    hal_encoder_t encoder,
    hal_encoder_stats_t *stats);
```

Recommended statistics:

```text
frames_input
frames_encoded
frames_dropped
bytes_output
average_bitrate
instant_bitrate
average_qp
queue_depth
overflow_count
```

---

# 7. Audio Capture

## Required when audio is supported

```c
hal_status_t hal_list_audio_inputs(...);

hal_status_t hal_get_audio_input_capabilities(...);

hal_status_t hal_open_audio_input(...);

hal_status_t hal_start_audio_input(...);

hal_status_t hal_read_audio_frame(...);

hal_status_t hal_stop_audio_input(...);

hal_status_t hal_close_audio_input(...);
```

PCM configuration should expose:

```text
sample_rate
sample_format
channels
frame_samples
```

Typical sample formats:

```text
S16_LE
S24_LE
S32_LE
FLOAT32
```

---

# 7.1 Audio Processing

Optional:

```c
hal_status_t hal_set_microphone_gain(...);

hal_status_t hal_set_agc(...);

hal_status_t hal_set_noise_suppression(...);

hal_status_t hal_set_aec(...);

hal_status_t hal_set_high_pass_filter(...);
```

---

# 7.2 Audio Encoder

```c
hal_status_t hal_open_audio_encoder(...);

hal_status_t hal_submit_audio_frame(...);

hal_status_t hal_read_encoded_audio(...);

hal_status_t hal_close_audio_encoder(...);
```

Possible codecs:

```text
AAC
G711A
G711U
G726
OPUS
PCM
```

---

# 8. Audio Output / Two-Way Audio

Often forgotten in camera HAL design.

```c
hal_status_t hal_list_audio_outputs(...);

hal_status_t hal_open_audio_output(...);

hal_status_t hal_start_audio_output(...);

hal_status_t hal_write_audio_frame(...);

hal_status_t hal_set_output_volume(...);

hal_status_t hal_stop_audio_output(...);

hal_status_t hal_close_audio_output(...);
```

Used for:

```text
two-way audio
speaker
alarm sound
intercom
doorbell camera
```

---

# 9. OSD / Video Overlay

## Purpose

Hardware-assisted overlays.

## APIs

```c
hal_status_t hal_create_overlay(
    const hal_overlay_config_t *config,
    hal_overlay_t *overlay);

hal_status_t hal_update_overlay(
    hal_overlay_t overlay,
    const hal_overlay_content_t *content);

hal_status_t hal_attach_overlay(
    hal_overlay_t overlay,
    hal_video_stream_t stream);

hal_status_t hal_set_overlay_position(...);

hal_status_t hal_set_overlay_visibility(...);

hal_status_t hal_destroy_overlay(...);
```

Overlay types:

```text
TEXT
BITMAP
RECTANGLE
MASK
TIMESTAMP
```

Possible uses:

```text
camera name
date/time
watermark
privacy mask
debug information
AI bounding boxes
```

Privacy masks should preferably be applied before external encoding/streaming.

---

# 10. Time and Clock

## Required operations

```c
hal_timestamp_t hal_now_monotonic(void);

hal_status_t hal_now_utc(
    hal_utc_time_t *time);

hal_status_t hal_get_clock_sync_state(
    hal_clock_sync_state_t *state);

hal_status_t hal_get_clock_source(
    hal_clock_source_info_t *source);
```

Optional:

```c
hal_status_t hal_set_utc_time(...);

hal_status_t hal_configure_ntp(...);

hal_status_t hal_force_time_sync(...);
```

Clock state should identify:

```text
UNINITIALIZED
UNSYNCED
SYNCING
SYNCED
DEGRADED
```

Also expose:

```text
source
last_sync_time
estimated_error
clock_jump_detected
```

Media sequencing should rely primarily on monotonic timestamps.

---

# 11. Secure Storage

## Required

```c
hal_status_t hal_store_secret(
    const char *name,
    const void *data,
    size_t size,
    const hal_secret_policy_t *policy);

hal_status_t hal_load_secret(...);

hal_status_t hal_delete_secret(...);
```

However, where hardware allows it, non-exportable secret handles are preferable:

```c
hal_status_t hal_create_secret(...);

hal_status_t hal_open_secret(
    const char *name,
    hal_secret_t *secret);

hal_status_t hal_close_secret(...);
```

Private keys should ideally never be returned as raw bytes.

---

# 12. Cryptography

## Required

```c
hal_status_t hal_generate_random(
    void *buffer,
    size_t length);

hal_status_t hal_verify_signature(...);

hal_status_t hal_get_trust_store_info(...);
```

Recommended:

```c
hal_status_t hal_hash(...);

hal_status_t hal_hmac(...);

hal_status_t hal_generate_keypair(...);

hal_status_t hal_sign_digest(...);

hal_status_t hal_encrypt(...);

hal_status_t hal_decrypt(...);

hal_status_t hal_generate_csr(...);

hal_status_t hal_import_certificate(...);

hal_status_t hal_get_device_certificate(...);
```

Potential hardware backends may include secure elements, TPMs, TrustZone or SoC OTP/key ladders, but those details must remain invisible to core.

---

# 13. Persistent Storage

## Volume discovery

```c
hal_status_t hal_list_volumes(...);

hal_status_t hal_get_volume_info(...);

hal_status_t hal_open_volume(...);

hal_status_t hal_close_volume(...);
```

Volume purposes:

```text
CONFIG
RECORDING
EVENT_RECORDING
SNAPSHOT
LOG
UPDATE
TEMPORARY
```

---

## Object operations

```c
hal_status_t hal_write_object(...);

hal_status_t hal_read_object(...);

hal_status_t hal_delete_object(...);

hal_status_t hal_rename_object(...);

hal_status_t hal_atomic_replace(...);

hal_status_t hal_stat_object(...);

hal_status_t hal_list_objects(...);
```

---

## Durability

```c
hal_status_t hal_fsync_object(...);

hal_status_t hal_fsync_scope(...);
```

---

## Capacity

```c
hal_status_t hal_stat_capacity(
    hal_volume_id_t volume,
    hal_capacity_info_t *info);
```

Should include:

```text
total_bytes
free_bytes
available_bytes
reserved_bytes
read_only
healthy
```

Optional flash-health information:

```text
estimated_wear
bad_blocks
io_errors
media_lifetime
```

Core should not depend on MTD/UBI/JFFS2/UBIFS/ext4 implementation details.

---

# 14. Network Interface Management

## Discovery

```c
hal_status_t hal_list_network_interfaces(...);

hal_status_t hal_get_interface_state(...);

hal_status_t hal_get_interface_capabilities(...);
```

Interface types:

```text
ETHERNET
WIFI
CELLULAR
USB_ETHERNET
VIRTUAL
```

---

## IP configuration

```c
hal_status_t hal_get_ip_configuration(...);

hal_status_t hal_set_ip_configuration(...);
```

Modes:

```text
DHCP
STATIC
LINK_LOCAL
```

Configuration may expose:

```text
IPv4
IPv6
gateway
DNS servers
MTU
hostname
```

---

# 14.1 Ethernet

```c
hal_status_t hal_get_ethernet_link(...);
```

Information:

```text
link_up
speed
duplex
auto_negotiation
```

---

# 14.2 Wi-Fi

Optional:

```c
hal_status_t hal_wifi_scan(...);

hal_status_t hal_wifi_connect(...);

hal_status_t hal_wifi_disconnect(...);

hal_status_t hal_wifi_get_state(...);

hal_status_t hal_wifi_forget_network(...);
```

Credentials must use protected storage.

---

# 15. Network Transport

## DNS

```c
hal_status_t hal_resolve_dns(
    const char *hostname,
    const hal_dns_policy_t *policy,
    hal_address_list_t *addresses);
```

## Outbound connection

```c
hal_status_t hal_open_client_transport(
    const hal_transport_params_t *params,
    hal_transport_t *transport);

hal_status_t hal_transport_read(...);

hal_status_t hal_transport_write(...);

hal_status_t hal_transport_close(...);
```

Transport types:

```text
TCP
UDP
TLS
DTLS
QUIC optional
```

---

# 15.1 TLS

```c
hal_status_t hal_get_tls_context(...);

hal_status_t hal_create_tls_context(...);

hal_status_t hal_tls_get_peer_info(...);
```

Trust decisions must use configured trust policy.

Core should not manually bypass certificate validation.

---

# 15.2 Listener Transport

Only expose this when the product explicitly needs local services such as RTSP/ONVIF.

```c
hal_status_t hal_open_listener(
    const hal_listener_params_t *params,
    const hal_listener_policy_t *policy,
    hal_listener_t *listener);
```

Listener creation must require explicit service policy.

This prevents arbitrary undocumented network services from appearing in core.

---

# 16. OTA Update

## Query state

```c
hal_status_t hal_get_update_state(...);

hal_status_t hal_get_update_capabilities(...);

hal_status_t hal_get_boot_slot_info(...);
```

---

## Staging

```c
hal_status_t hal_stage_update(
    hal_artifact_ref_t artifact,
    hal_manifest_ref_t manifest);

hal_status_t hal_get_update_progress(...);

hal_status_t hal_validate_staged_update(...);
```

Validation should include platform-required checks such as:

```text
signature
hash
image format
hardware compatibility
version compatibility
anti-rollback policy
available storage
```

---

## Activation

```c
hal_status_t hal_activate_staged_update(...);

hal_status_t hal_mark_boot_successful(...);
```

---

## Recovery

```c
hal_status_t hal_get_recovery_status(...);

hal_status_t hal_rollback_to_previous_slot(...);

hal_status_t hal_reboot_to_recovery(...);
```

Optional:

```c
hal_status_t hal_set_anti_rollback_marker(...);
```

A failed boot must be distinguishable from a normal reboot.

---

# 17. Device Lifecycle Control

## Required

```c
hal_status_t hal_reboot(
    hal_reboot_reason_t reason);

hal_status_t hal_factory_reset(
    hal_factory_reset_mode_t mode);
```

Factory-reset modes may include:

```text
USER_SETTINGS_ONLY

NETWORK_SETTINGS

CREDENTIALS

FULL_USER_DATA

SECURE_FULL_RESET
```

Exact deletion semantics must be documented.

---

# 18. Watchdog

Instead of only:

```c
feed_watchdog()
```

prefer a complete watchdog contract:

```c
hal_status_t hal_watchdog_open(
    uint32_t timeout_ms,
    hal_watchdog_t *watchdog);

hal_status_t hal_watchdog_feed(
    hal_watchdog_t watchdog);

hal_status_t hal_watchdog_set_timeout(...);

hal_status_t hal_watchdog_close(...);
```

Optional:

```c
hal_status_t hal_watchdog_get_remaining_time(...);
```

---

# 19. PTZ / Motor Control

## Discovery

```c
hal_status_t hal_get_ptz_capabilities(...);
```

## Movement

```c
hal_status_t hal_ptz_move(
    const hal_ptz_command_t *command);

hal_status_t hal_ptz_stop(...);

hal_status_t hal_ptz_get_position(...);

hal_status_t hal_ptz_move_absolute(...);

hal_status_t hal_ptz_move_relative(...);
```

---

## Presets

```c
hal_status_t hal_ptz_save_preset(...);

hal_status_t hal_ptz_delete_preset(...);

hal_status_t hal_ptz_goto_preset(...);

hal_status_t hal_ptz_list_presets(...);
```

---

## Calibration

```c
hal_status_t hal_ptz_calibrate(...);

hal_status_t hal_ptz_get_calibration_state(...);
```

Potential state:

```text
IDLE
CALIBRATING
READY
LIMIT_REACHED
BLOCKED
FAULT
```

Core remains responsible for authorization.

---

# 20. GPIO and Physical IO

Useful for professional/security cameras.

## GPIO

```c
hal_status_t hal_list_gpio(...);

hal_status_t hal_gpio_configure(...);

hal_status_t hal_gpio_read(...);

hal_status_t hal_gpio_write(...);
```

---

## Alarm input

```c
hal_status_t hal_list_alarm_inputs(...);

hal_status_t hal_get_alarm_input_state(...);

hal_status_t hal_configure_alarm_input(...);
```

---

## Alarm output

```c
hal_status_t hal_list_alarm_outputs(...);

hal_status_t hal_set_alarm_output(...);
```

---

# 21. Sensors

Generic interface for auxiliary sensors.

```c
hal_status_t hal_list_sensors(...);

hal_status_t hal_get_sensor_info(...);

hal_status_t hal_read_sensor(...);

hal_status_t hal_subscribe_sensor(...);
```

Possible sensor classes:

```text
PIR
AMBIENT_LIGHT
TEMPERATURE
HUMIDITY
ACCELEROMETER
GYROSCOPE
MAGNETOMETER
DOOR_SWITCH
TAMPER_SWITCH
```

---

# 22. Thermal and Power Management

## Thermal

```c
hal_status_t hal_get_temperature_sensors(...);

hal_status_t hal_read_temperature(...);

hal_status_t hal_get_thermal_state(...);
```

Thermal states:

```text
NORMAL
WARM
WARNING
CRITICAL
SHUTDOWN
```

---

## Power

```c
hal_status_t hal_get_power_state(...);

hal_status_t hal_set_power_mode(...);

hal_status_t hal_get_power_source(...);
```

Possible power modes:

```text
NORMAL
LOW_POWER
SUSPEND
DEEP_SLEEP
```

Possible sources:

```text
DC
POE
USB
BATTERY
SOLAR
```

---

# 23. Status Indicators / Buzzer

```c
hal_status_t hal_list_indicators(...);

hal_status_t hal_set_indicator(...);

hal_status_t hal_set_indicator_pattern(...);
```

Examples:

```text
POWER_LED
STATUS_LED
NETWORK_LED
RECORD_LED
```

Optional:

```c
hal_status_t hal_buzzer_play(...);

hal_status_t hal_buzzer_stop(...);
```

---

# 24. AI / NPU Accelerator

Recommended as an optional HAL family rather than allowing core to call the SoC NPU SDK.

## Capabilities

```c
hal_status_t hal_list_ai_accelerators(...);

hal_status_t hal_get_ai_capabilities(...);
```

## Model lifecycle

```c
hal_status_t hal_ai_load_model(
    const hal_ai_model_desc_t *model,
    hal_ai_model_t *handle);

hal_status_t hal_ai_unload_model(...);
```

## Inference

```c
hal_status_t hal_ai_create_context(...);

hal_status_t hal_ai_submit_frame(...);

hal_status_t hal_ai_get_result(...);

hal_status_t hal_ai_destroy_context(...);
```

The HAL should normalize execution and buffer ownership.

Interpretation of model outputs can remain inside higher-level AI services.

---

# 25. Snapshot / JPEG Capture

Even when video encoding exists, a dedicated snapshot API is useful.

```c
hal_status_t hal_capture_snapshot(
    hal_video_stream_t source,
    const hal_snapshot_params_t *params,
    hal_encoded_packet_t *jpeg);
```

Parameters may include:

```text
width
height
quality
timestamp overlay
EXIF metadata
```

---

# 26. Privacy Control

Security cameras increasingly need a hardware-level privacy mode.

```c
hal_status_t hal_get_privacy_state(...);

hal_status_t hal_set_privacy_state(...);
```

Potential implementations:

```text
stop sensor
disable ISP output
mechanical shutter
privacy mask
microphone mute
```

Core defines policy.

HAL guarantees enforcement.

---

# 27. Microphone Privacy

Separate from software audio enable/disable.

```c
hal_status_t hal_get_microphone_privacy_state(...);

hal_status_t hal_set_microphone_privacy_state(...);
```

Hardware mute should be distinguishable from software mute when possible.

---

# 28. Hardware Health

```c
hal_status_t hal_get_hardware_health(
    hal_hardware_health_t *health);
```

Potential components:

```text
sensor
ISP
encoder
memory
flash
SD card
network
temperature
secure element
RTC
PTZ
microphone
speaker
```

Each component should expose:

```text
OK
DEGRADED
FAILED
UNKNOWN
```

---

# 29. Reset / Boot Diagnostics

## Required

```c
hal_status_t hal_get_last_reset_reason(
    hal_reset_reason_t *reason);
```

Potential reasons:

```text
POWER_ON
SOFTWARE_REBOOT
WATCHDOG
BROWNOUT
THERMAL
KERNEL_PANIC
BOOT_FAILURE
OTA_ROLLBACK
FACTORY_RESET
UNKNOWN
```

Optional:

```c
hal_status_t hal_get_boot_counter(...);

hal_status_t hal_get_crash_summary(...);
```

Core should receive sanitized metadata, not arbitrary memory dumps.

---

# 30. Observability

## Health

```c
hal_status_t hal_emit_health_event(
    const hal_health_event_t *event);
```

## Audit

```c
hal_status_t hal_emit_audit_event(
    const hal_audit_event_t *event);
```

## Metrics

```c
hal_status_t hal_record_metric(
    const hal_metric_sample_t *sample);
```

Platform metrics may expose:

```text
CPU utilization
memory utilization
temperature
encoder FPS
encoder bitrate
dropped frames
network TX/RX
storage utilization
storage errors
watchdog state
```

Sensitive information must not be included.

---

# 31. Provisioning and Manufacturing

Recommended separate interface because manufacturing operations have a very different security policy from runtime functions.

## Read-only runtime APIs

```c
hal_status_t hal_get_manufacturing_info(...);

hal_status_t hal_get_provisioning_state(...);
```

Privileged provisioning APIs may include:

```c
hal_status_t hal_provision_device_identity(...);

hal_status_t hal_provision_device_certificate(...);

hal_status_t hal_program_otp(...);

hal_status_t hal_lock_provisioning(...);
```

These APIs should not normally be available after production provisioning is locked.

---

# 32. Configuration Persistence

For small platform-specific values, exposing a normalized transactional KV store may be useful.

```c
hal_status_t hal_config_get(...);

hal_status_t hal_config_set(...);

hal_status_t hal_config_delete(...);

hal_status_t hal_config_begin_transaction(...);

hal_status_t hal_config_commit(...);

hal_status_t hal_config_abort(...);
```

However, product-level configuration schema remains owned by core.

HAL only provides persistence semantics.

---

# 33. Recommended Global API Groups

A practical header layout:

```text
include/hal/

    hal.h

    hal_types.h
    hal_error.h
    hal_event.h

    hal_platform.h

    hal_buffer.h

    hal_camera.h
    hal_isp.h
    hal_light.h

    hal_video_encoder.h

    hal_audio.h

    hal_osd.h

    hal_time.h

    hal_crypto.h
    hal_secure_storage.h

    hal_storage.h

    hal_network.h

    hal_update.h

    hal_device.h
    hal_watchdog.h

    hal_ptz.h

    hal_gpio.h
    hal_sensor.h

    hal_power.h
    hal_thermal.h

    hal_ai.h

    hal_observability.h

    hal_provisioning.h
```

A vendor implementation might look like:

```text
device-agent/

    core/
        camera/
        media/
        recording/
        networking/
        ota/
        security/

    platform/

        linux/
            hal_camera_v4l2.c
            hal_audio_alsa.c
            hal_network_linux.c

        sstar/
            hal_camera_sstar.c
            hal_isp_sstar.c
            hal_encoder_sstar.c
            hal_ai_sstar.c

        novatek/
            hal_camera_nvt.c
            hal_isp_nvt.c
            hal_encoder_nvt.c

        ambarella/
            hal_camera_amba.c
            hal_encoder_amba.c
```

Core must compile without including any of those vendor headers.

---

# 34. Recommended Mandatory Interface Set

For every camera platform:

| HAL family         | Requirement |
| ------------------ | ----------- |
| Common/Foundation  | REQUIRED    |
| Platform Identity  | REQUIRED    |
| Buffer Management  | REQUIRED    |
| Camera Discovery   | REQUIRED    |
| Camera Capture     | REQUIRED    |
| ISP Basic Controls | REQUIRED    |
| Video Encoder      | REQUIRED    |
| Time               | REQUIRED    |
| Secure Storage     | REQUIRED    |
| Crypto             | REQUIRED    |
| Persistent Storage | REQUIRED    |
| Network            | REQUIRED    |
| OTA                | REQUIRED    |
| Device Lifecycle   | REQUIRED    |
| Watchdog           | REQUIRED    |
| Observability      | REQUIRED    |
| Hardware Health    | REQUIRED    |

Feature-dependent:

| HAL family       | Requirement |
| ---------------- | ----------- |
| Audio Capture    | CONDITIONAL |
| Audio Output     | CONDITIONAL |
| OSD              | OPTIONAL    |
| PTZ              | CONDITIONAL |
| IR / White Light | CONDITIONAL |
| GPIO / Alarm     | OPTIONAL    |
| PIR              | OPTIONAL    |
| Thermal          | RECOMMENDED |
| Power Management | RECOMMENDED |
| NPU              | OPTIONAL    |
| Wi-Fi            | CONDITIONAL |
| Manufacturing    | RECOMMENDED |
| Snapshot         | RECOMMENDED |
| Privacy          | RECOMMENDED |

---

# 35. Recommended Camera Pipeline

HAL should support the logical pipeline:

```text
                       ┌──────────────┐
                       │ Camera Sensor│
                       └───────┬──────┘
                               │
                               ▼
                       ┌──────────────┐
                       │     ISP      │
                       │ AE/AWB/AF    │
                       │ WDR/HDR/NR   │
                       └───────┬──────┘
                               │
                               ▼
                     ┌──────────────────┐
                     │ HAL Video Buffer │
                     └────────┬─────────┘
                              │
             ┌────────────────┼─────────────────┐
             │                │                 │
             ▼                ▼                 ▼
       ┌──────────┐      ┌──────────┐      ┌─────────┐
       │ Encoder  │      │   NPU    │      │Snapshot │
       └────┬─────┘      └──────────┘      └─────────┘
            │
            ▼
     ┌──────────────┐
     │ H264 / H265  │
     │ encoded pkt  │
     └──────┬───────┘
            │
      ┌─────┴───────────────┐
      │                     │
      ▼                     ▼
 Livestream              Recording
 RTSP/WebRTC              Storage
```

The portable core manages policy and orchestration.

The platform HAL manages hardware.

---

# 36. Ownership Boundary

## Core owns

```text
user configuration
authentication
authorization
stream policy
recording policy
retention policy
cloud connectivity policy
OTA policy
event policy
analytics interpretation
RTSP/WebRTC/application protocols
ONVIF business logic
```

## HAL owns

```text
sensor driver
ISP
DMA
hardware encoder
hardware audio
NPU execution
GPIO
PTZ motor
flash durability
secure hardware
watchdog
boot slots
platform networking primitives
```

This boundary is extremely important.

---

# 37. APIs That Must NOT Leak Into Core

Do not expose APIs such as:

```c
ioctl(...)
open("/dev/video0")
VIDIOC_*
mmap(...)
MI_VIF_*
MI_VPE_*
MI_ISP_*
MI_VENC_*

HI_MPI_VI_*
HI_MPI_VPSS_*
HI_MPI_VENC_*

IMP_ISP_*
IMP_FrameSource_*
IMP_Encoder_*

NVT_VCAP_*
NVT_VENC_*

snd_pcm_*
gpio_set_value(...)
```

Core should only see:

```text
hal_open_video_stream()
hal_read_video_frame()
hal_open_encoder()
hal_read_encoded_packet()
...
```

---

# 38. API Versioning Rules

Every public structure should support extension.

Recommended pattern:

```c
typedef struct {
    uint32_t struct_size;
    uint32_t version;

    ...
} hal_encoder_config_t;
```

This allows fields to be appended without immediately breaking ABI compatibility.

HAL should expose:

```c
hal_get_api_version()
```

Example:

```text
HAL API 1.0
HAL API 1.1
HAL API 2.0
```

Major version:

```text
breaking API change
```

Minor version:

```text
backward-compatible capability/API extension
```

---

# 39. Thread-Safety Rules

Every function should explicitly belong to one of:

```text
THREAD_SAFE
HANDLE_SERIALIZED
CALLER_SERIALIZED
```

Recommended default:

```text
Global query APIs:
    THREAD_SAFE

Different camera handles:
    THREAD_SAFE

Same stream handle:
    HANDLE_SERIALIZED unless explicitly documented

Event callbacks:
    must not block HAL internal worker threads
```

---

# 40. Blocking Rules

Potentially blocking calls must accept timeouts.

For example:

```c
hal_read_video_frame(
    stream,
    timeout_ms,
    &frame);

hal_read_encoded_packet(
    encoder,
    timeout_ms,
    &packet);
```

Recommended special value:

```c
HAL_TIMEOUT_INFINITE
```

Non-blocking:

```c
timeout_ms = 0
```

Possible result:

```text
HAL_ERR_AGAIN
```

---

# 41. Media Resource Lifecycle

Every hardware object should follow a consistent lifecycle:

```text
DISCOVER
   │
   ▼
OPEN
   │
   ▼
CONFIGURE
   │
   ▼
START
   │
   ▼
RUNNING
   │
   ▼
STOP
   │
   ▼
CLOSE
```

For example:

```c
hal_open_video_stream()
hal_start_video_stream()

while (...) {
    hal_read_video_frame();
    ...
    hal_buffer_release();
}

hal_stop_video_stream();
hal_close_video_stream();
```

Encoder:

```c
hal_open_encoder();
hal_start_encoder();

hal_submit_frame();
hal_read_encoded_packet();

hal_stop_encoder();
hal_close_encoder();
```

---

# 42. Resource Limit Discovery

Core should never guess hardware limits.

Provide capabilities such as:

```text
max_camera_inputs
max_capture_streams

max_encoder_channels

max_h264_channels
max_h265_channels

max_total_encode_pixels_per_second

max_4k_encoders
max_1080p_encoders

max_osd_regions

max_ai_contexts

max_audio_channels

max_concurrent_snapshots
```

This is particularly important because different camera SoCs can expose very different media-pipeline limits.

---

# 43. Suggested Minimal V1 HAL

For an initial production implementation, I would freeze V1 around these interfaces first:

```text
hal_base
hal_platform

hal_buffer

hal_camera
hal_isp

hal_video_encoder

hal_audio

hal_time

hal_crypto
hal_secure_storage

hal_storage

hal_network

hal_update

hal_device
hal_watchdog

hal_observability
```

Then implement V1.1/V1.2 extensions:

```text
hal_osd
hal_ptz
hal_gpio
hal_sensor
hal_light
hal_ai
hal_power
hal_thermal
hal_privacy
```

This keeps the initial HAL manageable without creating architectural shortcuts that later force vendor APIs into `core`.

---

# 44. Most Important Camera HAL Contract

The most important architectural rule is:

```text
                     DEVICE-AGENT CORE
                           │
                           │
                     PUBLIC HAL API
                           │
             ┌─────────────┼─────────────┐
             │             │             │
             ▼             ▼             ▼
         SigmaStar      Novatek      Ambarella
             │             │             │
             ▼             ▼             ▼
        Vendor SDK      Vendor SDK     Vendor SDK
             │             │             │
             └──────────── Hardware ─────┘
```

`device-agent/core` must never know which SoC exists underneath the HAL.

A correct abstraction means code such as:

```c
camera_stream = hal_open_video_stream(...);
encoder = hal_open_encoder(...);
```

works identically whether the product uses SigmaStar, Novatek, Ambarella, Ingenic, Rockchip, or another supported platform.

The HAL implementation owns all translation between the normalized contract and the vendor-specific media pipeline.
