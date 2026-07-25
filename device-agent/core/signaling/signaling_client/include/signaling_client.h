/**
 * @file signaling_client.h
 * @brief Signaling Client Engine Interface for WebRTC JSON Envelopes.
 */

#ifndef CAMERA_SYSTEM_SIGNALING_CLIENT_H
#define CAMERA_SYSTEM_SIGNALING_CLIENT_H

#include "signaling_transport.h"
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaque handle representing a signaling client instance.
 */
typedef struct signaling_client signaling_client_t;

/**
 * @brief Callback function signature invoked when valid signaling messages are received.
 * 
 * @param[in] type Message type string ("OFFER", "ANSWER", "CANDIDATE", "PING", etc.).
 * @param[in] session_id Session ID string matching envelope session.
 * @param[in] payload_json Stringified JSON payload object.
 * @param[in] user_data Opaque user pointer passed during client creation.
 */
typedef void (*signaling_message_cb)(const char *type,
                                    const char *session_id,
                                    const char *payload_json,
                                    void *user_data);

/**
 * @brief Configuration structure for initializing a signaling client instance.
 */
typedef struct {
    signaling_transport_ops_t transport_ops; /**< Transport Vtable function pointers. */
    void *transport_ctx;                     /**< Opaque transport context pointer. */
    const char *server_uri;                  /**< Target server URI string. */
} signaling_client_config_t;

/**
 * @brief Creates a new signaling client instance.
 * 
 * @param[in] config Client configuration parameters. Must not be NULL.
 * @param[in] callback Message listener callback function. Must not be NULL.
 * @param[in] user_data Opaque pointer passed to callback.
 * @param[out] out_client Pointer to receive created handle. Must not be NULL.
 * 
 * @return 0 on success, or a negative error code on failure:
 *         - -1: Invalid input parameter or NULL pointer.
 *         - -2: Memory allocation failure.
 */
int signaling_client_create(const signaling_client_config_t *config,
                            signaling_message_cb callback,
                            void *user_data,
                            signaling_client_t **out_client);

/**
 * @brief Destroys a signaling client instance and closes transport socket.
 * 
 * @param[in] client Client handle. If NULL, operation is a no-op.
 */
void signaling_client_destroy(signaling_client_t *client);

/**
 * @brief Connects signaling client to the remote signaling server via transport vtable.
 * 
 * @param[in] client Client handle. Must not be NULL.
 * 
 * @return 0 on success, or negative error code on failure.
 */
int signaling_client_connect(signaling_client_t *client);

/**
 * @brief Formats and transmits a JSON envelope message with input validation.
 * 
 * Envelope schema:
 * { "version": 1, "type": "...", "session_id": "...", "request_id": "...", "timestamp": ..., "payload": ... }
 * 
 * @param[in] client Client handle. Must not be NULL.
 * @param[in] type Message type string ("OFFER", "ANSWER", "CANDIDATE", "PING", etc.). Must not be NULL.
 * @param[in] session_id Session ID string. Must not be NULL.
 * @param[in] payload_json JSON string payload. Must not be NULL.
 * 
 * @return 0 on success, or negative error code on failure (-1 if input bounds violated).
 */
int signaling_client_send_message(signaling_client_t *client,
                                  const char *type,
                                  const char *session_id,
                                  const char *payload_json);

/**
 * @brief Parses an incoming raw JSON envelope string with AST parser and bounds checking.
 * 
 * Validates payload structure and triggers registered signaling_message_cb callback.
 * 
 * @param[in] client Client handle. Must not be NULL.
 * @param[in] raw_json Raw JSON envelope string. Must not be NULL.
 * 
 * @return 0 on success, or negative error code on failure (-1 if malformed or oversized).
 */
int signaling_client_receive_raw(signaling_client_t *client, const char *raw_json);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_SYSTEM_SIGNALING_CLIENT_H */
