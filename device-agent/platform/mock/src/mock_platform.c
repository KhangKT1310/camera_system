#include <stdio.h>
#include <string.h>
#include <time.h>
#include "hal/hal.h"

static bool g_hal_initialized = false;
static bool g_video_stream_active = false;
static uint64_t g_frame_sequence = 0;

hal_status_t hal_init(const hal_init_params_t *params) {
    (void)params;
    g_hal_initialized = true;
    return HAL_OK;
}

void hal_shutdown(void) {
    g_hal_initialized = false;
    g_video_stream_active = false;
}

hal_version_t hal_get_api_version(void) {
    hal_version_t ver = {1, 0, 0};
    return ver;
}

hal_status_t hal_get_platform_capabilities(hal_platform_capabilities_t *caps) {
    if (!caps) return HAL_ERR_INVALID_ARGUMENT;
    memset(caps, 0, sizeof(*caps));

    caps->capability_version.major = 1;
    caps->capability_version.minor = 0;
    caps->capability_version.patch = 0;

    snprintf(caps->platform_name, sizeof(caps->platform_name), "MockPlatform");
    snprintf(caps->platform_version, sizeof(caps->platform_version), "1.0.0");
    snprintf(caps->board_model, sizeof(caps->board_model), "MockBoard-x86_64");
    snprintf(caps->firmware_version, sizeof(caps->firmware_version), "1.0.0-mock");

    /* Security (Product Profile alignment) */
    caps->security.secure_storage = HAL_CAPABILITY_SUPPORTED;
    caps->security.hardware_root_of_trust = HAL_CAPABILITY_RESTRICTED;
    caps->security.csrng = HAL_CAPABILITY_REQUIRED;
    caps->security.device_identity_type = HAL_CAPABILITY_REQUIRED;
    caps->security.verified_boot_state = HAL_CAPABILITY_SUPPORTED;

    /* Media (Product Profile alignment) */
    caps->media.video_capture = HAL_CAPABILITY_REQUIRED;
    caps->media.audio_capture = HAL_CAPABILITY_SUPPORTED;
    caps->media.audio_output = HAL_CAPABILITY_SUPPORTED;
    caps->media.hardware_encoder = HAL_CAPABILITY_REQUIRED;
    caps->media.isp = HAL_CAPABILITY_SUPPORTED;
    caps->media.osd = HAL_CAPABILITY_SUPPORTED;
    caps->media.monotonic_clock = HAL_CAPABILITY_REQUIRED;
    caps->media.max_video_inputs = 1;
    caps->media.max_video_streams = 2;
    caps->media.max_encoders = 2;
    caps->media.max_audio_streams = 1;

    caps->media.timestamp_model = HAL_TIMESTAMP_MODEL_MONOTONIC;

    /* Video Input Capabilities */
    caps->media.video_input_count = 1;
    caps->media.video_inputs[0].input_id = 0;
    snprintf(caps->media.video_inputs[0].name, sizeof(caps->media.video_inputs[0].name), "MockSensor0");
    caps->media.video_inputs[0].max_width = 1920;
    caps->media.video_inputs[0].max_height = 1080;
    caps->media.video_inputs[0].fps_range.min_val = 1;
    caps->media.video_inputs[0].fps_range.max_val = 30;
    caps->media.video_inputs[0].pixel_format_count = 1;
    caps->media.video_inputs[0].supported_formats[0] = HAL_PIXEL_FORMAT_NV12;

    /* Audio Input Capabilities */
    caps->media.audio_input_count = 1;
    caps->media.audio_inputs[0].input_id = 0;
    snprintf(caps->media.audio_inputs[0].name, sizeof(caps->media.audio_inputs[0].name), "MockMic0");
    caps->media.audio_inputs[0].max_sample_rate = 48000;
    caps->media.audio_inputs[0].max_channels = 2;

    /* Codec Capabilities */
    caps->media.codec_count = 2;
    caps->media.codecs[0].codec = HAL_VIDEO_CODEC_H264;
    caps->media.codecs[0].profile = HAL_CODEC_PROFILE_HIGH;
    caps->media.codecs[0].bitrate_range_kbps.min_val = 256;
    caps->media.codecs[0].bitrate_range_kbps.max_val = 8192;
    caps->media.codecs[0].supports_keyframe_request = true;

    caps->media.codecs[1].codec = HAL_VIDEO_CODEC_H265;
    caps->media.codecs[1].profile = HAL_CODEC_PROFILE_MAIN;
    caps->media.codecs[1].bitrate_range_kbps.min_val = 128;
    caps->media.codecs[1].bitrate_range_kbps.max_val = 4096;
    caps->media.codecs[1].supports_keyframe_request = true;

    /* Storage */
    caps->storage.local_volumes = HAL_CAPABILITY_SUPPORTED;
    caps->storage.capacity_reporting = HAL_CAPABILITY_SUPPORTED;
    caps->storage.atomic_rename = HAL_CAPABILITY_SUPPORTED;
    caps->storage.fsync_support = HAL_CAPABILITY_SUPPORTED;
    caps->storage.encryption_at_rest = HAL_CAPABILITY_RESTRICTED;

    /* Network (Product Profile alignment) */
    caps->network.ethernet = HAL_CAPABILITY_SUPPORTED;
    caps->network.wifi = HAL_CAPABILITY_UNSUPPORTED;
    caps->network.ipv4_support = HAL_CAPABILITY_SUPPORTED;
    caps->network.ipv6_support = HAL_CAPABILITY_SUPPORTED;
    caps->network.tls_trust_store = HAL_CAPABILITY_REQUIRED;

    caps->network.net_interface_count = 1;
    snprintf(caps->network.network_interfaces[0].interface_name, sizeof(caps->network.network_interfaces[0].interface_name), "eth0");
    caps->network.network_interfaces[0].type = HAL_NET_IF_ETHERNET;
    caps->network.network_interfaces[0].mtu_hint = 1500;

    snprintf(caps->network.tls_trust_store_source, sizeof(caps->network.tls_trust_store_source), "/etc/ssl/certs");
    caps->network.default_mtu_hint = 1500;

    /* Update */
    caps->update.ab_partition_support = HAL_CAPABILITY_SUPPORTED;
    caps->update.rollback_support = HAL_CAPABILITY_SUPPORTED;
    caps->update.anti_rollback_support = HAL_CAPABILITY_SUPPORTED;
    caps->update.signed_manifest_support = HAL_CAPABILITY_SUPPORTED;

    /* Control */
    caps->control.watchdog = HAL_CAPABILITY_SUPPORTED;
    caps->control.reboot_control = HAL_CAPABILITY_SUPPORTED;
    caps->control.factory_reset = HAL_CAPABILITY_SUPPORTED;
    caps->control.ptz = HAL_CAPABILITY_UNSUPPORTED;
    caps->control.crash_log = HAL_CAPABILITY_SUPPORTED;
    caps->control.debug_interface_state = HAL_CAPABILITY_RESTRICTED;
    caps->control.metrics = HAL_CAPABILITY_SUPPORTED;

    return HAL_OK;
}

