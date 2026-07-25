/**
 * @file rtp_packetizer.h
 * @brief H.264 RTP Packetizer Subsystem Interface for WebRTC Media Streaming.
 */

#ifndef CAMERA_SYSTEM_RTP_PACKETIZER_H
#define CAMERA_SYSTEM_RTP_PACKETIZER_H

#include "media_frame.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaque handle representing an H.264 RTP packetizer instance.
 */
typedef struct rtp_packetizer rtp_packetizer_t;

/**
 * @brief Callback function invoked when an RTP packet is constructed and ready for transmission.
 * 
 * @param[in] packet_data Pointer to output RTP packet bytes. Must not be modified.
 * @param[in] packet_size Size of RTP packet in bytes (including 12-byte RTP header).
 * @param[in] user_data Opaque user pointer passed during packetizer creation.
 */
typedef void (*rtp_packet_cb)(const uint8_t *packet_data, size_t packet_size, void *user_data);

/**
 * @brief Configuration structure for initializing the RTP packetizer.
 */
typedef struct {
    uint32_t ssrc;         /**< Synchronization Source identifier (RTP header field). */
    uint8_t payload_type;  /**< Dynamic RTP payload type (e.g. 96 for H.264). */
    uint16_t mtu;          /**< Maximum Transmission Unit size in bytes (e.g. 1200). */
} rtp_packetizer_config_t;

/**
 * @brief Creates a new H.264 RTP packetizer instance.
 * 
 * @param[in] config Packetizer parameters. Must not be NULL.
 * @param[in] callback Callback invoked when packet bytes are generated. Must not be NULL.
 * @param[in] user_data Opaque pointer passed to callback.
 * @param[out] out_packetizer Pointer to receive created handle. Must not be NULL.
 * 
 * @return 0 on success, or a negative error code on failure:
 *         - -1: Invalid input parameter or NULL pointer.
 *         - -2: Memory allocation failure.
 */
int rtp_packetizer_create(const rtp_packetizer_config_t *config,
                          rtp_packet_cb callback,
                          void *user_data,
                          rtp_packetizer_t **out_packetizer);

/**
 * @brief Destroys an RTP packetizer instance and frees allocated memory.
 * 
 * @param[in] packetizer Handle to packetizer. If NULL, operation is a no-op.
 */
void rtp_packetizer_destroy(rtp_packetizer_t *packetizer);

/**
 * @brief Processes an incoming media frame, packetizing H.264 NALUs into Single NALU or FU-A RTP packets.
 * 
 * Performs 90kHz timestamp conversion (pts_us * 90 / 1000) and sets RTP marker bits on frame boundaries.
 * 
 * @param[in] packetizer Target RTP packetizer handle. Must not be NULL.
 * @param[in] frame Pointer to media_frame_t containing H.264 payload. Must not be NULL.
 * 
 * @return 0 on success, or negative error code on failure (-1 if invalid or not H.264).
 */
int rtp_packetizer_push_frame(rtp_packetizer_t *packetizer, const media_frame_t *frame);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_SYSTEM_RTP_PACKETIZER_H */
