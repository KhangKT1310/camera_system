#include "webrtc_transport.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static bool g_on_local_desc_called = false;
static bool g_on_local_cand_called = false;
static webrtc_state_t g_last_state = WEBRTC_STATE_IDLE;

static void on_local_description(void *user, const char *type, const char *sdp) {
    (void)user;
    assert(type != NULL);
    assert(sdp != NULL);
    g_on_local_desc_called = true;
}

static void on_local_candidate(void *user, const char *candidate, const char *mid) {
    (void)user;
    assert(candidate != NULL);
    assert(mid != NULL);
    g_on_local_cand_called = true;
}

static void on_state_changed(void *user, webrtc_state_t state) {
    (void)user;
    g_last_state = state;
}

static void test_transport_lifecycle(void) {
    g_on_local_desc_called = false;
    g_on_local_cand_called = false;

    webrtc_transport_callbacks_t callbacks = {
        .on_local_description = on_local_description,
        .on_local_candidate = on_local_candidate,
        .on_state_changed = on_state_changed
    };

    webrtc_transport_config_t config = {0};
    webrtc_transport_t *transport = NULL;

    int ret = webrtc_transport_create(&config, &callbacks, NULL, &transport);
    assert(ret == 0);
    assert(transport != NULL);
    assert(webrtc_transport_get_state(transport) == WEBRTC_STATE_IDLE);

    /* Test candidate-before-SDP queueing */
    ret = webrtc_transport_add_remote_candidate(transport, "candidate:1 1 UDP 2122260223 192.168.1.100 50000 typ host", "video");
    assert(ret == 0);

    /* Test Offer Creation */
    ret = webrtc_transport_create_offer(transport);
    assert(ret == 0);
    assert(g_on_local_desc_called);
    assert(g_on_local_cand_called);

    /* Test setting remote SDP Answer & candidate queue flushing */
    const char *mock_answer_sdp =
        "v=0\r\n"
        "o=- 87654321 2 IN IP4 127.0.0.1\r\n"
        "s=Viewer Answer\r\n"
        "m=video 9 UDP/TLS/RTP/SAVPF 96\r\n"
        "a=recvonly\r\n";

    ret = webrtc_transport_set_remote_description(transport, "answer", mock_answer_sdp);
    assert(ret == 0);
    assert(webrtc_transport_get_state(transport) == WEBRTC_STATE_CONNECTED);

    webrtc_transport_destroy(transport);
    printf("test_transport_lifecycle passed!\n");
}

static void test_libdatachannel_backend_dispatch(void) {
    g_on_local_desc_called = false;
    g_on_local_cand_called = false;

    webrtc_transport_callbacks_t callbacks = {
        .on_local_description = on_local_description,
        .on_local_candidate = on_local_candidate,
        .on_state_changed = on_state_changed
    };

    webrtc_transport_config_t config = {
        .enable_libdatachannel = true
    };
    webrtc_transport_t *transport = NULL;

    int ret = webrtc_transport_create(&config, &callbacks, NULL, &transport);
    assert(ret == 0);
    assert(transport != NULL);

    ret = webrtc_transport_create_offer(transport);
    assert(ret == 0);
    assert(g_on_local_desc_called);
    assert(g_on_local_cand_called);

    const char *mock_answer = "v=0\r\nm=video 9 UDP/TLS/RTP/SAVPF 96\r\n";
    ret = webrtc_transport_set_remote_description(transport, "answer", mock_answer);
    assert(ret == 0);
    assert(g_last_state == WEBRTC_STATE_CONNECTED);

    uint8_t dummy_rtp[20] = {0x80, 0x60, 0x00, 0x01};
    ret = webrtc_transport_send_rtp(transport, dummy_rtp, sizeof(dummy_rtp));
    assert(ret == 0);

    webrtc_transport_destroy(transport);
    printf("test_libdatachannel_backend_dispatch passed!\n");
}

int main(void) {
    printf("Running webrtc_transport tests...\n");
    test_transport_lifecycle();
    test_libdatachannel_backend_dispatch();
    printf("All webrtc_transport tests passed successfully.\n");
    return 0;
}
