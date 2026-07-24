#ifndef CAMERA_HAL_UPDATE_H
#define CAMERA_HAL_UPDATE_H

#include "hal_error.h"
#include "hal_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HAL_UPDATE_STATE_IDLE = 0,
    HAL_UPDATE_STATE_STAGING,
    HAL_UPDATE_STATE_VALIDATING,
    HAL_UPDATE_STATE_READY_TO_ACTIVATE,
    HAL_UPDATE_STATE_ACTIVATING,
    HAL_UPDATE_STATE_FAILED
} hal_update_state_t;

typedef struct {
    hal_update_state_t state;
    uint32_t progress_percent;
    char error_message[128];
} hal_update_status_t;

hal_status_t hal_get_update_state(hal_update_status_t *status);
hal_status_t hal_stage_update(const char *artifact_path, const char *manifest_path);
hal_status_t hal_validate_staged_update(void);
hal_status_t hal_activate_staged_update(void);
hal_status_t hal_rollback_to_previous_slot(void);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_HAL_UPDATE_H */