hal_status_t hal_query_feature(hal_feature_id_t feature, hal_feature_info_t *info) {
    if (!info) return HAL_ERR_INVALID_ARGUMENT;
    info->feature = feature;
    info->state = HAL_CAPABILITY_SUPPORTED;
    snprintf(info->description, sizeof(info->description), "Mock implementation feature %d", (int)feature);
    return HAL_OK;
}

hal_status_t hal_get_platform_info(hal_platform_info_t *info) {
    if (!info) return HAL_ERR_INVALID_ARGUMENT;
    snprintf(info->manufacturer, sizeof(info->manufacturer), "MockCorp");
    snprintf(info->product_name, sizeof(info->product_name), "VirtualCamera");
    snprintf(info->product_model, sizeof(info->product_model), "V-100");
    snprintf(info->soc_name, sizeof(info->soc_name), "MockSoC-X");
    snprintf(info->soc_revision, sizeof(info->soc_revision), "v1.0");
    return HAL_OK;
}

hal_status_t hal_get_device_identity(hal_device_identity_t *identity) {
    if (!identity) return HAL_ERR_INVALID_ARGUMENT;
    snprintf(identity->serial_number, sizeof(identity->serial_number), "MOCK-SN-123456");
    snprintf(identity->device_uuid, sizeof(identity->device_uuid), "00000000-0000-0000-0000-000000000001");
    snprintf(identity->mac_address, sizeof(identity->mac_address), "02:00:00:00:00:01");
    return HAL_OK;
}

hal_status_t hal_get_boot_integrity_state(hal_boot_integrity_t *state) {
    if (!state) return HAL_ERR_INVALID_ARGUMENT;
    state->secure_boot_enabled = true;
    state->boot_verified = true;
    snprintf(state->bootloader_version, sizeof(state->bootloader_version), "U-Boot Mock 2026.01");
    return HAL_OK;
}

