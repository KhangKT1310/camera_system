#ifndef CAMERA_HAL_VIDEO_ENCODER_H
#define CAMERA_HAL_VIDEO_ENCODER_H

#include "hal_buffer.h"
#include "hal_camera.h"
#include "hal_error.h"
#include "hal_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HAL_RATE_CONTROL_CBR = 0,
    HAL_RATE_CONTROL_VBR,
    HAL_RATE_CONTROL_CVBR,
    HAL_RATE_CONTROL_FIXQP,
    HAL_RATE_CONTROL_AVBR
} hal_rate_control_t;

typedef struct {
    hal_video_codec_t codec;
    uint32_t width;
    uint32_t height;
    hal_rational_t frame_rate;
    uint32_t bitrate;
    uint32_t max_bitrate;
    hal_rate_control_t rate_control;
    uint32_t gop_size;
    hal_codec_profile_t profile;
    bool low_latency;
} hal_encoder_config_t;

typedef struct {
    hal_buffer_t buffer;
    hal_video_codec_t codec;
    bool is_keyframe;
    uint64_t sequence;
    hal_timestamp_t timestamp;
    size_t data_size;
    uint32_t duration_us;
} hal_encoded_packet_t;

typedef struct {
    uint64_t frames_input;
    uint64_t frames_encoded;
    uint64_t frames_dropped;
    uint64_t bytes_output;
    uint32_t instant_bitrate;
    uint32_t average_qp;
} hal_encoder_stats_t;

hal_status_t hal_open_encoder(const hal_encoder_config_t *config, hal_encoder_t *encoder);
hal_status_t hal_start_encoder(hal_encoder_t encoder);
hal_status_t hal_submit_frame(hal_encoder_t encoder, const hal_video_frame_t *frame);
hal_status_t hal_read_encoded_packet(hal_encoder_t encoder, uint32_t timeout_ms, hal_encoded_packet_t *packet);
hal_status_t hal_request_keyframe(hal_encoder_t encoder);

hal_status_t hal_encoder_set_bitrate(hal_encoder_t encoder, uint32_t bitrate);
hal_status_t hal_encoder_set_framerate(hal_encoder_t encoder, hal_rational_t framerate);
hal_status_t hal_encoder_set_gop(hal_encoder_t encoder, uint32_t gop);
hal_status_t hal_get_encoder_stats(hal_encoder_t encoder, hal_encoder_stats_t *stats);

hal_status_t hal_stop_encoder(hal_encoder_t encoder);
hal_status_t hal_close_encoder(hal_encoder_t encoder);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_HAL_VIDEO_ENCODER_H */
