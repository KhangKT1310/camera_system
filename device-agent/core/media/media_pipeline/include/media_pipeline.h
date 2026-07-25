#ifndef CAMERA_SYSTEM_MEDIA_PIPELINE_H
#define CAMERA_SYSTEM_MEDIA_PIPELINE_H

#include "media_frame.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct media_pipeline media_pipeline_t;

typedef void (*media_pipeline_frame_cb)(const media_frame_t *frame, void *user_data);

typedef struct {
    size_t max_queue_depth;
    bool drop_oldest_on_overflow;
} media_pipeline_config_t;

/**
 * @brief Creates a media pipeline instance.
 * @param config Configuration parameters.
 * @param out_pipeline Pointer to receive created handle.
 * @return 0 on success, negative error code on failure.
 */
int media_pipeline_create(const media_pipeline_config_t *config, media_pipeline_t **out_pipeline);

/**
 * @brief Destroys a media pipeline instance and flushes pending frames.
 * @param pipeline Handle to pipeline.
 */
void media_pipeline_destroy(media_pipeline_t *pipeline);

/**
 * @brief Subscribes a callback listener to receive media frames.
 * @param pipeline Target media pipeline handle.
 * @param callback Frame listener callback function.
 * @param user_data Opaque pointer passed to callback.
 * @return 0 on success, negative error code on failure.
 */
int media_pipeline_subscribe(media_pipeline_t *pipeline, media_pipeline_frame_cb callback, void *user_data);

/**
 * @brief Unsubscribes a callback listener.
 * @param pipeline Target media pipeline handle.
 * @param callback Listener callback to remove.
 * @param user_data Opaque pointer associated with listener.
 * @return 0 on success, negative error code on failure.
 */
int media_pipeline_unsubscribe(media_pipeline_t *pipeline, media_pipeline_frame_cb callback, void *user_data);

/**
 * @brief Pushes a media frame into the pipeline for distribution to subscribers.
 * Increments reference count on the frame's media_buffer_t.
 * @param pipeline Target media pipeline handle.
 * @param frame Pointer to media_frame_t.
 * @return 0 on success, negative error code on failure.
 */
int media_pipeline_push_frame(media_pipeline_t *pipeline, const media_frame_t *frame);

/**
 * @brief Flushes all queued frames from the pipeline.
 * @param pipeline Target media pipeline handle.
 */
void media_pipeline_flush(media_pipeline_t *pipeline);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_SYSTEM_MEDIA_PIPELINE_H */
