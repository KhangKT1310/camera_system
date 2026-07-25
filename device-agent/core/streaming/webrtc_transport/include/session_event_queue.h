#ifndef CAMERA_SYSTEM_SESSION_EVENT_QUEUE_H
#define CAMERA_SYSTEM_SESSION_EVENT_QUEUE_H

#include "sdp_pool.h"
#include "webrtc_transport.h"
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SESSION_EVENT_QUEUE_CAPACITY 32

typedef enum {
    SESSION_EVENT_LOCAL_DESCRIPTION = 1,
    SESSION_EVENT_LOCAL_CANDIDATE = 2,
    SESSION_EVENT_STATE_CHANGED = 3,
    SESSION_EVENT_KEYFRAME_REQUESTED = 4,
    SESSION_EVENT_NEGOTIATION_TIMEOUT = 5
} session_event_type_t;

typedef struct {
    session_event_type_t type;
    union {
        struct {
            sdp_buffer_handle_t handle;
            size_t size;
        } local_description;
        struct {
            char candidate[WEBRTC_MAX_CANDIDATE_LENGTH];
            char mid[32];
        } local_candidate;
        struct {
            webrtc_state_t state;
        } state_changed;
    } data;
} session_event_t;

typedef struct session_event_queue session_event_queue_t;

int session_event_queue_create(session_event_queue_t **out_queue);
void session_event_queue_destroy(session_event_queue_t *queue);

int session_event_queue_push(session_event_queue_t *queue, const session_event_t *evt);
int session_event_queue_pop(session_event_queue_t *queue, session_event_t *out_evt);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_SYSTEM_SESSION_EVENT_QUEUE_H */
