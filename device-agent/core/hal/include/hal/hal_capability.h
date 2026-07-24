#ifndef CAMERA_HAL_CAPABILITY_H
#define CAMERA_HAL_CAPABILITY_H

#include "hal_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HAL_MAX_CAP_VIDEO_INPUTS 4
#define HAL_MAX_CAP_AUDIO_INPUTS 4
#define HAL_MAX_CAP_CODECS 8
#define HAL_MAX_CAP_NET_IFS 4
#define HAL_MAX_CAP_PIXEL_FORMATS 8

typedef enum {
    HAL_CAPABILITY_REQUIRED = 0,
    HAL_CAPABILITY_SUPPORTED,
    HAL_CAPABILITY_UNSUPPORTED,
    HAL_CAPABILITY_RESTRICTED
} hal_capability_state_t;

typedef struct {
    uint32_t min_val;
    uint32_t max_val;
} hal_range_t;

typedef enum {
    HAL_TIMESTAMP_MODEL_MONOTONIC = 0,
    HAL_TIMESTAMP_MODEL_MONOTONIC_RAW,
    HAL_TIMESTAMP_MODEL_WALL_CLOCK
} hal_timestamp_model_t;

typedef struct {
    uint32_t input_id;
    char name[32];
    uint32_t max_width;
    uint32_t max_height;
    hal_range_t fps_range;
    uint32_t pixel_format_count;
    hal_pixel_format_t supported_formats[HAL_MAX_CAP_PIXEL_FORMATS];
} hal_video_input_cap_t;

typedef struct {
    uint32_t input_id;
    char name[32];
    uint32_t max_sample_rate;
    uint32_t max_channels;
} hal_audio_input_cap_t;

typedef struct {
    hal_video_codec_t codec;
    hal_codec_profile_t profile;
    hal_range_t bitrate_range_kbps;
    bool supports_keyframe_request;
} hal_codec_cap_t;

typedef struct {
    char interface_name[32];
    hal_net_if_type_t type;
    uint32_t mtu_hint;
} hal_net_interface_cap_t;

/* Security Capability Snapshot */
typedef struct {
    hal_capability_state_t secure_storage;
    hal_capability_state_t hardware_root_of_trust;
    hal_capability_state_t csrng;
    hal_capability_state_t device_identity_type;
    hal_capability_state_t verified_boot_state;
} hal_security_capabilities_t;

/* Media Capability Snapshot */
typedef struct {
    hal_capability_state_t video_capture;
    hal_capability_state_t audio_capture;
    hal_capability_state_t audio_output;
    hal_capability_state_t hardware_encoder;
    hal_capability_state_t isp;
    hal_capability_state_t osd;
    hal_capability_state_t monotonic_clock;

    uint32_t max_video_inputs;
    uint32_t max_video_streams;
    uint32_t max_encoders;
    uint32_t max_audio_streams;

    uint32_t video_input_count;
    hal_video_input_cap_t video_inputs[HAL_MAX_CAP_VIDEO_INPUTS];

    uint32_t audio_input_count;
    hal_audio_input_cap_t audio_inputs[HAL_MAX_CAP_AUDIO_INPUTS];

    uint32_t codec_count;
    hal_codec_cap_t codecs[HAL_MAX_CAP_CODECS];

    hal_timestamp_model_t timestamp_model;
} hal_media_capabilities_t;

/* Storage Capability Snapshot */
typedef struct {
    hal_capability_state_t local_volumes;
    hal_capability_state_t capacity_reporting;
    hal_capability_state_t atomic_rename;
    hal_capability_state_t fsync_support;
    hal_capability_state_t encryption_at_rest;
} hal_storage_capabilities_t;

/* Network Capability Snapshot */
typedef struct {
    hal_capability_state_t ethernet;
    hal_capability_state_t wifi;
    hal_capability_state_t ipv4_support;
    hal_capability_state_t ipv6_support;
    hal_capability_state_t tls_trust_store;

    uint32_t net_interface_count;
    hal_net_interface_cap_t network_interfaces[HAL_MAX_CAP_NET_IFS];

    char tls_trust_store_source[64];
    uint32_t default_mtu_hint;
} hal_network_capabilities_t;

/* Update Capability Snapshot */
typedef struct {
    hal_capability_state_t ab_partition_support;
    hal_capability_state_t rollback_support;
    hal_capability_state_t anti_rollback_support;
    hal_capability_state_t signed_manifest_support;
} hal_update_capabilities_t;

/* Control & Diagnostics Capability Snapshot */
typedef struct {
    hal_capability_state_t watchdog;
    hal_capability_state_t reboot_control;
    hal_capability_state_t factory_reset;
    hal_capability_state_t ptz;
    hal_capability_state_t crash_log;
    hal_capability_state_t debug_interface_state;
    hal_capability_state_t metrics;
} hal_control_capabilities_t;

/* Full Platform Capability Snapshot (Versioned) */
typedef struct {
    hal_version_t capability_version;
    char platform_name[64];
    char platform_version[32];
    char board_model[64];
    char firmware_version[64];

    hal_security_capabilities_t security;
    hal_media_capabilities_t media;
    hal_storage_capabilities_t storage;
    hal_network_capabilities_t network;
    hal_update_capabilities_t update;
    hal_control_capabilities_t control;
} hal_platform_capabilities_t;

typedef enum {
    HAL_FEATURE_VIDEO_CAPTURE = 0,
    HAL_FEATURE_AUDIO_CAPTURE,
    HAL_FEATURE_AUDIO_OUTPUT,
    HAL_FEATURE_HW_ENCODER,
    HAL_FEATURE_SECURE_STORAGE,
    HAL_FEATURE_HARDWARE_CRYPTO,
    HAL_FEATURE_PTZ
} hal_feature_id_t;

typedef struct {
    hal_feature_id_t feature;
    hal_capability_state_t state;
    char description[64];
} hal_feature_info_t;

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_HAL_CAPABILITY_H */
