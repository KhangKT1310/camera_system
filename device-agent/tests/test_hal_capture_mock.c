#include <assert.h>
#include <stdio.h>
#include "hal/hal.h"

int main(void) {
    printf("Running test_hal_capture_mock...\n");

    hal_status_t status = hal_init(NULL);
    assert(status == HAL_OK);

    hal_video_input_info_t inputs[4];
    size_t count = 0;
    status = hal_list_video_inputs(inputs, &count);
    assert(status == HAL_OK);
    assert(count == 1);
    printf("Found %zu video input: %s\n", count, inputs[0].name);

    hal_video_mode_t modes[4];
    size_t mode_count = 0;
    status = hal_list_video_modes(inputs[0].input_id, modes, &mode_count);
    assert(status == HAL_OK);
    assert(mode_count >= 1);
    printf("Video Mode: %dx%d @ %d fps\n", modes[0].width, modes[0].height, modes[0].frame_rate.numerator);

    hal_video_stream_t stream = 0;
    status = hal_open_video_stream(inputs[0].input_id, &modes[0], &stream);
    assert(status == HAL_OK);
    assert(stream != 0);

    status = hal_start_video_stream(stream);
    assert(status == HAL_OK);

    hal_video_frame_t frame;
    status = hal_read_video_frame(stream, 100, &frame);
    assert(status == HAL_OK);
    assert(frame.sequence == 1);
    assert(frame.width == 1920);
    assert(frame.height == 1080);
    printf("Captured Frame #%llu: %dx%d (timestamp: %llu ns)\n",
           (unsigned long long)frame.sequence, frame.width, frame.height,
           (unsigned long long)frame.capture_timestamp.value_ns);

    status = hal_stop_video_stream(stream);
    assert(status == HAL_OK);

    status = hal_close_video_stream(stream);
    assert(status == HAL_OK);

    hal_shutdown();
    printf("test_hal_capture_mock PASSED!\n");
    return 0;
}
