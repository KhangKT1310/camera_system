#ifndef CAMERA_HAL_TYPES_H
#define CAMERA_HAL_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Version structure */
typedef struct {
    uint16_t major;
    uint16_t minor;
    uint16_t patch;
} hal_version_t;

/* Opaque handles */
typedef uint64_t hal_video_stream_t;
typedef uint64_t hal_audio_stream_t;
typedef uint64_t hal_encoder_t;
typedef uint64_t hal_audio_encoder_t;
typedef uint64_t hal_buffer_t;
typedef uint64_t hal_volume_t;
typedef uint64_t hal_transport_t;
typedef uint64_t hal_secret_t;
typedef uint64_t hal_overlay_t;
typedef uint64_t hal_watchdog_t;
typedef uint64_t hal_subscription_t;

/* Rational number representation (e.g., frame rates) */
typedef struct {
    uint32_t numerator;
    uint32_t denominator;
} hal_rational_t;

/* Clock sources */
typedef enum {
    HAL_CLOCK_MONOTONIC = 0,
    HAL_CLOCK_MONOTONIC_RAW,
    HAL_CLOCK_UTC,
    HAL_CLOCK_SENSOR
} hal_clock_source_t;

/* Timestamp structure */
typedef struct {
    uint64_t value_ns;
    hal_clock_source_t source;
    bool synchronized;
} hal_timestamp_t;

/* Pixel Formats */
typedef enum {
    HAL_PIXEL_FORMAT_UNKNOWN = 0,
    HAL_PIXEL_FORMAT_NV12,
    HAL_PIXEL_FORMAT_NV21,
    HAL_PIXEL_FORMAT_YUV420P,
    HAL_PIXEL_FORMAT_YUYV,
    HAL_PIXEL_FORMAT_UYVY,
    HAL_PIXEL_FORMAT_RGB888,
    HAL_PIXEL_FORMAT_BGR888,
    HAL_PIXEL_FORMAT_RAW8,
    HAL_PIXEL_FORMAT_RAW10,
    HAL_PIXEL_FORMAT_RAW12,
    HAL_PIXEL_FORMAT_RAW14
} hal_pixel_format_t;

/* Video Codecs */
typedef enum {
    HAL_VIDEO_CODEC_UNKNOWN = 0,
    HAL_VIDEO_CODEC_H264,
    HAL_VIDEO_CODEC_H265,
    HAL_VIDEO_CODEC_MJPEG,
    HAL_VIDEO_CODEC_JPEG,
    HAL_VIDEO_CODEC_AV1
} hal_video_codec_t;

/* Codec Profiles */
typedef enum {
    HAL_CODEC_PROFILE_BASELINE = 0,
    HAL_CODEC_PROFILE_MAIN,
    HAL_CODEC_PROFILE_HIGH
} hal_codec_profile_t;

/* Audio Codecs */
typedef enum {
    HAL_AUDIO_CODEC_UNKNOWN = 0,
    HAL_AUDIO_CODEC_PCM,
    HAL_AUDIO_CODEC_G711A,
    HAL_AUDIO_CODEC_G711U,
    HAL_AUDIO_CODEC_G726,
    HAL_AUDIO_CODEC_AAC,
    HAL_AUDIO_CODEC_OPUS
} hal_audio_codec_t;

/* Memory Types */
typedef enum {
    HAL_MEMORY_TYPE_SYSTEM = 0,
    HAL_MEMORY_TYPE_DMA,
    HAL_MEMORY_TYPE_VIDEO,
    HAL_MEMORY_TYPE_SECURE,
    HAL_MEMORY_TYPE_VENDOR_PRIVATE
} hal_memory_type_t;

/* Network Interface Types */
typedef enum {
    HAL_NET_IF_ETHERNET = 0,
    HAL_NET_IF_WIFI,
    HAL_NET_IF_CELLULAR,
    HAL_NET_IF_VIRTUAL
} hal_net_if_type_t;

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_HAL_TYPES_H */
