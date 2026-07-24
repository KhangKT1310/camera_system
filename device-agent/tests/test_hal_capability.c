#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "hal/hal.h"

int main(void) {
    printf("Running test_hal_capability...\n");

    hal_status_t status = hal_init(NULL);
    assert(status == HAL_OK);

    hal_version_t ver = hal_get_api_version();
    assert(ver.major == 1);
    printf("HAL Version: %d.%d.%d\n", ver.major, ver.minor, ver.patch);

    hal_platform_capabilities_t caps;
    status = hal_get_platform_capabilities(&caps);
    assert(status == HAL_OK);
    assert(caps.capability_version.major == 1);

    /* Assert Product Profile Rules from capability-model.md §36-46 */
    assert(caps.security.csrng == HAL_CAPABILITY_REQUIRED);
    assert(caps.security.device_identity_type == HAL_CAPABILITY_REQUIRED);
    assert(caps.security.secure_storage == HAL_CAPABILITY_SUPPORTED);
    assert(caps.media.video_capture == HAL_CAPABILITY_REQUIRED);
    assert(caps.media.hardware_encoder == HAL_CAPABILITY_REQUIRED);
    assert(caps.media.monotonic_clock == HAL_CAPABILITY_REQUIRED);
    assert(caps.network.tls_trust_store == HAL_CAPABILITY_REQUIRED);

    assert(caps.media.video_input_count == 1);
    assert(caps.media.video_inputs[0].max_width == 1920);
    assert(caps.media.codec_count == 2);
    assert(caps.media.codecs[0].codec == HAL_VIDEO_CODEC_H264);
    assert(caps.network.net_interface_count == 1);

    hal_platform_info_t info;
    status = hal_get_platform_info(&info);
    assert(status == HAL_OK);
    assert(strcmp(info.manufacturer, "MockCorp") == 0);
    printf("Platform Info: Manufacturer=%s, Product=%s, SoC=%s\n",
           info.manufacturer, info.product_name, info.soc_name);

    hal_device_identity_t identity;
    status = hal_get_device_identity(&identity);
    assert(status == HAL_OK);
    assert(strlen(identity.serial_number) > 0);
    printf("Device Identity: SN=%s, UUID=%s\n", identity.serial_number, identity.device_uuid);

    hal_shutdown();
    printf("test_hal_capability PASSED!\n");
    return 0;
}
