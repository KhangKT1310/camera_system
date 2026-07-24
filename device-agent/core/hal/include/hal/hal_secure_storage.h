#ifndef CAMERA_HAL_SECURE_STORAGE_H
#define CAMERA_HAL_SECURE_STORAGE_H

#include "hal_error.h"
#include "hal_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    bool hardware_protected;
    bool non_exportable;
} hal_secret_policy_t;

hal_status_t hal_store_secret(const char *name, const void *data, size_t size, const hal_secret_policy_t *policy);
hal_status_t hal_load_secret(const char *name, void *buffer, size_t buffer_size, size_t *out_size);
hal_status_t hal_delete_secret(const char *name);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_HAL_SECURE_STORAGE_H */
