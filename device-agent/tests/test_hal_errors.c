#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "hal/hal.h"

int main(void) {
    printf("Running test_hal_errors...\n");

    const char *str_ok = hal_status_string(HAL_OK);
    assert(strcmp(str_ok, "HAL_OK") == 0);

    const char *str_invalid = hal_status_string(HAL_ERR_INVALID_ARGUMENT);
    assert(strcmp(str_invalid, "HAL_ERR_INVALID_ARGUMENT") == 0);

    const char *str_unsupported = hal_status_string(HAL_ERR_NOT_SUPPORTED);
    assert(strcmp(str_unsupported, "HAL_ERR_NOT_SUPPORTED") == 0);

    printf("Error strings checked successfully.\n");
    printf("test_hal_errors PASSED!\n");
    return 0;
}
