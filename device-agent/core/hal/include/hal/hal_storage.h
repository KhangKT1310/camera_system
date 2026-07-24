#ifndef CAMERA_HAL_STORAGE_H
#define CAMERA_HAL_STORAGE_H

#include "hal_error.h"
#include "hal_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HAL_VOLUME_PURPOSE_CONFIG = 0,
    HAL_VOLUME_PURPOSE_RECORDING,
    HAL_VOLUME_PURPOSE_EVENT_RECORDING,
    HAL_VOLUME_PURPOSE_SNAPSHOT,
    HAL_VOLUME_PURPOSE_LOG,
    HAL_VOLUME_PURPOSE_UPDATE,
    HAL_VOLUME_PURPOSE_TEMPORARY
} hal_volume_purpose_t;

typedef struct {
    uint64_t total_bytes;
    uint64_t free_bytes;
    uint64_t available_bytes;
    bool read_only;
    bool healthy;
} hal_capacity_info_t;

hal_status_t hal_open_volume(uint32_t volume_id, hal_volume_purpose_t purpose, hal_volume_t *volume);
hal_status_t hal_write_object(hal_volume_t volume, const char *path, const void *data, size_t size);
hal_status_t hal_read_object(hal_volume_t volume, const char *path, void *buffer, size_t buffer_size, size_t *out_size);
hal_status_t hal_delete_object(hal_volume_t volume, const char *path);
hal_status_t hal_atomic_replace(hal_volume_t volume, const char *path, const void *data, size_t size);
hal_status_t hal_stat_capacity(hal_volume_t volume, hal_capacity_info_t *info);
hal_status_t hal_fsync_scope(hal_volume_t volume);
hal_status_t hal_close_volume(hal_volume_t volume);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_HAL_STORAGE_H */
