#include "sdp_pool.h"
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    char data[SDP_POOL_MAX_SIZE];
    size_t size;
    bool in_use;
} sdp_slot_t;

static sdp_slot_t g_sdp_slots[SDP_POOL_SLOTS];
static pthread_mutex_t g_pool_lock = PTHREAD_MUTEX_INITIALIZER;

void sdp_pool_init(void) {
    pthread_mutex_lock(&g_pool_lock);
    for (size_t i = 0; i < SDP_POOL_SLOTS; i++) {
        g_sdp_slots[i].in_use = false;
        g_sdp_slots[i].size = 0;
        g_sdp_slots[i].data[0] = '\0';
    }
    pthread_mutex_unlock(&g_pool_lock);
}

sdp_buffer_handle_t sdp_pool_acquire(const char *sdp, size_t len) {
    if (!sdp || len == 0 || len >= SDP_POOL_MAX_SIZE) {
        return SDP_INVALID_HANDLE;
    }

    pthread_mutex_lock(&g_pool_lock);
    sdp_buffer_handle_t handle = SDP_INVALID_HANDLE;

    for (size_t i = 0; i < SDP_POOL_SLOTS; i++) {
        if (!g_sdp_slots[i].in_use) {
            g_sdp_slots[i].in_use = true;
            memcpy(g_sdp_slots[i].data, sdp, len);
            g_sdp_slots[i].data[len] = '\0';
            g_sdp_slots[i].size = len;
            handle = (sdp_buffer_handle_t)i;
            break;
        }
    }

    pthread_mutex_unlock(&g_pool_lock);
    return handle;
}

const char *sdp_pool_get(sdp_buffer_handle_t handle) {
    if (handle >= SDP_POOL_SLOTS) {
        return NULL;
    }
    const char *res = NULL;
    pthread_mutex_lock(&g_pool_lock);
    if (g_sdp_slots[handle].in_use) {
        res = g_sdp_slots[handle].data;
    }
    pthread_mutex_unlock(&g_pool_lock);
    return res;
}

void sdp_pool_release(sdp_buffer_handle_t handle) {
    if (handle >= SDP_POOL_SLOTS) {
        return;
    }
    pthread_mutex_lock(&g_pool_lock);
    g_sdp_slots[handle].in_use = false;
    g_sdp_slots[handle].size = 0;
    g_sdp_slots[handle].data[0] = '\0';
    pthread_mutex_unlock(&g_pool_lock);
}
