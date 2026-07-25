#ifndef CAMERA_SYSTEM_SIGNALING_CLIENT_H
#define CAMERA_SYSTEM_SIGNALING_CLIENT_H

#include "signaling_transport.h"
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct signaling_client signaling_client_t;

typedef void (*signaling_message_cb)(const char *type,
                                    const char *session_id,
                                    const char *payload_json,
                                    void *user_data);

typedef struct {
    signaling_transport_ops_t transport_ops;
    void *transport_ctx;
    const char *server_uri;
} signaling_client_config_t;

/**
 * @brief Creates a signaling client instance.
 * @param config Client configuration.
 * @param callback Callback when valid signaling messages are received.
 * @param user_data Opaque pointer passed to callback.
 * @param out_client Pointer to receive created handle.
 * @return 0 on success, negative error code on failure.
 */
int signaling_client_create(const signaling_client_config_t *config,
                            signaling_message_cb callback,
                            void *user_data,
                            signaling_client_t **out_client);

/**
 * @brief Destroys a signaling client instance.
 * @param client Client handle.
 */
void signaling_client_destroy(signaling_client_t *client);

/**
 * @brief Connects signaling client to the remote server.
 * @param client Client handle.
 * @return 0 on success, negative error code on failure.
 */
int signaling_client_connect(signaling_client_t *client);

/**
 * @brief Formats and sends a JSON envelope message with input validation.
 * @param client Client handle.
 * @param type Message type string ("OFFER", "ANSWER", "CANDIDATE", "PING", etc.).
 * @param session_id Session ID string.
 * @param payload_json JSON string payload.
 * @return 0 on success, negative error code on failure.
 */
int signaling_client_send_message(signaling_client_t *client,
                                  const char *type,
                                  const char *session_id,
                                  const char *payload_json);

/**
 * @brief Parses an incoming raw JSON envelope string with input bounds checking.
 * @param client Client handle.
 * @param raw_json Raw JSON envelope string.
 * @return 0 on success, negative error code on failure.
 */
int signaling_client_receive_raw(signaling_client_t *client, const char *raw_json);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_SYSTEM_SIGNALING_CLIENT_H */
