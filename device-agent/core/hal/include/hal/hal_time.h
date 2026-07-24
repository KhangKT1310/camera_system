#ifndef CAMERA_HAL_TIME_H
#define CAMERA_HAL_TIME_H

#include "hal_error.h"
#include "hal_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint64_t seconds_since_epoch;
    uint32_t nanoseconds;
} hal_utc_time_t;

typedef enum {
    HAL_CLOCK_SYNC_UNINITIALIZED = 0,
    HAL_CLOCK_SYNC_UNSYNCED,
    HAL_CLOCK_SYNC_SYNCING,
    HAL_CLOCK_SYNC_SYNCED,
    HAL_CLOCK_SYNC_DEGRADED
} hal_clock_sync_state_t;

hal_timestamp_t hal_now_monotonic(void);
hal_status_t hal_now_utc(hal_utc_time_t *time);
hal_status_t hal_get_clock_sync_state(hal_clock_sync_state_t *state);
hal_status_t hal_set_utc_time(const hal_utc_time_t *time);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_HAL_TIME_H */
