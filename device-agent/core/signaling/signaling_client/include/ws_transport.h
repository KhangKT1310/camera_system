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
 * @brief Creates a POSIX C17 WebSocket transport context for dev/production signaling.
 * @param uri Server WebSocket URI (e.g. "ws://localhost:8080/ws?role=camera&session_id=stream1").
 * @param out_ctx Pointer to receive created transport context handle.
 * @return 0 on success, negative error code on failure.
 */
int ws_transport_create(const char *uri, ws_transport_ctx_t **out_ctx);

/**
 * @brief Destroys a WebSocket transport context handle.
 * @param ctx Context handle.
 */
void ws_transport_destroy(ws_transport_ctx_t *ctx);

/**
 * @brief Returns the signaling_transport_ops_t vtable for WebSocket transport.
 * @return signaling_transport_ops_t struct containing function pointers.
 */
signaling_transport_ops_t ws_transport_get_ops(void);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_SYSTEM_WS_TRANSPORT_H */
