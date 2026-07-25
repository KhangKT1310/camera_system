#ifndef CAMERA_SYSTEM_WEBRTC_TRANSPORT_H
#define CAMERA_SYSTEM_WEBRTC_TRANSPORT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WEBRTC_MAX_PENDING_CANDIDATES 32
#define WEBRTC_MAX_CANDIDATE_LENGTH 2048

typedef enum {
    WEBRTC_STATE_IDLE = 0,
    WEBRTC_STATE_CREATING = 1,
    WEBRTC_STATE_NEGOTIATING = 2,
    WEBRTC_STATE_CONNECTING = 3,
    WEBRTC_STATE_CONNECTED = 4,
    WEBRTC_STATE_DISCONNECTED = 5,
    WEBRTC_STATE_FAILED = 6,
    WEBRTC_STATE_CLOSING = 7,
    WEBRTC_STATE_CLOSED = 8
} webrtc_state_t;

typedef struct {
    void (*on_local_description)(void *user, const char *type, const char *sdp);
    void (*on_local_candidate)(void *user, const char *candidate, const char *mid);
    void (*on_state_changed)(void *user, webrtc_state_t state);
} webrtc_transport_callbacks_t;

typedef struct {
    const char *stun_server;
    const char *turn_server;
    const char *turn_username;
    const char *turn_credential;
    bool enable_libdatachannel;
} webrtc_transport_config_t;

typedef struct webrtc_transport webrtc_transport_t;

/**
 * @brief Creates a WebRTC transport instance.
 * @param config Configuration parameters.
 * @param callbacks Async notification callbacks.
 * @param user_data Opaque pointer passed to callbacks.
 * @param out_transport Pointer to receive created handle.
 * @return 0 on success, negative error code on failure.
 */
int webrtc_transport_create(const webrtc_transport_config_t *config,
                           const webrtc_transport_callbacks_t *callbacks,
                           void *user_data,
                           webrtc_transport_t **out_transport);

/**
 * @brief Retains a reference to a WebRTC transport instance.
 * @param transport Handle to transport.
 * @return Retained transport pointer.
 */
webrtc_transport_t *webrtc_transport_retain(webrtc_transport_t *transport);

/**
 * @brief Releases a reference from a WebRTC transport instance. Frees memory when refcount hits 0.
 * @param transport Handle to transport.
 */
void webrtc_transport_release(webrtc_transport_t *transport);

/**
 * @brief Destroys a WebRTC transport instance (marks CLOSED and releases creation reference).
 * @param transport Handle to transport.
 */
void webrtc_transport_destroy(webrtc_transport_t *transport);

/**
 * @brief Triggers SDP Offer generation.
 * @param transport Handle to transport.
 * @return 0 on success, negative error code on failure.
 */
int webrtc_transport_create_offer(webrtc_transport_t *transport);

/**
 * @brief Sets remote SDP (Offer or Answer). Flushes pending candidates if remote description is set.
 * @param transport Handle to transport.
 * @param type SDP type ("offer" or "answer").
 * @param sdp SDP string payload.
 * @return 0 on success, negative error code on failure.
 */
int webrtc_transport_set_remote_description(webrtc_transport_t *transport, const char *type, const char *sdp);

/**
 * @brief Adds a remote ICE candidate. Queues candidate if remote SDP description is not yet set.
 * @param transport Handle to transport.
 * @param candidate ICE candidate string.
 * @param mid Media stream identification string (e.g. "video" or "0").
 * @return 0 on success, negative error code on failure.
 */
int webrtc_transport_add_remote_candidate(webrtc_transport_t *transport, const char *candidate, const char *mid);

/**
 * @brief Transmits an RTP packet over the WebRTC media track via DTLS-SRTP.
 * @param transport Handle to transport.
 * @param packet Pointer to RTP packet bytes.
 * @param size Size of packet bytes.
 * @return 0 on success, negative error code on failure.
 */
int webrtc_transport_send_rtp(webrtc_transport_t *transport, const uint8_t *packet, size_t size);

/**
 * @brief Gets current WebRTC transport state.
 * @param transport Handle to transport.
 * @return Current webrtc_state_t.
 */
webrtc_state_t webrtc_transport_get_state(webrtc_transport_t *transport);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_SYSTEM_WEBRTC_TRANSPORT_H */
