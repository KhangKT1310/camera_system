#include "media_frame.h"
#include <stdlib.h>

static void default_buffer_release(media_buffer_t *buf) {
    if (!buf) {
        return;
    }
    if (buf->data) {
        free(buf->data);
        buf->data = NULL;
    }
    free(buf);
}

media_buffer_t *media_buffer_create(size_t capacity) {
    if (capacity == 0) {
        return NULL;
    }

    media_buffer_t *buf = (media_buffer_t *)calloc(1, sizeof(media_buffer_t));
    if (!buf) {
        return NULL;
    }

    buf->data = (uint8_t *)malloc(capacity);
    if (!buf->data) {
        free(buf);
        return NULL;
    }

    buf->capacity = capacity;
    buf->size = 0;
    atomic_init(&buf->refcount, 1);
    buf->release_fn = default_buffer_release;
    buf->owner = NULL;

    return buf;
}

void media_buffer_retain(media_buffer_t *buf) {
    if (!buf) {
        return;
    }
    atomic_fetch_add(&buf->refcount, 1);
}

void media_buffer_release(media_buffer_t *buf) {
    if (!buf) {
        return;
    }
    if (atomic_fetch_sub(&buf->refcount, 1) == 1) {
        if (buf->release_fn) {
            buf->release_fn(buf);
        } else {
            default_buffer_release(buf);
        }
    }
}