hal_status_t hal_get_provisioning_state(hal_provisioning_state_t *state) {
    if (!state) return HAL_ERR_INVALID_ARGUMENT;
    state->factory_provisioned = true;
    state->keys_locked = true;
    snprintf(state->provisioning_date, sizeof(state->provisioning_date), "2026-01-01T00:00:00Z");
    return HAL_OK;
}

hal_status_t hal_get_hardware_revision(hal_hw_revision_t *revision) {
    if (!revision) return HAL_ERR_INVALID_ARGUMENT;
    snprintf(revision->hw_revision, sizeof(revision->hw_revision), "rev-A");
    snprintf(revision->board_revision, sizeof(revision->board_revision), "board-v1");
    return HAL_OK;
}

hal_status_t hal_get_firmware_version(hal_firmware_version_t *version) {
    if (!version) return HAL_ERR_INVALID_ARGUMENT;
    snprintf(version->firmware_version, sizeof(version->firmware_version), "1.0.0-mock");
    snprintf(version->build_date, sizeof(version->build_date), "2026-07-24");
    return HAL_OK;
}

hal_status_t hal_list_video_inputs(hal_video_input_info_t *inputs, size_t *count) {
    if (!count) return HAL_ERR_INVALID_ARGUMENT;
    if (!inputs) {
        *count = 1;
        return HAL_OK;
    }
    inputs[0].input_id = 0;
    snprintf(inputs[0].name, sizeof(inputs[0].name), "Mock ColorBars Sensor");
    inputs[0].type = HAL_INPUT_TYPE_VIRTUAL;
    snprintf(inputs[0].sensor_name, sizeof(inputs[0].sensor_name), "VirtualSensor");
    inputs[0].connected = true;
    inputs[0].max_width = 1920;
    inputs[0].max_height = 1080;
    inputs[0].max_fps = 30;
    inputs[0].supports_hdr = false;
    inputs[0].supports_wdr = false;
    *count = 1;
    return HAL_OK;
}

hal_status_t hal_list_video_modes(hal_video_input_id_t input, hal_video_mode_t *modes, size_t *count) {
    (void)input;
    if (!count) return HAL_ERR_INVALID_ARGUMENT;
    if (!modes) {
        *count = 1;
        return HAL_OK;
    }
    modes[0].width = 1920;
    modes[0].height = 1080;
    modes[0].frame_rate.numerator = 30;
    modes[0].frame_rate.denominator = 1;
    modes[0].format = HAL_PIXEL_FORMAT_NV12;
    *count = 1;
    return HAL_OK;
}

hal_status_t hal_open_video_stream(hal_video_input_id_t input, const hal_video_mode_t *mode, hal_video_stream_t *stream) {
    (void)input;
    (void)mode;
    if (!stream) return HAL_ERR_INVALID_ARGUMENT;
    *stream = 1; /* Mock handle */
    return HAL_OK;
}

hal_status_t hal_start_video_stream(hal_video_stream_t stream) {
    if (stream != 1) return HAL_ERR_NOT_FOUND;
    g_video_stream_active = true;
    return HAL_OK;
}

hal_status_t hal_read_video_frame(hal_video_stream_t stream, uint32_t timeout_ms, hal_video_frame_t *frame) {
    (void)timeout_ms;
    if (stream != 1 || !g_video_stream_active) return HAL_ERR_INVALID_STATE;
    if (!frame) return HAL_ERR_INVALID_ARGUMENT;

    memset(frame, 0, sizeof(*frame));
    frame->sequence = ++g_frame_sequence;
    frame->width = 1920;
    frame->height = 1080;
    frame->format = HAL_PIXEL_FORMAT_NV12;
    frame->capture_timestamp = hal_now_monotonic();
    return HAL_OK;
}

hal_status_t hal_stop_video_stream(hal_video_stream_t stream) {
    if (stream != 1) return HAL_ERR_NOT_FOUND;
    g_video_stream_active = false;
    return HAL_OK;
}

hal_status_t hal_close_video_stream(hal_video_stream_t stream) {
    (void)stream;
    g_video_stream_active = false;
    return HAL_OK;
}

hal_timestamp_t hal_now_monotonic(void) {
    hal_timestamp_t ts;
    struct timespec spec;
    clock_gettime(CLOCK_MONOTONIC, &spec);
    ts.value_ns = (uint64_t)spec.tv_sec * 1000000000ULL + (uint64_t)spec.tv_nsec;
    ts.source = HAL_CLOCK_MONOTONIC;
    ts.synchronized = true;
    return ts;
}
