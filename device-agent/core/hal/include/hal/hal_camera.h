#ifndef CAMERA_HAL_CAMERA_H
#define CAMERA_HAL_CAMERA_H

#include "hal_buffer.h"
#include "hal_error.h"
#include "hal_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t hal_video_input_id_t;

typedef enum {
    HAL_INPUT_TYPE_MIPI_CSI = 0,
    HAL_INPUT_TYPE_USB,
    HAL_INPUT_TYPE_BT656,
    HAL_INPUT_TYPE_BT1120,
    HAL_INPUT_TYPE_HDMI,
    HAL_INPUT_TYPE_VIRTUAL
} hal_input_type_t;

typedef struct {
    hal_video_input_id_t input_id;
    char name[64];
    hal_input_type_t type;
    char sensor_name[64];
    bool connected;
    uint32_t max_width;
    uint32_t max_height;
    uint32_t max_fps;
    bool supports_hdr;
    bool supports_wdr;
} hal_video_input_info_t;

typedef struct {
    uint32_t width;
    uint32_t height;
    hal_rational_t frame_rate;
    hal_pixel_format_t format;
} hal_video_mode_t;

typedef struct {
    hal_buffer_t buffer;
    uint64_t sequence;
    uint32_t width;
    uint32_t height;
    hal_pixel_format_t format;
    hal_timestamp_t capture_timestamp;
    uint32_t exposure_us;
    float analog_gain;
    float digital_gain;
    bool corrupted;
    uint32_t dropped_since_previous;
} hal_video_frame_t;

hal_status_t hal_list_video_inputs(hal_video_input_info_t *inputs, size_t *count);
hal_status_t hal_list_video_modes(hal_video_input_id_t input, hal_video_mode_t *modes, size_t *count);

hal_status_t hal_open_video_stream(hal_video_input_id_t input, const hal_video_mode_t *mode, hal_video_stream_t *stream);
hal_status_t hal_start_video_stream(hal_video_stream_t stream);
hal_status_t hal_read_video_frame(hal_video_stream_t stream, uint32_t timeout_ms, hal_video_frame_t *frame);
hal_status_t hal_stop_video_stream(hal_video_stream_t stream);
hal_status_t hal_close_video_stream(hal_video_stream_t stream);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_HAL_CAMERA_H */
