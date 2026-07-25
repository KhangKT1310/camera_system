#include "libdatachannel_adapter.h"
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct ldc_backend {
    webrtc_transport_config_t config;
    ldc_callbacks_t callbacks;
    void *user_data;
    int pc_id;
    int video_track_id;
    atomic_bool closing;
};

int ldc_backend_create(const webrtc_transport_config_t *config,
                       const ldc_callbacks_t *callbacks,
                       void *user,
                       ldc_backend_t **out) {
    if (!out) {
        return -1;
    }

    ldc_backend_t *b = (ldc_backend_t *)calloc(1, sizeof(ldc_backend_t));
    if (!b) {
        return -1;
    }

    if (config) {
        b->config = *config;
    }
    if (callbacks) {
        b->callbacks = *callbacks;
    }

    b->user_data = user;
    b->pc_id = 1; /* Peer connection ID simulation handle */
    b->video_track_id = 100; /* Track ID handle */
    atomic_store(&b->closing, false);

    *out = b;
    return 0;
}

int ldc_backend_create_offer(ldc_backend_t *b) {
    if (!b || atomic_load(&b->closing)) {
        return -1;
    }

    /* SDP offer generation via libdatachannel adapter */
    const char *mock_offer =
        "v=0\r\n"
        "o=- 12345678 2 IN IP4 127.0.0.1\r\n"
        "s=CameraSystem Livestream\r\n"
        "t=0 0\r\n"
        "m=video 9 UDP/TLS/RTP/SAVPF 96\r\n"
        "c=IN IP4 0.0.0.0\r\n"
        "a=rtpmap:96 H264/90000\r\n"
        "a=fmtp:96 profile-level-id=42e01f;packetization-mode=1;level-asymmetry-allowed=1\r\n"
        "a=sendonly\r\n";

    if (b->callbacks.on_local_description) {
        b->callbacks.on_local_description(b->user_data, "offer", mock_offer);
    }

    if (b->callbacks.on_local_candidate) {
        b->callbacks.on_local_candidate(b->user_data, "candidate:1 1 UDP 2122260223 127.0.0.1 50000 typ host", "video");
    }

    return 0;
}

int ldc_backend_set_remote_description(ldc_backend_t *b, const char *type, const char *sdp) {
    if (!b || !type || !sdp || atomic_load(&b->closing)) {
        return -1;
    }

    if (b->callbacks.on_state_changed) {
        b->callbacks.on_state_changed(b->user_data, WEBRTC_STATE_CONNECTED);
    }

    return 0;
}

int ldc_backend_add_candidate(ldc_backend_t *b, const char *candidate, const char *mid) {
    (void)mid;
    if (!b || !candidate || atomic_load(&b->closing)) {
        return -1;
    }
    return 0;
}

int ldc_backend_send_rtp(ldc_backend_t *b, const uint8_t *packet, size_t size) {
    if (!b || !packet || size == 0 || atomic_load(&b->closing)) {
        return -1;
    }
    /*
     * In real libdatachannel build linking libdatachannel library:
     * rtcSendMessage(b->video_track_id, (const char *)packet, (int)size);
     */
    return 0;
}

void ldc_backend_destroy(ldc_backend_t *b) {
    if (!b) {
        return;
    }

    atomic_store(&b->closing, true);

    if (b->callbacks.on_state_changed) {
        b->callbacks.on_state_changed(b->user_data, WEBRTC_STATE_CLOSED);
    }

    free(b);
}
