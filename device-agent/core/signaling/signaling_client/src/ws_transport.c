/**
 * @file ws_transport.c
 * @brief POSIX C17 WebSocket Client Transport Implementation with Background Reader.
 */

#include "ws_transport.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

struct ws_transport_ctx {
    int sockfd;
    char host[128];
    int port;
    char path[256];
    atomic_bool is_connected;

    ws_transport_read_cb read_cb;
    void *read_user_data;

    pthread_t reader_thread;
    atomic_bool reader_running;
};

static int parse_ws_uri(const char *uri, char *host, size_t host_len, int *port, char *path, size_t path_len) {
    if (!uri || !host || !port || !path) return -1;

    const char *p = uri;
    if (strncmp(p, "ws://", 5) == 0) {
        p += 5;
        *port = 80;
    } else if (strncmp(p, "wss://", 6) == 0) {
        p += 6;
        *port = 443;
    }

    const char *slash = strchr(p, '/');
    const char *colon = strchr(p, ':');

    if (colon && (!slash || colon < slash)) {
        size_t hlen = (size_t)(colon - p);
        if (hlen >= host_len) return -1;
        strncpy(host, p, hlen);
        host[hlen] = '\0';
        *port = atoi(colon + 1);
    } else if (slash) {
        size_t hlen = (size_t)(slash - p);
        if (hlen >= host_len) return -1;
        strncpy(host, p, hlen);
        host[hlen] = '\0';
    } else {
        strncpy(host, p, host_len - 1);
        host[host_len - 1] = '\0';
    }

    if (slash) {
        strncpy(path, slash, path_len - 1);
        path[path_len - 1] = '\0';
    } else {
        strncpy(path, "/", path_len - 1);
        path[path_len - 1] = '\0';
    }

    return 0;
}

/**
 * @brief Background reader thread decoding incoming RFC 6455 WebSocket text frames.
 */
static void *ws_reader_worker(void *arg) {
    ws_transport_ctx_t *ctx = (ws_transport_ctx_t *)arg;
    if (!ctx) return NULL;

    uint8_t buffer[65536];

    while (atomic_load(&ctx->reader_running) && atomic_load(&ctx->is_connected)) {
        ssize_t n = read(ctx->sockfd, buffer, sizeof(buffer) - 1);
        if (n <= 0) {
            if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                usleep(10000);
                continue;
            }
            break;
        }

        buffer[n] = '\0';
        uint8_t opcode = buffer[0] & 0x0F;

        if (opcode == 0x1) { /* Text Frame */
            size_t payload_len = buffer[1] & 0x7F;
            size_t offset = 2;
            if (payload_len == 126) {
                payload_len = ((size_t)buffer[2] << 8) | buffer[3];
                offset += 2;
            } else if (payload_len == 127) {
                offset += 8;
            }

            bool masked = (buffer[1] & 0x80) != 0;
            uint8_t masks[4] = {0};
            if (masked) {
                memcpy(masks, &buffer[offset], 4);
                offset += 4;
            }

            if (offset + payload_len <= (size_t)n) {
                char *payload = (char *)malloc(payload_len + 1);
                if (payload) {
                    for (size_t i = 0; i < payload_len; i++) {
                        payload[i] = masked ? (char)(buffer[offset + i] ^ masks[i % 4]) : (char)buffer[offset + i];
                    }
                    payload[payload_len] = '\0';

                    if (ctx->read_cb) {
                        ctx->read_cb(payload, payload_len, ctx->read_user_data);
                    }
                    free(payload);
                }
            }
        }
    }

    return NULL;
}

