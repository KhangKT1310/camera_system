#ifndef CAMERA_SYSTEM_RTP_PACKETIZER_H
#define CAMERA_SYSTEM_RTP_PACKETIZER_H

#include "media_frame.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rtp_packetizer rtp_packetizer_t;

typedef void (*rtp_packet_cb)(const uint8_t *packet_data, size_t packet_size, void *user_data);

typedef struct {
    uint32_t ssrc;
    uint8_t payload_type;
    uint16_t mtu;
} rtp_packetizer_config_t;

/**
 * @brief Creates an RTP packetizer instance.
 * @param config Packetizer parameters.
 * @param callback Callback invoked when an RTP packet is produced.
 * @param user_data Opaque pointer passed to callback.
 * @param out_packetizer Pointer to receive created handle.
 * @return 0 on success, negative error code on failure.
 */
int rtp_packetizer_create(const rtp_packetizer_config_t *config,
                          rtp_packet_cb callback,
                          void *user_data,
                          rtp_packetizer_t **out_packetizer);

/**
 * @brief Destroys an RTP packetizer instance.
 * @param packetizer Handle to packetizer.
 */
void rtp_packetizer_destroy(rtp_packetizer_t *packetizer);

/**
 * @brief Packetizes an H.264 media frame into RTP packets (Single NALU / FU-A).
 * Converts pts_us to 90 kHz RTP clock timestamp.
 * @param packetizer Handle to packetizer.
 * @param frame Pointer to media_frame_t containing Annex-B formatted H.264 NALUs.
 * @return 0 on success, negative error code on failure.
 */
int rtp_packetizer_push_frame(rtp_packetizer_t *packetizer, const media_frame_t *frame);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_SYSTEM_RTP_PACKETIZER_H */
