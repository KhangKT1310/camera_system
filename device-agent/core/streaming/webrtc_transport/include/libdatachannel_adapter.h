#ifndef CAMERA_SYSTEM_LIBDATACHANNEL_ADAPTER_H
#define CAMERA_SYSTEM_LIBDATACHANNEL_ADAPTER_H

#include "webrtc_transport.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ldc_backend ldc_backend_t;

typedef struct {
    void (*on_local_description)(void *user, const char *type, const char *sdp);
    void (*on_local_candidate)(void *user, const char *candidate, const char *mid);
    void (*on_state_changed)(void *user, webrtc_state_t state);
    void (*on_keyframe_requested)(void *user);
} ldc_callbacks_t;

/**
 * @brief Creates a libdatachannel backend instance.
 * @param config Transport settings (STUN/TURN, SSRC, PT).
 * @param callbacks Async event callbacks.
 * @param user Opaque pointer passed to callbacks.
 * @param out Pointer to receive created handle.
 * @return 0 on success, negative error code on failure.
 */
int ldc_backend_create(const webrtc_transport_config_t *config,
                       const ldc_callbacks_t *callbacks,
                       void *user,
                       ldc_backend_t **out);

/**
 * @brief Triggers SDP Offer generation on libdatachannel peer connection.
 * @param backend Backend handle.
 * @return 0 on success, negative error code on failure.
 */
int ldc_backend_create_offer(ldc_backend_t *backend);

/**
 * @brief Applies remote SDP Answer or Offer to libdatachannel peer connection.
 * @param backend Backend handle.
 * @param type SDP type ("offer" or "answer").
 * @param sdp SDP string payload.
 * @return 0 on success, negative error code on failure.
 */
int ldc_backend_set_remote_description(ldc_backend_t *backend, const char *type, const char *sdp);

/**
 * @brief Adds a remote ICE candidate to libdatachannel peer connection.
 * @param backend Backend handle.
 * @param candidate ICE candidate string.
 * @param mid Media stream identification string (e.g. "video").
 * @return 0 on success, negative error code on failure.
 */
int ldc_backend_add_candidate(ldc_backend_t *backend, const char *candidate, const char *mid);

/**
 * @brief Transmits a raw RTP packet over DTLS-SRTP using rtcSendMessage.
 * @param backend Backend handle.
 * @param packet Pointer to RTP packet bytes.
 * @param size Size of packet bytes.
 * @return 0 on success, negative error code on failure.
 */
int ldc_backend_send_rtp(ldc_backend_t *backend, const uint8_t *packet, size_t size);

/**
 * @brief Destroys a libdatachannel backend instance with shutdown barrier.
 * @param backend Backend handle.
 */
void ldc_backend_destroy(ldc_backend_t *backend);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_SYSTEM_LIBDATACHANNEL_ADAPTER_H */
