/**
 * @file media_frame.h
 * @brief Zero-Copy Ref-Counted Media Buffer and Frame Definitions for Camera Subsystem.
 */

#ifndef CAMERA_SYSTEM_MEDIA_FRAME_H
#define CAMERA_SYSTEM_MEDIA_FRAME_H

#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Supported media codecs in the camera system.
 */
typedef enum {
    MEDIA_CODEC_UNKNOWN = 0,
    MEDIA_CODEC_H264 = 1,
    MEDIA_CODEC_H265 = 2,
    MEDIA_CODEC_AAC = 3,
    MEDIA_CODEC_OPUS = 4
} media_codec_t;

/**
 * @brief Media frame payload type classification.
 */
typedef enum {
    MEDIA_FRAME_TYPE_UNKNOWN = 0,
    MEDIA_FRAME_TYPE_IDR = 1,
    MEDIA_FRAME_TYPE_P_FRAME = 2,
    MEDIA_FRAME_TYPE_B_FRAME = 3,
    MEDIA_FRAME_TYPE_SPS_PPS = 4,
    MEDIA_FRAME_TYPE_AUDIO = 5
} media_frame_type_t;

/**
 * @brief Timestamp metadata structure for frame synchronization.
 */
typedef struct {
    uint64_t pts_us;            /**< Presentation timestamp in microseconds. */
    uint64_t capture_time_us;   /**< HAL hardware capture timestamp in microseconds. */
    uint64_t sequence;          /**< Monotonically increasing frame sequence number. */
} media_timestamp_t;

/**
 * @brief Zero-copy ref-counted media buffer structure.
 */
typedef struct media_buffer {
    uint8_t *data;              /**< Pointer to payload byte buffer. */
    size_t capacity;            /**< Total allocated capacity in bytes. */
    size_t size;                /**< Current valid payload size in bytes. */
    atomic_uint refcount;       /**< Atomic reference counter. */
    void (*release_fn)(struct media_buffer *buf); /**< Custom release function callback. */
    void *owner;                /**< Opaque pointer to owner/context. */
} media_buffer_t;

/**
 * @brief High-level media frame encapsulation for pipeline distribution.
 */
typedef struct {
    media_buffer_t *buffer;     /**< Ref-counted underlying payload buffer. */
    const uint8_t *data;        /**< Pointer to frame payload slice. */
    size_t size;                /**< Size of frame payload slice. */
    media_timestamp_t timestamp;/**< Frame timestamp metadata. */
    media_codec_t codec;        /**< Frame codec identifier. */
    media_frame_type_t frame_type; /**< Frame NALU/slice type. */
    uint32_t flags;             /**< Operational frame flags. */
} media_frame_t;

/**
 * @brief Allocates a new media buffer with default malloc-backed storage and release callback.
 * 
 * @param[in] capacity Maximum payload capacity of the buffer in bytes. Must be > 0.
 * 
 * @return Pointer to newly allocated media_buffer_t with refcount = 1, or NULL on memory failure.
 * 
 * @note Memory Ownership: Caller receives 1 reference and MUST call media_buffer_release() when finished.
 */
media_buffer_t *media_buffer_create(size_t capacity);

/**
 * @brief Atomically increments reference counter of a media buffer.
 * 
 * @param[in] buf Target media buffer. If NULL, operation is a no-op.
 * 
 * @note Thread-Safety: Safe to invoke concurrently across threads.
 */
void media_buffer_retain(media_buffer_t *buf);

/**
 * @brief Atomically decrements reference counter of a media buffer.
 * 
 * If the reference count reaches 0, the buffer memory is released via release_fn or free().
 * 
 * @param[in] buf Target media buffer. If NULL, operation is a no-op.
 * 
 * @note Thread-Safety: Safe to invoke concurrently across threads.
 */
void media_buffer_release(media_buffer_t *buf);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_SYSTEM_MEDIA_FRAME_H */
