#include "ws_transport.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
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
    bool is_connected;
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

    ctx->is_connected = true;
    return 0;
}

static int ws_send_impl(void *ctx_ptr, const void *data, size_t size) {
    ws_transport_ctx_t *ctx = (ws_transport_ctx_t *)ctx_ptr;
    if (!ctx || ctx->sockfd < 0 || !ctx->is_connected || !data || size == 0) {
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

    if (ctx->sockfd >= 0) {
        close(ctx->sockfd);
        ctx->sockfd = -1;
    }
    ctx->is_connected = false;
    return 0;
}

int ws_transport_create(const char *uri, ws_transport_ctx_t **out_ctx) {
    if (!out_ctx) return -1;

    ws_transport_ctx_t *ctx = (ws_transport_ctx_t *)calloc(1, sizeof(ws_transport_ctx_t));
    if (!ctx) return -1;

    ctx->sockfd = -1;
    ctx->is_connected = false;

    if (uri) {
        parse_ws_uri(uri, ctx->host, sizeof(ctx->host), &ctx->port, ctx->path, sizeof(ctx->path));
    }

    *out_ctx = ctx;
    return 0;
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
