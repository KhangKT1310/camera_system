#ifndef CAMERA_HAL_CRYPTO_H
#define CAMERA_HAL_CRYPTO_H

#include "hal_error.h"
#include "hal_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char key_type[32];
    bool hardware_backed;
} hal_trust_store_info_t;

hal_status_t hal_generate_random(void *buffer, size_t length);
hal_status_t hal_verify_signature(const uint8_t *digest, size_t digest_len, const uint8_t *sig, size_t sig_len, const char *key_ref);
hal_status_t hal_get_trust_store_info(hal_trust_store_info_t *info);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_HAL_CRYPTO_H */
