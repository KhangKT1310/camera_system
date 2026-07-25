/**
 * @file camera_agent.h
 * @brief Standalone Camera Agent Daemon Core Interface for WebRTC Surveillance System.
 * 
 * Provides configuration parameters, lifecycle controls, and pipeline orchestration
 * connecting Camera HAL Capture, Media Pipeline, H.264 RTP Packetizer, WebRTC Transport,
 * and POSIX WebSocket Signaling Client.
 */

#ifndef CAMERA_SYSTEM_CAMERA_AGENT_H
#define CAMERA_SYSTEM_CAMERA_AGENT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaque handle representing a running Camera Agent daemon instance.
 */
typedef struct camera_agent camera_agent_t;

/**
 * @brief Configuration parameters required to initialize the Camera Agent daemon.
 */
typedef struct {
    /**
     * @brief Target WebSocket Signaling Server URI (e.g. "ws://localhost:8080/ws").
     * Must not be NULL.
     */
    const char *signaling_server_uri;

    /**
     * @brief Unique session identifier string matching remote viewers (e.g. "stream1").
     * Must not be NULL.
     */
    const char *session_id;

    /**
     * @brief Target camera device index or HAL identifier (e.g. 0 for /dev/video0).
     */
    int camera_device_id;

    /**
     * @brief Target video frame width in pixels (e.g. 1920).
     */
    uint32_t width;

    /**
     * @brief Target video frame height in pixels (e.g. 1080).
     */
    uint32_t height;

    /**
     * @brief Target video framerate in frames-per-second (e.g. 30).
     */
    uint32_t fps;

    /**
     * @brief RTP stream synchronization SSRC identifier (e.g. 0x12345678).
     */
    uint32_t ssrc;

    /**
     * @brief Flag indicating whether to use mock HAL capture (true) or physical camera HAL (false).
     */
    bool use_mock_hal;

    /**
     * @brief Flag indicating whether to enable real libdatachannel backend (true) or mock transport (false).
     */
    bool enable_libdatachannel;
} camera_agent_config_t;

/**
 * @brief Creates and initializes a Camera Agent daemon instance.
 * 
 * Allocates internal session structures, configures HAL camera capture,
 * initializes the zero-copy media pipeline, H.264 RTP packetizer, WebRTC transport,
 * and sets up the WebSocket signaling client vtable.
 * 
 * @param[in] config Pointer to camera agent configuration structure. Must not be NULL.
 * @param[out] out_agent Pointer to receive created camera agent handle pointer. Must not be NULL.
 * 
 * @return 0 on success, or a negative error code on failure:
 *         - -1: Invalid input parameter or NULL pointer passed.
 *         - -2: Memory allocation failure.
 *         - -3: Failed to initialize HAL camera capture device.
 *         - -4: Failed to initialize WebRTC transport or signaling client.
 * 
 * @note Thread-Safety: Must be invoked once from the main startup thread before starting the loop.
 */
int camera_agent_create(const camera_agent_config_t *config, camera_agent_t **out_agent);

/**
 * @brief Starts the Camera Agent daemon processing loop.
 * 
 * Connects the WebSocket signaling client to the remote signaling server,
 * initiates camera frame capture, and begins background frame distribution.
 * 
 * @param[in] agent Valid non-NULL handle returned by camera_agent_create().
 * 
 * @return 0 on success, or a negative error code on failure:
 *         - -1: Invalid input handle.
 *         - -2: Agent is already running or in an invalid state.
 *         - -3: WebSocket connection failed.
 * 
 * @note Asynchronous Operation: Frame capture and signaling run on background worker threads.
 */
int camera_agent_start(camera_agent_t *agent);

/**
 * @brief Stops frame capture and terminates active WebRTC and signaling sessions.
 * 
 * Signals worker loops to terminate, closes active WebRTC peer connections,
 * and disconnects WebSocket signaling transport cleanly.
 * 
 * @param[in] agent Valid non-NULL handle returned by camera_agent_create().
 * 
 * @return 0 on success, or a negative error code on failure:
 *         - -1: Invalid input handle.
 * 
 * @note Idempotent: Calling camera_agent_stop() on an already stopped agent is safe and returns 0.
 */
int camera_agent_stop(camera_agent_t *agent);

/**
 * @brief Destroys a Camera Agent instance and frees all allocated resources.
 * 
 * Stops the agent if running, destroys HAL capture handles, media pipeline,
 * RTP packetizers, WebRTC transport, signaling client, and frees allocated memory.
 * 
 * @param[in] agent Valid handle to destroy. Passing NULL is safely ignored.
 * 
 * @note Memory Ownership: The agent handle pointer becomes invalid after this function returns.
 */
void camera_agent_destroy(camera_agent_t *agent);

#ifdef __cplusplus
}
#endif

#endif /* CAMERA_SYSTEM_CAMERA_AGENT_H */
