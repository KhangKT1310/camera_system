#ifndef CAMERA_SYSTEM_MEDIA_FRAME_H
#define CAMERA_SYSTEM_MEDIA_FRAME_H

#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MEDIA_CODEC_UNKNOWN = 0,
    MEDIA_CODEC_H264 = 1,
    MEDIA_CODEC_H265 = 2,
    MEDIA_CODEC_AAC = 3,
    MEDIA_CODEC_OPUS = 4
} media_codec_t;

typedef enum {
    MEDIA_FRAME_TYPE_UNKNOWN = 0,
    MEDIA_FRAME_TYPE_IDR = 1,
    MEDIA_FRAME_TYPE_P_FRAME = 2,
    MEDIA_FRAME_TYPE_B_FRAME = 3,
    MEDIA_FRAME_TYPE_SPS_PPS = 4,
    MEDIA_FRAME_TYPE_AUDIO = 5
} media_frame_type_t;

typedef struct {
    uint64_t pts_us;
    uint64_t capture_time_us;
    uint64_t sequence;
} media_timestamp_t;

typedef struct media_buffer {
    uint8_t *data;
    size_t capacity;
    size_t size;
    atomic_uint refcount;
    void (*release_fn)(struct media_buffer *buf);
    void *owner;
} media_buffer_t;

typedef struct {
    media_buffer_t *buffer;
    const uint8_t *data;
    size_t size;
    media_timestamp_t timestamp;
    media_codec_t codec;
    media_frame_type_t frame_type;
    uint32_t flags;
} media_frame_t;

/**
 * @brief Allocates a new media buffer with a default malloc-backed release function.
 * @param capacity Maximum capacity of the buffer payload in bytes.
 * @return Pointer to newly allocated media_buffer_t, or NULL on memory failure.
 */
media_buffer_t *media_buffer_create(size_t capacity);

/**
 * @brief Retains a reference to the media buffer (increments refcount atomically).
 * @param buf Target media buffer.
 */
void media_buffer_retain(media_buffer_t *buf);

/**
 * @brief Releases a reference to the media buffer (decrements refcount atomically).
 * If refcount drops to 0, invokes release_fn or frees memory.
 * @param buf Target media buffer.
 */
void media_buffer_release(media_buffer_t *buf);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_SYSTEM_MEDIA_FRAME_H */
