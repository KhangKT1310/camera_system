/**
 * @file main.c
 * @brief Main Entry Point for Standalone Camera Agent Daemon Executable.
 * 
 * Parses CLI parameters, handles SIGINT/SIGTERM signals, and manages daemon execution.
 */

#include "camera_agent.h"
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static camera_agent_t *g_agent = NULL;
static volatile sig_atomic_t g_running = 1;

/**
 * @brief Signal handler for SIGINT and SIGTERM.
 * 
 * Initiates clean daemon shutdown when interrupt signals are received.
 * 
 * @param[in] sig Received signal number.
 */
static void handle_signal(int sig) {
    (void)sig;
    g_running = 0;
}

/**
 * @brief Prints CLI usage help.
 * @param[in] prog Program name string.
 */
static void print_usage(const char *prog) {
    printf("Usage: %s [options]\n", prog);
    printf("Options:\n");
    printf("  --server URI    WebSocket Signaling Server URI (default: ws://localhost:8080/ws?role=camera&session_id=stream1)\n");
    printf("  --session ID    Session identifier (default: stream1)\n");
    printf("  --device ID     Camera HAL device ID (default: 0)\n");
    printf("  --width PX      Video width in pixels (default: 1920)\n");
    printf("  --height PX     Video height in pixels (default: 1080)\n");
    printf("  --fps N         Video framerate (default: 30)\n");
    printf("  --mock-hal      Use mock camera HAL capture instead of physical hardware\n");
    printf("  --webrtc        Enable real libdatachannel backend for WebRTC media\n");
    printf("  --help          Display this help message\n");
}

int main(int argc, char **argv) {
    printf("====================================================\n");
    printf("  Camera System - WebRTC Camera Agent Daemon v1.0   \n");
    printf("====================================================\n");

    camera_agent_config_t config = {
        .signaling_server_uri = "ws://localhost:8080/ws?role=camera&session_id=stream1",
        .session_id = "stream1",
        .camera_device_id = 0,
        .width = 1920,
        .height = 1080,
        .fps = 30,
        .ssrc = 0x12345678,
        .use_mock_hal = true,
        .enable_libdatachannel = false
    };

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--server") == 0 && i + 1 < argc) {
            config.signaling_server_uri = argv[++i];
        } else if (strcmp(argv[i], "--session") == 0 && i + 1 < argc) {
            config.session_id = argv[++i];
        } else if (strcmp(argv[i], "--device") == 0 && i + 1 < argc) {
            config.camera_device_id = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--width") == 0 && i + 1 < argc) {
            config.width = (uint32_t)atoi(argv[++i]);
        } else if (strcmp(argv[i], "--height") == 0 && i + 1 < argc) {
            config.height = (uint32_t)atoi(argv[++i]);
        } else if (strcmp(argv[i], "--fps") == 0 && i + 1 < argc) {
            config.fps = (uint32_t)atoi(argv[++i]);
        } else if (strcmp(argv[i], "--mock-hal") == 0) {
            config.use_mock_hal = true;
        } else if (strcmp(argv[i], "--webrtc") == 0) {
            config.enable_libdatachannel = true;
        } else if (strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        }
    }

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    int ret = camera_agent_create(&config, &g_agent);
    if (ret != 0) {
        printf("[Main] Error creating camera agent: %d\n", ret);
        return 1;
    }

    ret = camera_agent_start(g_agent);
    if (ret != 0) {
        printf("[Main] Error starting camera agent: %d\n", ret);
        camera_agent_destroy(g_agent);
        return 1;
    }

    printf("[Main] Camera Agent daemon is running. Press Ctrl+C to terminate.\n");

    /* Daemon execution loop */
    while (g_running) {
        usleep(100000); // 100ms sleep
    }

    printf("\n[Main] Received shutdown signal, terminating daemon cleanly...\n");
    camera_agent_destroy(g_agent);
    printf("[Main] Daemon terminated cleanly.\n");
    return 0;
}
