#include "signaling_client.h"
#include "webrtc_transport.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char g_last_payload[1024] = {0};

static void signaling_payload_cb(const char *type, const char *session_id, const char *payload_json, void *user_data) {
    (void)type;
    (void)session_id;
    (void)user_data;
    if (payload_json) {
        strncpy(g_last_payload, payload_json, sizeof(g_last_payload) - 1);
    }
}

static void test_signaling_malformed_json(void) {
    signaling_client_config_t config = {0};
    signaling_client_t *client = NULL;

    int ret = signaling_client_create(&config, signaling_payload_cb, NULL, &client);
    assert(ret == 0);

    /* Reject NULL input */
    assert(signaling_client_receive_raw(client, NULL) != 0);

    /* Reject empty JSON */
    assert(signaling_client_receive_raw(client, "") != 0);

    /* Reject JSON lacking required "type" field */
    assert(signaling_client_receive_raw(client, "{\"session_id\":\"123\"}") != 0);

    /* Reject trailing garbage input */
    assert(signaling_client_receive_raw(client, "{\"type\":\"offer\"}garbage") != 0);

    /* Test oversized payload (> 64KB) */
    char *oversized = (char *)malloc(70000);
    memset(oversized, 'A', 69999);
    oversized[69999] = '\0';
    assert(signaling_client_receive_raw(client, oversized) != 0);
    free(oversized);

    signaling_client_destroy(client);
    printf("test_signaling_malformed_json passed!\n");
}

static void test_signaling_payload_isolation(void) {
    signaling_client_config_t config = {0};
    signaling_client_t *client = NULL;

    int ret = signaling_client_create(&config, signaling_payload_cb, NULL, &client);
    assert(ret == 0);

    /* Test object payload isolation */
    const char *envelope_obj =
        "{\n"
        "  \"version\": 1,\n"
        "  \"type\": \"OFFER\",\n"
        "  \"session_id\": \"sess-100\",\n"
        "  \"payload\": {\"sdp\":\"v=0...\"}\n"
        "}";

    memset(g_last_payload, 0, sizeof(g_last_payload));
    ret = signaling_client_receive_raw(client, envelope_obj);
    assert(ret == 0);
    assert(strcmp(g_last_payload, "{\"sdp\":\"v=0...\"}") == 0);

    /* Test escaped quote payload isolation e.g. {"payload":{"sdp":"a\"b"}} */
    const char *envelope_escaped =
        "{\n"
        "  \"type\": \"OFFER\",\n"
        "  \"payload\": {\"sdp\":\"a\\\"b\"}\n"
        "}";

    memset(g_last_payload, 0, sizeof(g_last_payload));
    ret = signaling_client_receive_raw(client, envelope_escaped);
    assert(ret == 0);
    assert(strcmp(g_last_payload, "{\"sdp\":\"a\\\"b\"}") == 0);

    /* Test control character payload isolation e.g. \u0001 */
    const char *envelope_ctrl =
        "{\n"
        "  \"type\": \"OFFER\",\n"
        "  \"payload\": {\"ctrl\":\"\\u0001\"}\n"
        "}";

    memset(g_last_payload, 0, sizeof(g_last_payload));
    ret = signaling_client_receive_raw(client, envelope_ctrl);
    assert(ret == 0);
    assert(strcmp(g_last_payload, "{\"ctrl\":\"\\u0001\"}") == 0);

    /* Test array payload isolation */
    const char *envelope_arr =
        "{\n"
        "  \"version\": 1,\n"
        "  \"type\": \"CANDIDATE\",\n"
        "  \"session_id\": \"sess-100\",\n"
        "  \"payload\": [\"cand1\", \"cand2\"]\n"
        "}";

    memset(g_last_payload, 0, sizeof(g_last_payload));
    ret = signaling_client_receive_raw(client, envelope_arr);
    assert(ret == 0);
    assert(strcmp(g_last_payload, "[\"cand1\",\"cand2\"]") == 0);

    signaling_client_destroy(client);
    printf("test_signaling_payload_isolation passed!\n");
}

static void test_transport_candidate_bounds(void) {
    webrtc_transport_callbacks_t callbacks = {0};
    webrtc_transport_config_t config = {0};
    webrtc_transport_t *transport = NULL;

    int ret = webrtc_transport_create(&config, &callbacks, NULL, &transport);
    assert(ret == 0);

    /* Attempt to queue more than WEBRTC_MAX_PENDING_CANDIDATES before SDP */
    for (int i = 0; i < WEBRTC_MAX_PENDING_CANDIDATES; i++) {
        ret = webrtc_transport_add_remote_candidate(transport, "candidate:1 1 UDP 2122260223 127.0.0.1 5000 typ host", "video");
        assert(ret == 0);
    }

    /* 33rd candidate should be rejected due to queue overflow bounds */
    ret = webrtc_transport_add_remote_candidate(transport, "candidate:1 1 UDP 2122260223 127.0.0.1 5001 typ host", "video");
    assert(ret != 0);

    webrtc_transport_destroy(transport);
    printf("test_transport_candidate_bounds passed!\n");
}

int main(void) {
    printf("Running malformed_input fuzz and bounds tests...\n");
    test_signaling_malformed_json();
    test_signaling_payload_isolation();
    test_transport_candidate_bounds();
    printf("All malformed_input tests passed successfully.\n");
    return 0;
}
