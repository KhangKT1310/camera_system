#include "webrtc_transport.h"
#include "libdatachannel_adapter.h"
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
    atomic_int active_callers;
    atomic_int ref_count;
};

static void set_state(webrtc_transport_t *t, webrtc_state_t new_state) {
    if (t->state != new_state) {
        t->state = new_state;
        if (t->callbacks.on_state_changed) {
            t->callbacks.on_state_changed(t->user_data, new_state);
        }
    }
}

static void on_ldc_local_description(void *user, const char *type, const char *sdp) {
    webrtc_transport_t *t = (webrtc_transport_t *)user;
    if (t && t->callbacks.on_local_description) {
        t->callbacks.on_local_description(t->user_data, type, sdp);
    }
}

static void on_ldc_local_candidate(void *user, const char *candidate, const char *mid) {
    webrtc_transport_t *t = (webrtc_transport_t *)user;
    if (t && t->callbacks.on_local_candidate) {
        t->callbacks.on_local_candidate(t->user_data, candidate, mid);
    }
}

static void on_ldc_state_changed(void *user, webrtc_state_t state) {
    webrtc_transport_t *t = (webrtc_transport_t *)user;
    if (t) {
        pthread_mutex_lock(&t->lock);
        set_state(t, state);
        pthread_mutex_unlock(&t->lock);
    }
}

static ldc_backend_t *acquire_backend(webrtc_transport_t *t) {
    if (!t) return NULL;
    pthread_mutex_lock(&t->lock);
    if (t->state == WEBRTC_STATE_CLOSED || !t->backend_ctx || !t->config.enable_libdatachannel) {
        pthread_mutex_unlock(&t->lock);
        return NULL;
    }
    atomic_fetch_add(&t->active_callers, 1);
    ldc_backend_t *b = (ldc_backend_t *)t->backend_ctx;
    pthread_mutex_unlock(&t->lock);
    return b;
}

