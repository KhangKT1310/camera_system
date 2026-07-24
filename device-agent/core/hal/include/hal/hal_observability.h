#ifndef CAMERA_HAL_OBSERVABILITY_H
#define CAMERA_HAL_OBSERVABILITY_H

#include "hal_error.h"
#include "hal_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HAL_HEALTH_LEVEL_OK = 0,
    HAL_HEALTH_LEVEL_WARNING,
    HAL_HEALTH_LEVEL_ERROR,
    HAL_HEALTH_LEVEL_FATAL
} hal_health_level_t;

typedef struct {
    hal_health_level_t level;
    char module_name[32];
    char message[128];
    uint64_t timestamp_ns;
} hal_health_event_t;

typedef struct {
    char event_type[32];
    char details[128];
    uint64_t timestamp_ns;
} hal_audit_event_t;

typedef struct {
    char metric_name[32];
    double value;
    uint64_t timestamp_ns;
} hal_metric_sample_t;

hal_status_t hal_emit_health_event(const hal_health_event_t *event);
hal_status_t hal_emit_audit_event(const hal_audit_event_t *event);
hal_status_t hal_record_metric(const hal_metric_sample_t *sample);
hal_status_t hal_get_last_reset_reason(char *out_reason, size_t max_len);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_HAL_OBSERVABILITY_H */
