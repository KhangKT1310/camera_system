/**
 * @file livestream_session_controller.h
 * @brief Livestream Session Controller Subsystem Interface for WebRTC Session Negotiation.
 */

#ifndef CAMERA_SYSTEM_LIVESTREAM_SESSION_CONTROLLER_H
#define CAMERA_SYSTEM_LIVESTREAM_SESSION_CONTROLLER_H

#include "media_pipeline.h"
#include "webrtc_transport.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaque handle representing a livestream session controller instance.
 */
typedef struct livestream_session_controller livestream_session_controller_t;

/**
 * @brief Configuration structure for initializing a livestream session controller.
 */
typedef struct {
    const char *session_id;                    /**< Unique session identifier string (e.g. "stream1"). */
    media_pipeline_t *media_pipeline;          /**< Pointer to active media pipeline instance. */
    webrtc_transport_config_t transport_config;/**< WebRTC transport configuration parameters. */
} livestream_session_config_t;

/**
 * @brief Creates a new livestream session controller instance.
 * 
 * @param[in] config Session parameters. Must not be NULL.
 * @param[in] transport_callbacks WebRTC transport notification callbacks. Must not be NULL.
 * @param[in] user_data Opaque pointer passed to callbacks.
 * @param[out] out_controller Pointer to receive created controller handle. Must not be NULL.
 * 
 * @return 0 on success, or a negative error code on failure:
 *         - -1: Invalid input parameter or NULL pointer.
 *         - -2: Memory allocation failure.
 *         - -3: Transport initialization failure.
 */
int livestream_session_controller_create(const livestream_session_config_t *config,
                                          const webrtc_transport_callbacks_t *transport_callbacks,
                                          void *user_data,
                                          livestream_session_controller_t **out_controller);

/**
 * @brief Destroys a livestream session controller instance and closes active WebRTC connections.
 * 
 * @param[in] controller Controller handle. If NULL, operation is a no-op.
 */
void livestream_session_controller_destroy(livestream_session_controller_t *controller);

/**
 * @brief Starts the livestream session by creating a WebRTC SDP offer negotiation.
 * 
 * @param[in] controller Controller handle. Must not be NULL.
 * 
 * @return 0 on success, or negative error code on failure.
 */
int livestream_session_controller_start(livestream_session_controller_t *controller);

/**
 * @brief Processes incoming signaling messages (OFFER, ANSWER, CANDIDATE) for the session controller.
 * 
 * @param[in] controller Controller handle. Must not be NULL.
 * @param[in] type Signaling message type string ("OFFER", "ANSWER", "CANDIDATE").
 * @param[in] session_id Session ID string matching target session.
 * @param[in] payload_json Stringified JSON payload payload.
 * 
 * @return 0 on success, or negative error code on failure.
 */
int livestream_session_controller_handle_signaling(livestream_session_controller_t *controller,
                                                   const char *type,
                                                   const char *session_id,
                                                   const char *payload_json);

/**
 * @brief Applies remote SDP Answer received from signaling server.
 * 
 * @param[in] controller Controller handle. Must not be NULL.
 * @param[in] sdp Remote SDP Answer string. Must not be NULL.
 * 
 * @return 0 on success, or negative error code on failure.
 */
int livestream_session_controller_apply_answer(livestream_session_controller_t *controller, const char *sdp);

/**
 * @brief Adds a remote ICE candidate received from signaling server.
 * 
 * @param[in] controller Controller handle. Must not be NULL.
 * @param[in] candidate Remote ICE candidate string. Must not be NULL.
 * @param[in] mid Media stream identification string (e.g. "video" or "0").
 * 
 * @return 0 on success, or negative error code on failure.
 */
int livestream_session_controller_add_ice_candidate(livestream_session_controller_t *controller, const char *candidate, const char *mid);

/**
 * @brief Stops the active livestream session and resets transport state.
 * 
 * @param[in] controller Controller handle. Must not be NULL.
 * 
 * @return 0 on success, or negative error code on failure.
 */
int livestream_session_controller_stop(livestream_session_controller_t *controller);

/**
 * @brief Gets current transport state of the livestream session in a thread-safe manner.
 * 
 * @param[in] controller Controller handle.
 * 
 * @return Current webrtc_state_t enum value.
 */
webrtc_state_t livestream_session_controller_get_state(livestream_session_controller_t *controller);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_SYSTEM_LIVESTREAM_SESSION_CONTROLLER_H */
