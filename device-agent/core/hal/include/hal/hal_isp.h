#ifndef CAMERA_HAL_ISP_H
#define CAMERA_HAL_ISP_H

#include "hal_camera.h"
#include "hal_error.h"
#include "hal_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t brightness;
    int32_t contrast;
    int32_t saturation;
    int32_t sharpness;
    int32_t hue;
    bool mirror;
    bool flip;
    uint32_t rotation_degrees;
} hal_image_controls_t;

typedef enum {
    HAL_EXPOSURE_AUTO = 0,
    HAL_EXPOSURE_MANUAL,
    HAL_EXPOSURE_SHUTTER_PRIORITY,
    HAL_EXPOSURE_GAIN_PRIORITY
} hal_exposure_mode_t;

typedef enum {
    HAL_ANTI_FLICKER_OFF = 0,
    HAL_ANTI_FLICKER_50HZ,
    HAL_ANTI_FLICKER_60HZ,
    HAL_ANTI_FLICKER_AUTO
} hal_anti_flicker_t;

typedef enum {
    HAL_WHITE_BALANCE_AUTO = 0,
    HAL_WHITE_BALANCE_MANUAL,
    HAL_WHITE_BALANCE_INDOOR,
    HAL_WHITE_BALANCE_OUTDOOR,
    HAL_WHITE_BALANCE_DAYLIGHT,
    HAL_WHITE_BALANCE_CLOUDY
} hal_white_balance_mode_t;

typedef enum {
    HAL_DAYNIGHT_DAY = 0,
    HAL_DAYNIGHT_NIGHT,
    HAL_DAYNIGHT_AUTO,
    HAL_DAYNIGHT_SCHEDULE
} hal_daynight_mode_t;

typedef enum {
    HAL_IR_CUT_DAY_POSITION = 0,
    HAL_IR_CUT_NIGHT_POSITION,
    HAL_IR_CUT_TRANSITIONING,
    HAL_IR_CUT_FAULT
} hal_ir_cut_state_t;

hal_status_t hal_get_image_controls(hal_video_input_id_t input, hal_image_controls_t *controls);
hal_status_t hal_set_image_controls(hal_video_input_id_t input, const hal_image_controls_t *controls);

hal_status_t hal_set_exposure_mode(hal_video_input_id_t input, hal_exposure_mode_t mode);
hal_status_t hal_set_manual_exposure(hal_video_input_id_t input, uint32_t exposure_us, float analog_gain, float digital_gain);
hal_status_t hal_set_anti_flicker(hal_video_input_id_t input, hal_anti_flicker_t mode);

hal_status_t hal_set_white_balance_mode(hal_video_input_id_t input, hal_white_balance_mode_t mode);
hal_status_t hal_set_manual_white_balance(hal_video_input_id_t input, float red_gain, float blue_gain);

hal_status_t hal_set_daynight_mode(hal_video_input_id_t input, hal_daynight_mode_t mode);
hal_status_t hal_set_ir_cut_state(hal_ir_cut_state_t state);
hal_status_t hal_get_ir_cut_state(hal_ir_cut_state_t *state);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_HAL_ISP_H */
