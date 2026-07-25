#include "signaling_client.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_JSON_ENVELOPE_LEN 65536
#define MAX_FIELD_LEN 256

struct signaling_client {
    signaling_client_config_t config;
    signaling_message_cb callback;
    void *user_data;
    bool is_connected;
    uint64_t request_seq;
};

int signaling_client_create(const signaling_client_config_t *config,
                            signaling_message_cb callback,
                            void *user_data,
                            signaling_client_t **out_client) {
    if (!config || !callback || !out_client) {
        return -1;
    }

    signaling_client_t *c = (signaling_client_t *)calloc(1, sizeof(signaling_client_t));
    if (!c) {
        return -1;
    }

    c->config = *config;
    c->callback = callback;
    c->user_data = user_data;
    c->is_connected = false;
    c->request_seq = 1;

    *out_client = c;
    return 0;
}

void signaling_client_destroy(signaling_client_t *c) {
    if (!c) {
        return;
    }

    if (c->is_connected && c->config.transport_ops.close) {
        c->config.transport_ops.close(c->config.transport_ctx);
    }

    free(c);
}

int signaling_client_connect(signaling_client_t *c) {
    if (!c) {
        return -1;
    }

    if (c->config.transport_ops.connect) {
        int ret = c->config.transport_ops.connect(c->config.transport_ctx, c->config.server_uri);
        if (ret == 0) {
            c->is_connected = true;
            return 0;
        }
        return ret;
    }

    c->is_connected = true;
    return 0;
}

int signaling_client_send_message(signaling_client_t *c,
                                  const char *type,
                                  const char *session_id,
                                  const char *payload_json) {
    if (!c || !type || !session_id || !payload_json) {
        return -1;
    }

    if (strlen(type) > MAX_FIELD_LEN || strlen(session_id) > MAX_FIELD_LEN || strlen(payload_json) > (MAX_JSON_ENVELOPE_LEN - 512)) {
        return -1; /* Input bounds failure */
    }

    char buffer[MAX_JSON_ENVELOPE_LEN];
    uint64_t now_ts = (uint64_t)time(NULL);

    int len = snprintf(buffer, sizeof(buffer),
                       "{\n"
                       "  \"version\": 1,\n"
                       "  \"type\": \"%s\",\n"
                       "  \"session_id\": \"%s\",\n"
                       "  \"request_id\": \"req-%llu\",\n"
                       "  \"timestamp\": %llu,\n"
                       "  \"payload\": %s\n"
                       "}",
                       type, session_id, (unsigned long long)(c->request_seq++), (unsigned long long)now_ts, payload_json);

    if (len <= 0 || (size_t)len >= sizeof(buffer)) {
        return -1;
    }

    if (c->config.transport_ops.send && c->is_connected) {
        return c->config.transport_ops.send(c->config.transport_ctx, buffer, (size_t)len);
    }

    return 0;
}

int signaling_client_receive_raw(signaling_client_t *c, const char *raw_json) {
    if (!c || !raw_json) {
        return -1;
    }

    size_t raw_len = strlen(raw_json);
    if (raw_len == 0 || raw_len > MAX_JSON_ENVELOPE_LEN) {
        return -1; /* Malformed or oversized envelope */
    }

    /* Basic C string parsing for type, session_id, and payload bounds */
    char type[MAX_FIELD_LEN] = {0};
    char session_id[MAX_FIELD_LEN] = {0};

    const char *type_pos = strstr(raw_json, "\"type\"");
    if (!type_pos) {
        return -1;
    }
    const char *val_start = strchr(type_pos + 6, '"');
    if (!val_start) {
        return -1;
    }
    const char *val_end = strchr(val_start + 1, '"');
    if (!val_end || (size_t)(val_end - val_start - 1) >= sizeof(type)) {
        return -1;
    }
    strncpy(type, val_start + 1, (size_t)(val_end - val_start - 1));

    const char *sess_pos = strstr(raw_json, "\"session_id\"");
    if (sess_pos) {
        val_start = strchr(sess_pos + 12, '"');
        if (val_start) {
            val_end = strchr(val_start + 1, '"');
            if (val_end && (size_t)(val_end - val_start - 1) < sizeof(session_id)) {
                strncpy(session_id, val_start + 1, (size_t)(val_end - val_start - 1));
            }
        }
    }

    const char *payload_pos = strstr(raw_json, "\"payload\"");
    const char *payload_json = payload_pos ? strchr(payload_pos + 9, ':') : NULL;
    if (payload_json) {
        payload_json++;
        while (*payload_json == ' ' || *payload_json == '\n' || *payload_json == '\r') {
            payload_json++;
        }
    } else {
        payload_json = "{}";
    }

    if (c->callback) {
        c->callback(type, session_id, payload_json, c->user_data);
    }

    return 0;
}
