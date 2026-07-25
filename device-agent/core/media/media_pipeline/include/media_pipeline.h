/**
 * @file media_pipeline.h
 * @brief Zero-Copy Media Pipeline Subsystem Interface for Camera Frame Distribution.
 */

#ifndef CAMERA_SYSTEM_MEDIA_PIPELINE_H
#define CAMERA_SYSTEM_MEDIA_PIPELINE_H

#include "media_frame.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaque handle representing a media pipeline instance.
 */
typedef struct media_pipeline media_pipeline_t;

/**
 * @brief Subscriber callback function signature for receiving media frames.
 * 
 * @param[in] frame Pointer to media frame structure containing ref-counted buffer. Must not be modified.
 * @param[in] user_data Opaque user pointer passed during subscription.
 */
typedef void (*media_pipeline_frame_cb)(const media_frame_t *frame, void *user_data);

/**
 * @brief Configuration structure for creating a media pipeline instance.
 */
typedef struct {
    size_t max_queue_depth;       /**< Maximum number of frames allowed in queue before overflow. */
    bool drop_oldest_on_overflow; /**< Drop oldest frame when queue overflows if true. */
} media_pipeline_config_t;

/**
 * @brief Creates a new media pipeline instance.
 * 
 * @param[in] config Configuration parameters. Must not be NULL.
 * @param[out] out_pipeline Pointer to receive created media pipeline handle. Must not be NULL.
 * 
 * @return 0 on success, or a negative error code on failure:
 *         - -1: Invalid input parameter or NULL pointer.
 *         - -2: Memory allocation failure.
 * 
 * @note Thread-Safety: Initialized instance must be configured before multi-threaded frame pushing.
 */
int media_pipeline_create(const media_pipeline_config_t *config, media_pipeline_t **out_pipeline);

/**
 * @brief Destroys a media pipeline instance, unsubscribing all listeners and flushing queued frames.
 * 
 * @param[in] pipeline Handle to media pipeline. If NULL, operation is a no-op.
 * 
 * @note Memory Ownership: Discards internal handles and releases queued buffer references.
 */
void media_pipeline_destroy(media_pipeline_t *pipeline);

/**
 * @brief Subscribes a frame listener callback to receive distributed media frames.
 * 
 * @param[in] pipeline Target media pipeline handle. Must not be NULL.
 * @param[in] callback Listener callback function pointer. Must not be NULL.
 * @param[in] user_data Opaque pointer passed to callback.
 * 
 * @return 0 on success, or negative error code on failure (-1 if invalid or max subscribers reached).
 */
int media_pipeline_subscribe(media_pipeline_t *pipeline, media_pipeline_frame_cb callback, void *user_data);

/**
 * @brief Unsubscribes a previously registered frame listener callback.
 * 
 * @param[in] pipeline Target media pipeline handle. Must not be NULL.
 * @param[in] callback Listener callback function pointer to remove.
 * @param[in] user_data Opaque pointer associated with listener.
 * 
 * @return 0 on success, or negative error code on failure (-1 if listener not found).
 */
int media_pipeline_unsubscribe(media_pipeline_t *pipeline, media_pipeline_frame_cb callback, void *user_data);

/**
 * @brief Pushes a media frame into the pipeline for distribution to all subscribed listeners.
 * 
 * Automatically increments reference count on the frame's media_buffer_t during distribution.
 * 
 * @param[in] pipeline Target media pipeline handle. Must not be NULL.
 * @param[in] frame Pointer to valid media_frame_t. Must not be NULL.
 * 
 * @return 0 on success, or negative error code on failure.
 */
int media_pipeline_push_frame(media_pipeline_t *pipeline, const media_frame_t *frame);

/**
 * @brief Flushes all queued frames from the media pipeline.
 * 
 * @param[in] pipeline Target media pipeline handle. Must not be NULL.
 */
void media_pipeline_flush(media_pipeline_t *pipeline);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_SYSTEM_MEDIA_PIPELINE_H */
