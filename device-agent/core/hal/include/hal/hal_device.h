#ifndef CAMERA_HAL_DEVICE_H
#define CAMERA_HAL_DEVICE_H

#include "hal_error.h"
#include "hal_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HAL_REBOOT_REASON_USER_REQUEST = 0,
    HAL_REBOOT_REASON_UPDATE_COMPLETE,
    HAL_REBOOT_REASON_SYSTEM_FAILURE,
    HAL_REBOOT_REASON_FACTORY_RESET
} hal_reboot_reason_t;

typedef enum {
    HAL_FACTORY_RESET_USER_SETTINGS_ONLY = 0,
    HAL_FACTORY_RESET_CREDENTIALS,
    HAL_FACTORY_RESET_FULL_USER_DATA,
    HAL_FACTORY_RESET_SECURE_FULL_RESET
} hal_factory_reset_mode_t;

hal_status_t hal_reboot(hal_reboot_reason_t reason);
hal_status_t hal_factory_reset(hal_factory_reset_mode_t mode);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_HAL_DEVICE_H */
