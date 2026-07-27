/**
 * @file ws_transport.h
 * @brief C17 POSIX WebSocket Client Transport Interface for Signaling.
 */

#ifndef CAMERA_SYSTEM_WS_TRANSPORT_H
#define CAMERA_SYSTEM_WS_TRANSPORT_H

#include "signaling_transport.h"
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ws_transport_ctx ws_transport_ctx_t;

/**
 * @brief Callback function invoked when a raw WebSocket text message frame is received.
 * 
 * @param[in] payload_text Null-terminated text payload string. Must not be modified.
 * @param[in] size Size of payload in bytes.
 * @param[in] user_data Opaque user pointer passed during registration.
 */
typedef void (*ws_transport_read_cb)(const char *payload_text, size_t size, void *user_data);

/**
 * @brief Creates a POSIX C17 WebSocket transport context for dev/production signaling.
 * 
 * @param[in] uri Server WebSocket URI (e.g. "ws://localhost:8080/ws?role=camera&session_id=stream1").
 * @param[out] out_ctx Pointer to receive created transport context handle. Must not be NULL.
 * 
 * @return 0 on success, negative error code on failure.
 */
int ws_transport_create(const char *uri, ws_transport_ctx_t **out_ctx);

/**
 * @brief Sets the callback for receiving incoming WebSocket text messages.
 * 
 * @param[in] ctx Context handle. Must not be NULL.
 * @param[in] callback Message callback function.
 * @param[in] user_data Opaque pointer passed to callback.
 */
void ws_transport_set_read_callback(ws_transport_ctx_t *ctx, ws_transport_read_cb callback, void *user_data);

/**
 * @brief Destroys a WebSocket transport context handle and closes background reader thread.
 * 
 * @param[in] ctx Context handle. If NULL, operation is a no-op.
 */
void ws_transport_destroy(ws_transport_ctx_t *ctx);

/**
 * @brief Returns the signaling_transport_ops_t vtable for WebSocket transport.
 * 
 * @return signaling_transport_ops_t struct containing function pointers.
 */
signaling_transport_ops_t ws_transport_get_ops(void);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_SYSTEM_WS_TRANSPORT_H */