static void release_backend(webrtc_transport_t *t) {
    if (t) {
        atomic_fetch_sub(&t->active_callers, 1);
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
    atomic_init(&t->active_callers, 0);
    atomic_init(&t->ref_count, 1);

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    if (pthread_mutex_init(&t->lock, &attr) != 0) {
        pthread_mutexattr_destroy(&attr);
        free(t);
        return -1;
    }
    pthread_mutexattr_destroy(&attr);

    if (t->config.enable_libdatachannel) {
        ldc_callbacks_t ldc_cbs = {
            .on_local_description = on_ldc_local_description,
            .on_local_candidate = on_ldc_local_candidate,
            .on_state_changed = on_ldc_state_changed,
            .on_keyframe_requested = NULL
        };
        ldc_backend_create(&t->config, &ldc_cbs, t, (ldc_backend_t **)&t->backend_ctx);
    }

    *out_transport = t;
    return 0;
}

webrtc_transport_t *webrtc_transport_retain(webrtc_transport_t *t) {
    if (!t) return NULL;
    atomic_fetch_add(&t->ref_count, 1);
    return t;
}

void webrtc_transport_release(webrtc_transport_t *t) {
    if (!t) return;
    if (atomic_fetch_sub(&t->ref_count, 1) == 1) {
        pthread_mutex_lock(&t->lock);
        ldc_backend_t *b = (ldc_backend_t *)t->backend_ctx;
        t->backend_ctx = NULL;
        pthread_mutex_unlock(&t->lock);

        while (atomic_load(&t->active_callers) > 0) {
            struct timespec ts = { .tv_sec = 0, .tv_nsec = 1000000 };
            nanosleep(&ts, NULL);
        }

        if (b) {
            ldc_backend_destroy(b);
        }

        pthread_mutex_destroy(&t->lock);
        free(t);
    }
}

void webrtc_transport_destroy(webrtc_transport_t *t) {
    if (!t) {
        return;
    }

    pthread_mutex_lock(&t->lock);
    set_state(t, WEBRTC_STATE_CLOSED);
    pthread_mutex_unlock(&t->lock);

    webrtc_transport_release(t);
}

int webrtc_transport_create_offer(webrtc_transport_t *t) {
    if (!t) {
        return -1;
    }

    webrtc_transport_retain(t);

    pthread_mutex_lock(&t->lock);
    if (t->state == WEBRTC_STATE_CLOSED) {
        pthread_mutex_unlock(&t->lock);
        webrtc_transport_release(t);
        return -1;
    }
    set_state(t, WEBRTC_STATE_CREATING);
    set_state(t, WEBRTC_STATE_NEGOTIATING);
    pthread_mutex_unlock(&t->lock);

    ldc_backend_t *b = acquire_backend(t);
    if (b) {
        int ret = ldc_backend_create_offer(b);
        release_backend(t);
        webrtc_transport_release(t);
        return ret;
    }

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

    pthread_mutex_lock(&t->lock);
    if (t->callbacks.on_local_description) {
        t->callbacks.on_local_description(t->user_data, "offer", mock_offer_sdp);
    }

    if (t->callbacks.on_local_candidate) {
        t->callbacks.on_local_candidate(t->user_data, "candidate:1 1 UDP 2122260223 127.0.0.1 50000 typ host", "video");
    }
    pthread_mutex_unlock(&t->lock);

    webrtc_transport_release(t);
    return 0;
}

int webrtc_transport_set_remote_description(webrtc_transport_t *t, const char *type, const char *sdp) {
    if (!t || !type || !sdp || strlen(sdp) == 0) {
        return -1;
    }

    webrtc_transport_retain(t);

    pthread_mutex_lock(&t->lock);
    if (t->state == WEBRTC_STATE_CLOSED) {
        pthread_mutex_unlock(&t->lock);
        webrtc_transport_release(t);
        return -1;
    }
    t->has_remote_description = true;
    set_state(t, WEBRTC_STATE_CONNECTING);

    pending_candidate_t pending[WEBRTC_MAX_PENDING_CANDIDATES];
    size_t pending_count = t->pending_candidate_count;
    for (size_t i = 0; i < pending_count; i++) {
        pending[i] = t->pending_candidates[i];
    }
    t->pending_candidate_count = 0;
    pthread_mutex_unlock(&t->lock);

    ldc_backend_t *b = acquire_backend(t);
    if (b) {
        ldc_backend_set_remote_description(b, type, sdp);
        for (size_t i = 0; i < pending_count; i++) {
            ldc_backend_add_candidate(b, pending[i].candidate, pending[i].mid);
        }
        release_backend(t);
    } else {
        pthread_mutex_lock(&t->lock);
        set_state(t, WEBRTC_STATE_CONNECTED);
        pthread_mutex_unlock(&t->lock);
    }

    webrtc_transport_release(t);
    return 0;
}

int webrtc_transport_add_remote_candidate(webrtc_transport_t *t, const char *candidate, const char *mid) {
    if (!t || !candidate || strlen(candidate) == 0) {
        return -1;
    }

    webrtc_transport_retain(t);

    pthread_mutex_lock(&t->lock);
    if (t->state == WEBRTC_STATE_CLOSED) {
        pthread_mutex_unlock(&t->lock);
        webrtc_transport_release(t);
        return -1;
    }

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
            pthread_mutex_unlock(&t->lock);
            webrtc_transport_release(t);
            return 0;
        } else {
            pthread_mutex_unlock(&t->lock);
            webrtc_transport_release(t);
            return -1; /* Queue overflow */
        }
    }
    pthread_mutex_unlock(&t->lock);

    ldc_backend_t *b = acquire_backend(t);
    if (b) {
        ldc_backend_add_candidate(b, candidate, mid);
        release_backend(t);
    }

    webrtc_transport_release(t);
    return 0;
}

int webrtc_transport_send_rtp(webrtc_transport_t *t, const uint8_t *packet, size_t size) {
    if (!t || !packet || size == 0) {
        return -1;
    }

    webrtc_transport_retain(t);

    pthread_mutex_lock(&t->lock);
    if (t->state != WEBRTC_STATE_CONNECTED && t->state != WEBRTC_STATE_CONNECTING) {
        pthread_mutex_unlock(&t->lock);
        webrtc_transport_release(t);
        return -1;
    }
    pthread_mutex_unlock(&t->lock);

    ldc_backend_t *b = acquire_backend(t);
    if (b) {
        int ret = ldc_backend_send_rtp(b, packet, size);
        release_backend(t);
        webrtc_transport_release(t);
        return ret;
    }

    webrtc_transport_release(t);
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
