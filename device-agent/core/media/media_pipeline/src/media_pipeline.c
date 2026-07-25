#include "media_pipeline.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SUBSCRIBERS 8

typedef struct {
    media_pipeline_frame_cb callback;
    void *user_data;
} subscriber_entry_t;

struct media_pipeline {
    media_pipeline_config_t config;
    subscriber_entry_t subscribers[MAX_SUBSCRIBERS];
    size_t subscriber_count;
    pthread_mutex_t lock;
};

int media_pipeline_create(const media_pipeline_config_t *config, media_pipeline_t **out_pipeline) {
    if (!out_pipeline) {
        return -1;
    }

    media_pipeline_t *pipeline = (media_pipeline_t *)calloc(1, sizeof(media_pipeline_t));
    if (!pipeline) {
        return -1;
    }

    if (config) {
        pipeline->config = *config;
    } else {
        pipeline->config.max_queue_depth = 30;
        pipeline->config.drop_oldest_on_overflow = true;
    }

    if (pthread_mutex_init(&pipeline->lock, NULL) != 0) {
        free(pipeline);
        return -1;
    }

    *out_pipeline = pipeline;
    return 0;
}

void media_pipeline_destroy(media_pipeline_t *pipeline) {
    if (!pipeline) {
        return;
    }

    pthread_mutex_destroy(&pipeline->lock);
    free(pipeline);
}

int media_pipeline_subscribe(media_pipeline_t *pipeline, media_pipeline_frame_cb callback, void *user_data) {
    if (!pipeline || !callback) {
        return -1;
    }

    pthread_mutex_lock(&pipeline->lock);
    if (pipeline->subscriber_count >= MAX_SUBSCRIBERS) {
        pthread_mutex_unlock(&pipeline->lock);
        return -1;
    }

    for (size_t i = 0; i < pipeline->subscriber_count; i++) {
        if (pipeline->subscribers[i].callback == callback && pipeline->subscribers[i].user_data == user_data) {
            pthread_mutex_unlock(&pipeline->lock);
            return 0;
        }
    }

    pipeline->subscribers[pipeline->subscriber_count].callback = callback;
    pipeline->subscribers[pipeline->subscriber_count].user_data = user_data;
    pipeline->subscriber_count++;

    pthread_mutex_unlock(&pipeline->lock);
    return 0;
}

int media_pipeline_unsubscribe(media_pipeline_t *pipeline, media_pipeline_frame_cb callback, void *user_data) {
    if (!pipeline || !callback) {
        return -1;
    }

    pthread_mutex_lock(&pipeline->lock);
    size_t index = MAX_SUBSCRIBERS;
    for (size_t i = 0; i < pipeline->subscriber_count; i++) {
        if (pipeline->subscribers[i].callback == callback && pipeline->subscribers[i].user_data == user_data) {
            index = i;
            break;
        }
    }

    if (index == MAX_SUBSCRIBERS) {
        pthread_mutex_unlock(&pipeline->lock);
        return -1;
    }

    for (size_t i = index; i < pipeline->subscriber_count - 1; i++) {
        pipeline->subscribers[i] = pipeline->subscribers[i + 1];
    }
    pipeline->subscriber_count--;
    memset(&pipeline->subscribers[pipeline->subscriber_count], 0, sizeof(subscriber_entry_t));

    pthread_mutex_unlock(&pipeline->lock);
    return 0;
}

int media_pipeline_push_frame(media_pipeline_t *pipeline, const media_frame_t *frame) {
    if (!pipeline || !frame || !frame->data || frame->size == 0) {
        return -1;
    }

    pthread_mutex_lock(&pipeline->lock);

    if (frame->buffer) {
        media_buffer_retain(frame->buffer);
    }

    subscriber_entry_t local_subscribers[MAX_SUBSCRIBERS];
    size_t local_count = pipeline->subscriber_count;
    memcpy(local_subscribers, pipeline->subscribers, local_count * sizeof(subscriber_entry_t));

    pthread_mutex_unlock(&pipeline->lock);

    for (size_t i = 0; i < local_count; i++) {
        if (local_subscribers[i].callback) {
            local_subscribers[i].callback(frame, local_subscribers[i].user_data);
        }
    }

    if (frame->buffer) {
        media_buffer_release(frame->buffer);
    }

    return 0;
}

void media_pipeline_flush(media_pipeline_t *pipeline) {
    if (!pipeline) {
        return;
    }
    pthread_mutex_lock(&pipeline->lock);
    pthread_mutex_unlock(&pipeline->lock);
}