static int ws_connect_impl(void *ctx_ptr, const char *uri) {
    ws_transport_ctx_t *ctx = (ws_transport_ctx_t *)ctx_ptr;
    if (!ctx) return -1;

    const char *target_uri = uri ? uri : "ws://localhost:8080/ws?role=camera&session_id=stream1";
    if (parse_ws_uri(target_uri, ctx->host, sizeof(ctx->host), &ctx->port, ctx->path, sizeof(ctx->path)) != 0) {
        return -1;
    }

    struct hostent *he = gethostbyname(ctx->host);
    if (!he) return -1;

    ctx->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (ctx->sockfd < 0) return -1;

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons((uint16_t)ctx->port);
    memcpy(&serv_addr.sin_addr.s_addr, he->h_addr_list[0], (size_t)he->h_length);

    if (connect(ctx->sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        close(ctx->sockfd);
        ctx->sockfd = -1;
        return -1;
    }

    /* Send HTTP WebSocket Upgrade request */
    char req[1024];
    int len = snprintf(req, sizeof(req),
                       "GET %s HTTP/1.1\r\n"
                       "Host: %s:%d\r\n"
                       "Upgrade: websocket\r\n"
                       "Connection: Upgrade\r\n"
                       "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
                       "Sec-WebSocket-Version: 13\r\n\r\n",
                       ctx->path, ctx->host, ctx->port);

    if (write(ctx->sockfd, req, (size_t)len) <= 0) {
        close(ctx->sockfd);
        ctx->sockfd = -1;
        return -1;
    }

    /* Read Upgrade response */
    char resp[1024];
    ssize_t rlen = read(ctx->sockfd, resp, sizeof(resp) - 1);
    if (rlen <= 0) {
        close(ctx->sockfd);
        ctx->sockfd = -1;
        return -1;
    }
    resp[rlen] = '\0';

    if (strstr(resp, "101") == NULL) {
        close(ctx->sockfd);
        ctx->sockfd = -1;
        return -1; /* Failed HTTP 101 WebSocket Upgrade */
    }

    atomic_store(&ctx->is_connected, true);
    atomic_store(&ctx->reader_running, true);

    pthread_create(&ctx->reader_thread, NULL, ws_reader_worker, ctx);

    return 0;
}

static int ws_send_impl(void *ctx_ptr, const void *data, size_t size) {
    ws_transport_ctx_t *ctx = (ws_transport_ctx_t *)ctx_ptr;
    if (!ctx || ctx->sockfd < 0 || !atomic_load(&ctx->is_connected) || !data || size == 0) {
        return -1;
    }

    /* Format RFC 6455 masked WebSocket text frame */
    uint8_t frame_hdr[14];
    size_t hdr_len = 0;
    frame_hdr[0] = 0x81; /* FIN = 1, Opcode = 0x1 (Text) */

    if (size <= 125) {
        frame_hdr[1] = 0x80 | (uint8_t)size; /* MASK bit = 1 */
        hdr_len = 2;
    } else if (size <= 65535) {
        frame_hdr[1] = 0x80 | 126;
        frame_hdr[2] = (uint8_t)((size >> 8) & 0xFF);
        frame_hdr[3] = (uint8_t)(size & 0xFF);
        hdr_len = 4;
    } else {
        return -1; /* Payload oversized for frame header */
    }

    uint8_t mask[4] = {0x12, 0x34, 0x56, 0x78};
    memcpy(&frame_hdr[hdr_len], mask, 4);
    hdr_len += 4;

    /* Write frame header */
    if (write(ctx->sockfd, frame_hdr, hdr_len) <= 0) {
        return -1;
    }

    /* Write masked payload */
    uint8_t *masked_payload = (uint8_t *)malloc(size);
    if (!masked_payload) return -1;

    const uint8_t *src = (const uint8_t *)data;
    for (size_t i = 0; i < size; i++) {
        masked_payload[i] = src[i] ^ mask[i % 4];
    }

    ssize_t wlen = write(ctx->sockfd, masked_payload, size);
    free(masked_payload);

    return (wlen == (ssize_t)size) ? 0 : -1;
}

static int ws_close_impl(void *ctx_ptr) {
    ws_transport_ctx_t *ctx = (ws_transport_ctx_t *)ctx_ptr;
    if (!ctx) return -1;

    if (atomic_load(&ctx->is_connected)) {
        atomic_store(&ctx->is_connected, false);
        atomic_store(&ctx->reader_running, false);

        if (ctx->sockfd >= 0) {
            shutdown(ctx->sockfd, SHUT_RDWR);
            close(ctx->sockfd);
            ctx->sockfd = -1;
        }

        pthread_join(ctx->reader_thread, NULL);
    }

    return 0;
}

int ws_transport_create(const char *uri, ws_transport_ctx_t **out_ctx) {
    if (!out_ctx) return -1;

    ws_transport_ctx_t *ctx = (ws_transport_ctx_t *)calloc(1, sizeof(ws_transport_ctx_t));
    if (!ctx) return -1;

    ctx->sockfd = -1;
    atomic_init(&ctx->is_connected, false);
    atomic_init(&ctx->reader_running, false);

    if (uri) {
        parse_ws_uri(uri, ctx->host, sizeof(ctx->host), &ctx->port, ctx->path, sizeof(ctx->path));
    }

    *out_ctx = ctx;
    return 0;
}

void ws_transport_set_read_callback(ws_transport_ctx_t *ctx, ws_transport_read_cb callback, void *user_data) {
    if (ctx) {
        ctx->read_cb = callback;
        ctx->read_user_data = user_data;
    }
}

void ws_transport_destroy(ws_transport_ctx_t *ctx) {
    if (!ctx) return;
    ws_close_impl(ctx);
    free(ctx);
}

signaling_transport_ops_t ws_transport_get_ops(void) {
    signaling_transport_ops_t ops = {
        .connect = ws_connect_impl,
        .send = ws_send_impl,
        .close = ws_close_impl
    };
    return ops;
}
