#ifndef CAMERA_HAL_PLATFORM_H
#define CAMERA_HAL_PLATFORM_H

#include "hal_capability.h"
#include "hal_error.h"
#include "hal_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char manufacturer[64];
    char product_name[64];
    char product_model[64];
    char soc_name[64];
    char soc_revision[32];
} hal_platform_info_t;

typedef struct {
    char serial_number[64];
    char device_uuid[64];
    char mac_address[18];
} hal_device_identity_t;

typedef struct {
    bool secure_boot_enabled;
    bool boot_verified;
    char bootloader_version[64];
} hal_boot_integrity_t;

typedef struct {
    bool factory_provisioned;
    bool keys_locked;
    char provisioning_date[32];
} hal_provisioning_state_t;

typedef struct {
    char hw_revision[32];
    char board_revision[32];
} hal_hw_revision_t;

typedef struct {
    char firmware_version[64];
    char build_date[32];
} hal_firmware_version_t;

typedef struct {
    uint32_t flags;
} hal_init_params_t;

/* Global HAL Lifecycle & Info APIs */
hal_status_t hal_init(const hal_init_params_t *params);
void hal_shutdown(void);
hal_version_t hal_get_api_version(void);

hal_status_t hal_get_platform_capabilities(hal_platform_capabilities_t *caps);
hal_status_t hal_query_feature(hal_feature_id_t feature, hal_feature_info_t *info);

/* Platform Identity APIs */
hal_status_t hal_get_platform_info(hal_platform_info_t *info);
hal_status_t hal_get_device_identity(hal_device_identity_t *identity);
hal_status_t hal_get_boot_integrity_state(hal_boot_integrity_t *state);
hal_status_t hal_get_provisioning_state(hal_provisioning_state_t *state);
hal_status_t hal_get_hardware_revision(hal_hw_revision_t *revision);
hal_status_t hal_get_firmware_version(hal_firmware_version_t *version);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_HAL_PLATFORM_H */
