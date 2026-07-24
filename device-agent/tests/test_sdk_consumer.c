#include <assert.h>
#include <stdio.h>
#include "hal/hal.h"

int main(void) {
    printf("Testing SDK Consumer Integration...\n");

    /* Verify hal_status_string from hal_interface */
    const char *str = hal_status_string(HAL_OK);
    assert(str != NULL);

    /* Verify hal_init & hal_get_api_version from platform implementation */
    hal_status_t status = hal_init(NULL);
    assert(status == HAL_OK);

    hal_version_t ver = hal_get_api_version();
    assert(ver.major >= 1);

    hal_platform_capabilities_t caps;
    status = hal_get_platform_capabilities(&caps);
    assert(status == HAL_OK);
    assert(caps.media.video_capture == HAL_CAPABILITY_REQUIRED);

    hal_shutdown();
    printf("SDK Consumer Integration Test PASSED cleanly!\n");
    return 0;
}
