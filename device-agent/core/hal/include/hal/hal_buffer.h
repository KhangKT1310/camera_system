#ifndef CAMERA_HAL_BUFFER_H
#define CAMERA_HAL_BUFFER_H

#include "hal_error.h"
#include "hal_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HAL_MAX_PLANES 4

typedef struct {
    void *address;
    size_t size;
    uint32_t stride;
} hal_buffer_plane_t;

typedef struct {
    size_t size;
    uint32_t plane_count;
    hal_memory_type_t memory_type;
    bool cpu_accessible;
    bool physically_contiguous;
    hal_buffer_plane_t planes[HAL_MAX_PLANES];
} hal_buffer_info_t;

typedef enum {
    HAL_MAP_READ = 1,
    HAL_MAP_WRITE = 2,
    HAL_MAP_READ_WRITE = 3
} hal_map_mode_t;

hal_status_t hal_buffer_retain(hal_buffer_t buffer);
hal_status_t hal_buffer_release(hal_buffer_t buffer);
hal_status_t hal_buffer_get_info(hal_buffer_t buffer, hal_buffer_info_t *info);

hal_status_t hal_buffer_map(hal_buffer_t buffer, hal_map_mode_t mode, void **address);
hal_status_t hal_buffer_unmap(hal_buffer_t buffer);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_HAL_BUFFER_H */
