#include "livestream_session_controller.h"
#include "media_frame.h"
#include "media_pipeline.h"
#include "signaling_client.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool g_desc_received = false;

static void on_session_local_description(void *user, const char *type, const char *sdp) {
    (void)user;
    assert(type != NULL);
    assert(sdp != NULL);
    g_desc_received = true;
}

int main(void) {
    printf("Running short E2E livestream integration test...\n");

    /* 1. Setup media pipeline */
    media_pipeline_config_t pipe_cfg = {.max_queue_depth = 10, .drop_oldest_on_overflow = true};
    media_pipeline_t *pipeline = NULL;
    int ret = media_pipeline_create(&pipe_cfg, &pipeline);
    assert(ret == 0);

    /* 2. Setup session controller */
    webrtc_transport_callbacks_t callbacks = {
        .on_local_description = on_session_local_description,
        .on_local_candidate = NULL,
        .on_state_changed = NULL
    };

    livestream_session_config_t session_cfg = {
        .session_id = "test-session-001",
        .media_pipeline = pipeline,
        .transport_config = {0}
    };

    livestream_session_controller_t *controller = NULL;
    ret = livestream_session_controller_create(&session_cfg, &callbacks, NULL, &controller);
    assert(ret == 0);

    /* 3. Start session */
    ret = livestream_session_controller_start(controller);
    assert(ret == 0);
    assert(g_desc_received);

    /* 4. Apply SDP Answer */
    const char *mock_answer_sdp =
        "v=0\r\n"
        "o=- 87654321 2 IN IP4 127.0.0.1\r\n"
        "s=Viewer Answer\r\n"
        "m=video 9 UDP/TLS/RTP/SAVPF 96\r\n"
        "a=recvonly\r\n";

    ret = livestream_session_controller_apply_answer(controller, mock_answer_sdp);
    assert(ret == 0);
    assert(livestream_session_controller_get_state(controller) == WEBRTC_STATE_CONNECTED);

    /* 5. Push mock H.264 video frame through pipeline */
    uint8_t h264_data[] = {0x00, 0x00, 0x00, 0x01, 0x67, 0x42, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x01, 0x65, 0x88, 0x84};
    media_buffer_t *buf = media_buffer_create(sizeof(h264_data));
    memcpy(buf->data, h264_data, sizeof(h264_data));
    buf->size = sizeof(h264_data);

    media_frame_t frame = {
        .buffer = buf,
        .data = buf->data,
        .size = buf->size,
        .timestamp = {.pts_us = 100000, .capture_time_us = 100000, .sequence = 1},
        .codec = MEDIA_CODEC_H264,
        .frame_type = MEDIA_FRAME_TYPE_IDR,
        .flags = 0
    };

    ret = media_pipeline_push_frame(pipeline, &frame);
    assert(ret == 0);

    media_buffer_release(buf);
    livestream_session_controller_destroy(controller);
    media_pipeline_destroy(pipeline);

    printf("Short E2E livestream integration test passed!\n");
    return 0;
}
