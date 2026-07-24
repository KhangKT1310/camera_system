#ifndef CAMERA_HAL_AUDIO_H
#define CAMERA_HAL_AUDIO_H

#include "hal_buffer.h"
#include "hal_error.h"
#include "hal_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t hal_audio_input_id_t;
typedef uint32_t hal_audio_output_id_t;

typedef enum {
    HAL_AUDIO_FORMAT_S16_LE = 0,
    HAL_AUDIO_FORMAT_S24_LE,
    HAL_AUDIO_FORMAT_S32_LE,
    HAL_AUDIO_FORMAT_FLOAT32
} hal_audio_sample_format_t;

typedef struct {
    uint32_t sample_rate;
    uint32_t channels;
    hal_audio_sample_format_t sample_format;
    uint32_t period_size_samples;
} hal_audio_config_t;

typedef struct {
    hal_buffer_t buffer;
    uint64_t sequence;
    hal_timestamp_t timestamp;
    uint32_t sample_rate;
    uint32_t channels;
    hal_audio_sample_format_t sample_format;
    size_t samples_count;
} hal_audio_frame_t;

typedef struct {
    hal_buffer_t buffer;
    hal_audio_codec_t codec;
    uint64_t sequence;
    hal_timestamp_t timestamp;
    size_t data_size;
} hal_encoded_audio_packet_t;

/* Audio Input / Capture */
hal_status_t hal_list_audio_inputs(hal_audio_input_id_t *inputs, size_t *count);
hal_status_t hal_open_audio_input(hal_audio_input_id_t input, const hal_audio_config_t *config, hal_audio_stream_t *stream);
hal_status_t hal_start_audio_input(hal_audio_stream_t stream);
hal_status_t hal_read_audio_frame(hal_audio_stream_t stream, uint32_t timeout_ms, hal_audio_frame_t *frame);
hal_status_t hal_stop_audio_input(hal_audio_stream_t stream);
hal_status_t hal_close_audio_input(hal_audio_stream_t stream);

/* Audio Controls & Processing */
hal_status_t hal_set_microphone_gain(hal_audio_input_id_t input, float gain_db);
hal_status_t hal_set_noise_suppression(hal_audio_input_id_t input, bool enable);
hal_status_t hal_set_aec(hal_audio_input_id_t input, bool enable);

/* Audio Encoder */
hal_status_t hal_open_audio_encoder(hal_audio_codec_t codec, const hal_audio_config_t *config, hal_audio_encoder_t *encoder);
hal_status_t hal_submit_audio_frame(hal_audio_encoder_t encoder, const hal_audio_frame_t *frame);
hal_status_t hal_read_encoded_audio(hal_audio_encoder_t encoder, uint32_t timeout_ms, hal_encoded_audio_packet_t *packet);
hal_status_t hal_close_audio_encoder(hal_audio_encoder_t encoder);

/* Audio Output / Speaker (Two-Way Audio) */
hal_status_t hal_list_audio_outputs(hal_audio_output_id_t *outputs, size_t *count);
hal_status_t hal_open_audio_output(hal_audio_output_id_t output, const hal_audio_config_t *config, hal_audio_stream_t *stream);
hal_status_t hal_start_audio_output(hal_audio_stream_t stream);
hal_status_t hal_write_audio_frame(hal_audio_stream_t stream, const hal_audio_frame_t *frame);
hal_status_t hal_set_output_volume(hal_audio_output_id_t output, float volume_percent);
hal_status_t hal_stop_audio_output(hal_audio_stream_t stream);
hal_status_t hal_close_audio_output(hal_audio_stream_t stream);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_HAL_AUDIO_H */
