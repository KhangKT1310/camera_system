#include "signaling_client.h"
#include "json_parser.h"
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

    json_node_t *root = json_parse(raw_json);
    if (!root || root->type != JSON_TYPE_OBJECT) {
        if (root) json_free(root);
        return -1;
    }

    json_node_t *type_node = json_get_child(root, "type");
    if (!type_node || type_node->type != JSON_TYPE_STRING || !type_node->val_string) {
        json_free(root);
        return -1;
    }

    const char *type = type_node->val_string;
    const char *session_id = "";
    json_node_t *sess_node = json_get_child(root, "session_id");
    if (sess_node && sess_node->type == JSON_TYPE_STRING && sess_node->val_string) {
        session_id = sess_node->val_string;
    }

    char payload_buf[MAX_JSON_ENVELOPE_LEN] = "{}";
    json_node_t *payload_node = json_get_child(root, "payload");
    if (payload_node) {
        if (json_stringify(payload_node, payload_buf, sizeof(payload_buf)) < 0) {
            json_free(root);
            return -1;
        }
    }

    if (c->callback) {
        c->callback(type, session_id, payload_buf, c->user_data);
    }

    json_free(root);
    return 0;
}
