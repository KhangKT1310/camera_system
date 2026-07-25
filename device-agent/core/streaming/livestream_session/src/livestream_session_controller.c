#include "livestream_session_controller.h"
#include "rtp_packetizer.h"
#include <stdlib.h>
#include <string.h>

struct livestream_session_controller {
    char session_id[128];
    media_pipeline_t *media_pipeline;
    webrtc_transport_t *transport;
    rtp_packetizer_t *packetizer;
    bool is_running;
};

static void on_rtp_packet_generated(const uint8_t *packet_data, size_t packet_size, void *user_data) {
    livestream_session_controller_t *ctrl = (livestream_session_controller_t *)user_data;
    if (ctrl && ctrl->transport) {
        webrtc_transport_send_rtp(ctrl->transport, packet_data, packet_size);
    }
}

static void on_media_frame_received(const media_frame_t *frame, void *user_data) {
    livestream_session_controller_t *ctrl = (livestream_session_controller_t *)user_data;
    if (ctrl && ctrl->packetizer && ctrl->is_running) {
        rtp_packetizer_push_frame(ctrl->packetizer, frame);
    }
}

int livestream_session_controller_create(const livestream_session_config_t *config,
                                          const webrtc_transport_callbacks_t *transport_callbacks,
                                          void *user_data,
                                          livestream_session_controller_t **out_controller) {
    if (!config || !config->session_id || !config->media_pipeline || !out_controller) {
        return -1;
    }

    livestream_session_controller_t *ctrl = (livestream_session_controller_t *)calloc(1, sizeof(livestream_session_controller_t));
    if (!ctrl) {
        return -1;
    }

    strncpy(ctrl->session_id, config->session_id, sizeof(ctrl->session_id) - 1);
    ctrl->media_pipeline = config->media_pipeline;
    ctrl->is_running = false;

    rtp_packetizer_config_t rtp_cfg = {.ssrc = 0x12345678, .payload_type = 96, .mtu = 1200};
    if (rtp_packetizer_create(&rtp_cfg, on_rtp_packet_generated, ctrl, &ctrl->packetizer) != 0) {
        free(ctrl);
        return -1;
    }

    if (webrtc_transport_create(&config->transport_config, transport_callbacks, user_data, &ctrl->transport) != 0) {
        rtp_packetizer_destroy(ctrl->packetizer);
        free(ctrl);
        return -1;
    }

    *out_controller = ctrl;
    return 0;
}

void livestream_session_controller_destroy(livestream_session_controller_t *ctrl) {
    if (!ctrl) {
        return;
    }

    livestream_session_controller_stop(ctrl);

    if (ctrl->transport) {
        webrtc_transport_destroy(ctrl->transport);
        ctrl->transport = NULL;
    }

    if (ctrl->packetizer) {
        rtp_packetizer_destroy(ctrl->packetizer);
        ctrl->packetizer = NULL;
    }

    free(ctrl);
}

int livestream_session_controller_start(livestream_session_controller_t *ctrl) {
    if (!ctrl || !ctrl->transport || !ctrl->media_pipeline) {
        return -1;
    }

    ctrl->is_running = true;

    /* Subscribe controller to receive frames from media_pipeline */
    media_pipeline_subscribe(ctrl->media_pipeline, on_media_frame_received, ctrl);

    /* Initiate WebRTC offer generation */
    return webrtc_transport_create_offer(ctrl->transport);
}

int livestream_session_controller_apply_answer(livestream_session_controller_t *ctrl, const char *sdp) {
    if (!ctrl || !ctrl->transport) {
        return -1;
    }
    return webrtc_transport_set_remote_description(ctrl->transport, "answer", sdp);
}

int livestream_session_controller_add_ice_candidate(livestream_session_controller_t *ctrl, const char *candidate, const char *mid) {
    if (!ctrl || !ctrl->transport) {
        return -1;
    }
    return webrtc_transport_add_remote_candidate(ctrl->transport, candidate, mid);
}

int livestream_session_controller_stop(livestream_session_controller_t *ctrl) {
    if (!ctrl) {
        return -1;
    }

    if (ctrl->is_running) {
        ctrl->is_running = false;
        if (ctrl->media_pipeline) {
            media_pipeline_unsubscribe(ctrl->media_pipeline, on_media_frame_received, ctrl);
        }
    }

    return 0;
}

webrtc_state_t livestream_session_controller_get_state(livestream_session_controller_t *ctrl) {
    if (!ctrl || !ctrl->transport) {
        return WEBRTC_STATE_CLOSED;
    }
    return webrtc_transport_get_state(ctrl->transport);
}
