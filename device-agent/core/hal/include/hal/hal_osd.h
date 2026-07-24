#ifndef CAMERA_HAL_OSD_H
#define CAMERA_HAL_OSD_H

#include "hal_camera.h"
#include "hal_error.h"
#include "hal_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HAL_OVERLAY_TYPE_TEXT = 0,
    HAL_OVERLAY_TYPE_BITMAP,
    HAL_OVERLAY_TYPE_RECTANGLE,
    HAL_OVERLAY_TYPE_PRIVACY_MASK,
    HAL_OVERLAY_TYPE_TIMESTAMP
} hal_overlay_type_t;

typedef struct {
    hal_overlay_type_t type;
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
    uint32_t color_rgba;
} hal_overlay_config_t;

typedef struct {
    char text[128];
    const uint8_t *bitmap_data;
    size_t bitmap_size;
} hal_overlay_content_t;

hal_status_t hal_create_overlay(const hal_overlay_config_t *config, hal_overlay_t *overlay);
hal_status_t hal_update_overlay(hal_overlay_t overlay, const hal_overlay_content_t *content);
hal_status_t hal_attach_overlay(hal_overlay_t overlay, hal_video_stream_t stream);
hal_status_t hal_set_overlay_visibility(hal_overlay_t overlay, bool visible);
hal_status_t hal_destroy_overlay(hal_overlay_t overlay);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_HAL_OSD_H */
