#ifndef CAMERA_SYSTEM_LIVESTREAM_SESSION_CONTROLLER_H
#define CAMERA_SYSTEM_LIVESTREAM_SESSION_CONTROLLER_H

#include "media_pipeline.h"
#include "webrtc_transport.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct livestream_session_controller livestream_session_controller_t;

typedef struct {
    const char *session_id;
    media_pipeline_t *media_pipeline;
    webrtc_transport_config_t transport_config;
} livestream_session_config_t;

/**
 * @brief Creates a livestream session controller.
 * @param config Session parameters.
 * @param transport_callbacks Callbacks for transport signaling events.
 * @param user_data Opaque user pointer.
 * @param out_controller Pointer to receive created controller handle.
 * @return 0 on success, negative error code on failure.
 */
int livestream_session_controller_create(const livestream_session_config_t *config,
                                          const webrtc_transport_callbacks_t *transport_callbacks,
                                          void *user_data,
                                          livestream_session_controller_t **out_controller);

/**
 * @brief Destroys a livestream session controller and stops streaming.
 * @param controller Controller handle.
 */
void livestream_session_controller_destroy(livestream_session_controller_t *controller);

/**
 * @brief Starts the livestream session (initiates WebRTC offer negotiation).
 * @param controller Controller handle.
 * @return 0 on success, negative error code on failure.
 */
int livestream_session_controller_start(livestream_session_controller_t *controller);

/**
 * @brief Applies remote SDP Answer from signaling.
 * @param controller Controller handle.
 * @param sdp SDP answer string.
 * @return 0 on success, negative error code on failure.
 */
int livestream_session_controller_apply_answer(livestream_session_controller_t *controller, const char *sdp);

/**
 * @brief Adds a remote ICE candidate received from signaling.
 * @param controller Controller handle.
 * @param candidate Candidate string.
 * @param mid Media stream ID string.
 * @return 0 on success, negative error code on failure.
 */
int livestream_session_controller_add_ice_candidate(livestream_session_controller_t *controller, const char *candidate, const char *mid);

/**
 * @brief Stops the livestream session.
 * @param controller Controller handle.
 * @return 0 on success, negative error code on failure.
 */
int livestream_session_controller_stop(livestream_session_controller_t *controller);

/**
 * @brief Gets current transport state of the livestream session.
 * @param controller Controller handle.
 * @return Current webrtc_state_t.
 */
webrtc_state_t livestream_session_controller_get_state(livestream_session_controller_t *controller);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_SYSTEM_LIVESTREAM_SESSION_CONTROLLER_H */
