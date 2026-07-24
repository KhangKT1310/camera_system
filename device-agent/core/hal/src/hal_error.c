#include "hal/hal_error.h"

const char *hal_status_string(hal_status_t status) {
    switch (status) {
        case HAL_OK:
            return "HAL_OK";
        case HAL_ERR_UNKNOWN:
            return "HAL_ERR_UNKNOWN";
        case HAL_ERR_INVALID_ARGUMENT:
            return "HAL_ERR_INVALID_ARGUMENT";
        case HAL_ERR_NOT_SUPPORTED:
            return "HAL_ERR_NOT_SUPPORTED";
        case HAL_ERR_NOT_FOUND:
            return "HAL_ERR_NOT_FOUND";
        case HAL_ERR_ALREADY_EXISTS:
            return "HAL_ERR_ALREADY_EXISTS";
        case HAL_ERR_BUSY:
            return "HAL_ERR_BUSY";
        case HAL_ERR_TIMEOUT:
            return "HAL_ERR_TIMEOUT";
        case HAL_ERR_AGAIN:
            return "HAL_ERR_AGAIN";
        case HAL_ERR_NO_MEMORY:
            return "HAL_ERR_NO_MEMORY";
        case HAL_ERR_NO_SPACE:
            return "HAL_ERR_NO_SPACE";
        case HAL_ERR_IO:
            return "HAL_ERR_IO";
        case HAL_ERR_PERMISSION:
            return "HAL_ERR_PERMISSION";
        case HAL_ERR_AUTHENTICATION:
            return "HAL_ERR_AUTHENTICATION";
        case HAL_ERR_INTEGRITY:
            return "HAL_ERR_INTEGRITY";
        case HAL_ERR_NOT_READY:
            return "HAL_ERR_NOT_READY";
        case HAL_ERR_INVALID_STATE:
            return "HAL_ERR_INVALID_STATE";
        case HAL_ERR_DEVICE_FAILURE:
            return "HAL_ERR_DEVICE_FAILURE";
        case HAL_ERR_SENSOR_FAILURE:
            return "HAL_ERR_SENSOR_FAILURE";
        case HAL_ERR_ENCODER_FAILURE:
            return "HAL_ERR_ENCODER_FAILURE";
        case HAL_ERR_STORAGE_FAILURE:
            return "HAL_ERR_STORAGE_FAILURE";
        case HAL_ERR_NETWORK_FAILURE:
            return "HAL_ERR_NETWORK_FAILURE";
        case HAL_ERR_CLOCK_UNSYNCED:
            return "HAL_ERR_CLOCK_UNSYNCED";
        case HAL_ERR_REBOOT_REQUIRED:
            return "HAL_ERR_REBOOT_REQUIRED";
        default:
            return "HAL_ERR_UNRECOGNIZED";
    }
}
