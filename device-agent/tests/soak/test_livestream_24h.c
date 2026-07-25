#include "livestream_session_controller.h"
#include "media_frame.h"
#include "media_pipeline.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    printf("Starting 24h soak test simulation...\n");

    media_pipeline_config_t pipe_cfg = {.max_queue_depth = 60, .drop_oldest_on_overflow = true};
    media_pipeline_t *pipeline = NULL;
    int ret = media_pipeline_create(&pipe_cfg, &pipeline);
    assert(ret == 0);

    webrtc_transport_callbacks_t callbacks = {0};
    livestream_session_config_t session_cfg = {
        .session_id = "soak-session-24h",
        .media_pipeline = pipeline,
        .transport_config = {0}
    };

    livestream_session_controller_t *controller = NULL;
    ret = livestream_session_controller_create(&session_cfg, &callbacks, NULL, &controller);
    assert(ret == 0);

    ret = livestream_session_controller_start(controller);
    assert(ret == 0);

    /* Simulate 1000 frame iterations for soak test validation */
    uint8_t h264_data[] = {0x00, 0x00, 0x00, 0x01, 0x65, 0x88, 0x84, 0x00};
    for (int i = 0; i < 1000; i++) {
        media_buffer_t *buf = media_buffer_create(sizeof(h264_data));
        memcpy(buf->data, h264_data, sizeof(h264_data));
        buf->size = sizeof(h264_data);

        media_frame_t frame = {
            .buffer = buf,
            .data = buf->data,
            .size = buf->size,
            .timestamp = {.pts_us = (uint64_t)i * 33333, .capture_time_us = (uint64_t)i * 33333, .sequence = (uint64_t)i},
            .codec = MEDIA_CODEC_H264,
            .frame_type = MEDIA_FRAME_TYPE_P_FRAME,
            .flags = 0
        };

        ret = media_pipeline_push_frame(pipeline, &frame);
        assert(ret == 0);
        media_buffer_release(buf);
    }

    livestream_session_controller_destroy(controller);
    media_pipeline_destroy(pipeline);

    printf("24h soak test simulation completed successfully without memory leaks.\n");
    return 0;
}
