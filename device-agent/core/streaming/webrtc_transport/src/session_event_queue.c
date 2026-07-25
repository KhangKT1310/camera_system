#include "session_event_queue.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

struct session_event_queue {
    session_event_t ring[SESSION_EVENT_QUEUE_CAPACITY];
    size_t head;
    size_t tail;
    size_t count;
    bool keyframe_request_pending;
    pthread_mutex_t lock;
};

int session_event_queue_create(session_event_queue_t **out_queue) {
    if (!out_queue) {
        return -1;
    }

    session_event_queue_t *q = (session_event_queue_t *)calloc(1, sizeof(session_event_queue_t));
    if (!q) {
        return -1;
    }

    q->head = 0;
    q->tail = 0;
    q->count = 0;
    q->keyframe_request_pending = false;

    if (pthread_mutex_init(&q->lock, NULL) != 0) {
        free(q);
        return -1;
    }

    *out_queue = q;
    return 0;
}

void session_event_queue_destroy(session_event_queue_t *q) {
    if (!q) {
        return;
    }

    pthread_mutex_lock(&q->lock);
    /* Release any pending SDP pool handles */
    for (size_t i = 0; i < q->count; i++) {
        size_t idx = (q->head + i) % SESSION_EVENT_QUEUE_CAPACITY;
        if (q->ring[idx].type == SESSION_EVENT_LOCAL_DESCRIPTION) {
            sdp_pool_release(q->ring[idx].data.local_description.handle);
        }
    }
    pthread_mutex_unlock(&q->lock);

    pthread_mutex_destroy(&q->lock);
    free(q);
}

int session_event_queue_push(session_event_queue_t *q, const session_event_t *evt) {
    if (!q || !evt) {
        return -1;
    }

    pthread_mutex_lock(&q->lock);

    /* Coalesce duplicate PLI keyframe requests */
    if (evt->type == SESSION_EVENT_KEYFRAME_REQUESTED) {
        if (q->keyframe_request_pending) {
            pthread_mutex_unlock(&q->lock);
            return 0; /* Coalesced */
        }
        q->keyframe_request_pending = true;
    }

    if (q->count >= SESSION_EVENT_QUEUE_CAPACITY) {
        pthread_mutex_unlock(&q->lock);
        return -1; /* Queue overflow for critical events */
    }

    q->ring[q->tail] = *evt;
    q->tail = (q->tail + 1) % SESSION_EVENT_QUEUE_CAPACITY;
    q->count++;

    pthread_mutex_unlock(&q->lock);
    return 0;
}

int session_event_queue_pop(session_event_queue_t *q, session_event_t *out_evt) {
    if (!q || !out_evt) {
        return -1;
    }

    pthread_mutex_lock(&q->lock);

    if (q->count == 0) {
        pthread_mutex_unlock(&q->lock);
        return -1;
    }

    *out_evt = q->ring[q->head];
    q->head = (q->head + 1) % SESSION_EVENT_QUEUE_CAPACITY;
    q->count--;

    if (out_evt->type == SESSION_EVENT_KEYFRAME_REQUESTED) {
        q->keyframe_request_pending = false;
    }

    pthread_mutex_unlock(&q->lock);
    return 0;
}
