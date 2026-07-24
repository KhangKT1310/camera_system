#ifndef CAMERA_HAL_ERROR_H
#define CAMERA_HAL_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HAL_OK = 0,

    HAL_ERR_UNKNOWN,
    HAL_ERR_INVALID_ARGUMENT,
    HAL_ERR_NOT_SUPPORTED,
    HAL_ERR_NOT_FOUND,
    HAL_ERR_ALREADY_EXISTS,

    HAL_ERR_BUSY,
    HAL_ERR_TIMEOUT,
    HAL_ERR_AGAIN,

    HAL_ERR_NO_MEMORY,
    HAL_ERR_NO_SPACE,
    HAL_ERR_IO,

    HAL_ERR_PERMISSION,
    HAL_ERR_AUTHENTICATION,
    HAL_ERR_INTEGRITY,

    HAL_ERR_NOT_READY,
    HAL_ERR_INVALID_STATE,

    HAL_ERR_DEVICE_FAILURE,
    HAL_ERR_SENSOR_FAILURE,
    HAL_ERR_ENCODER_FAILURE,
    HAL_ERR_STORAGE_FAILURE,
    HAL_ERR_NETWORK_FAILURE,

    HAL_ERR_CLOCK_UNSYNCED,
    HAL_ERR_REBOOT_REQUIRED
} hal_status_t;

/**
 * Convert a hal_status_t enum to a human-readable string representation.
 */
const char *hal_status_string(hal_status_t status);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_HAL_ERROR_H */
