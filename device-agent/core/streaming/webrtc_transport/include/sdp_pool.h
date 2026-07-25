#ifndef CAMERA_SYSTEM_SDP_POOL_H
#define CAMERA_SYSTEM_SDP_POOL_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SDP_POOL_MAX_SIZE (32 * 1024)
#define SDP_POOL_SLOTS 2
#define SDP_INVALID_HANDLE 0xFFFF

typedef uint16_t sdp_buffer_handle_t;

void sdp_pool_init(void);

/**
 * @brief Acquires an SDP buffer slot from pool and copies content.
 * @param sdp SDP string to store.
 * @param len Length of SDP string.
 * @return Slot handle, or SDP_INVALID_HANDLE if pool is full.
 */
sdp_buffer_handle_t sdp_pool_acquire(const char *sdp, size_t len);

/**
 * @brief Gets pointer to SDP string stored at slot handle.
 * @param handle Slot handle.
 * @return Const pointer to SDP string, or NULL if handle invalid.
 */
const char *sdp_pool_get(sdp_buffer_handle_t handle);

/**
 * @brief Releases an acquired SDP buffer slot back to pool.
 * @param handle Slot handle to release.
 */
void sdp_pool_release(sdp_buffer_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_SYSTEM_SDP_POOL_H */
