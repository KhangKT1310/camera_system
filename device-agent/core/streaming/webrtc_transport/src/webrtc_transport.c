#include "webrtc_transport.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char candidate[WEBRTC_MAX_CANDIDATE_LENGTH];
    char mid[64];
} pending_candidate_t;

struct webrtc_transport {
    webrtc_transport_config_t config;
    webrtc_transport_callbacks_t callbacks;
    void *user_data;
    webrtc_state_t state;
    bool has_remote_description;

    pending_candidate_t pending_candidates[WEBRTC_MAX_PENDING_CANDIDATES];
    size_t pending_candidate_count;

    pthread_mutex_t lock;

    /* Backend context handle for libdatachannel or mock engine */
    void *backend_ctx;
};

static void set_state(webrtc_transport_t *t, webrtc_state_t new_state) {
    if (t->state != new_state) {
        t->state = new_state;
        if (t->callbacks.on_state_changed) {
            t->callbacks.on_state_changed(t->user_data, new_state);
        }
    }
}

int webrtc_transport_create(const webrtc_transport_config_t *config,
                           const webrtc_transport_callbacks_t *callbacks,
                           void *user_data,
                           webrtc_transport_t **out_transport) {
    if (!out_transport) {
        return -1;
    }

    webrtc_transport_t *t = (webrtc_transport_t *)calloc(1, sizeof(webrtc_transport_t));
    if (!t) {
        return -1;
    }

    if (config) {
        t->config = *config;
    }
    if (callbacks) {
        t->callbacks = *callbacks;
    }

    t->user_data = user_data;
    t->state = WEBRTC_STATE_IDLE;
    t->has_remote_description = false;
    t->pending_candidate_count = 0;

    if (pthread_mutex_init(&t->lock, NULL) != 0) {
        free(t);
        return -1;
    }

    *out_transport = t;
    return 0;
}

void webrtc_transport_destroy(webrtc_transport_t *t) {
    if (!t) {
        return;
    }

    pthread_mutex_lock(&t->lock);
    set_state(t, WEBRTC_STATE_CLOSED);
    pthread_mutex_unlock(&t->lock);

    pthread_mutex_destroy(&t->lock);
    free(t);
}

int webrtc_transport_create_offer(webrtc_transport_t *t) {
    if (!t) {
        return -1;
    }

    pthread_mutex_lock(&t->lock);
    set_state(t, WEBRTC_STATE_CREATING);

    /* Generate local offer SDP simulation for mock loopback */
    const char *mock_offer_sdp =
        "v=0\r\n"
        "o=- 12345678 2 IN IP4 127.0.0.1\r\n"
        "s=CameraSystem Livestream\r\n"
        "t=0 0\r\n"
        "m=video 9 UDP/TLS/RTP/SAVPF 96\r\n"
        "c=IN IP4 0.0.0.0\r\n"
        "a=rtpmap:96 H264/90000\r\n"
        "a=sendonly\r\n";

    set_state(t, WEBRTC_STATE_NEGOTIATING);

    if (t->callbacks.on_local_description) {
        t->callbacks.on_local_description(t->user_data, "offer", mock_offer_sdp);
    }

    /* Simulate ICE candidate generation */
    if (t->callbacks.on_local_candidate) {
        t->callbacks.on_local_candidate(t->user_data, "candidate:1 1 UDP 2122260223 127.0.0.1 50000 typ host", "video");
    }

    pthread_mutex_unlock(&t->lock);
    return 0;
}

int webrtc_transport_set_remote_description(webrtc_transport_t *t, const char *type, const char *sdp) {
    if (!t || !type || !sdp || strlen(sdp) == 0) {
        return -1;
    }

    pthread_mutex_lock(&t->lock);
    t->has_remote_description = true;
    set_state(t, WEBRTC_STATE_CONNECTING);

    /* Flush pending ICE candidates that arrived before remote SDP description */
    for (size_t i = 0; i < t->pending_candidate_count; i++) {
        /* Process queued remote candidate */
        (void)t->pending_candidates[i];
    }
    t->pending_candidate_count = 0;

    /* Transition to connected once negotiation completes */
    set_state(t, WEBRTC_STATE_CONNECTED);

    pthread_mutex_unlock(&t->lock);
    return 0;
}

int webrtc_transport_add_remote_candidate(webrtc_transport_t *t, const char *candidate, const char *mid) {
    if (!t || !candidate || strlen(candidate) == 0) {
        return -1;
    }

    pthread_mutex_lock(&t->lock);

    if (!t->has_remote_description) {
        /* Queue candidate if remote description is not yet set */
        if (t->pending_candidate_count < WEBRTC_MAX_PENDING_CANDIDATES) {
            strncpy(t->pending_candidates[t->pending_candidate_count].candidate, candidate, WEBRTC_MAX_CANDIDATE_LENGTH - 1);
            if (mid) {
                strncpy(t->pending_candidates[t->pending_candidate_count].mid, mid, 63);
            } else {
                t->pending_candidates[t->pending_candidate_count].mid[0] = '\0';
            }
            t->pending_candidate_count++;
        } else {
            pthread_mutex_unlock(&t->lock);
            return -1; /* Queue overflow */
        }
    } else {
        /* Apply candidate immediately */
    }

    pthread_mutex_unlock(&t->lock);
    return 0;
}

int webrtc_transport_send_rtp(webrtc_transport_t *t, const uint8_t *packet, size_t size) {
    if (!t || !packet || size == 0) {
        return -1;
    }

    pthread_mutex_lock(&t->lock);
    if (t->state != WEBRTC_STATE_CONNECTED && t->state != WEBRTC_STATE_CONNECTING) {
        pthread_mutex_unlock(&t->lock);
        return -1;
    }
    pthread_mutex_unlock(&t->lock);

    return 0;
}

webrtc_state_t webrtc_transport_get_state(webrtc_transport_t *t) {
    if (!t) {
        return WEBRTC_STATE_CLOSED;
    }
    pthread_mutex_lock(&t->lock);
    webrtc_state_t s = t->state;
    pthread_mutex_unlock(&t->lock);
    return s;
}
