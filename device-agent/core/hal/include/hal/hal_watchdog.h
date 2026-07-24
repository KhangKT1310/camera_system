#ifndef CAMERA_HAL_WATCHDOG_H
#define CAMERA_HAL_WATCHDOG_H

#include "hal_error.h"
#include "hal_types.h"

#ifdef __cplusplus
extern "C" {
#endif

hal_status_t hal_watchdog_open(uint32_t timeout_ms, hal_watchdog_t *watchdog);
hal_status_t hal_watchdog_feed(hal_watchdog_t watchdog);
hal_status_t hal_watchdog_set_timeout(hal_watchdog_t watchdog, uint32_t timeout_ms);
hal_status_t hal_watchdog_close(hal_watchdog_t watchdog);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_HAL_WATCHDOG_H */
