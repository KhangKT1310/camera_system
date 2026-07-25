#include "media_frame.h"
#include "media_pipeline.h"
#include "rtp_packetizer.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_packet_count = 0;
static size_t g_last_packet_size = 0;
static uint8_t g_last_payload_type = 0;
static uint32_t g_last_ssrc = 0;

static void on_rtp_packet(const uint8_t *packet_data, size_t packet_size, void *user_data) {
    (void)user_data;
    assert(packet_data != NULL);
    assert(packet_size > 12);
    assert((packet_data[0] & 0xC0) == 0x80); /* Version 2 */

    g_packet_count++;
    g_last_packet_size = packet_size;
    g_last_payload_type = packet_data[1] & 0x7F;

    uint32_t ssrc_net = 0;
    memcpy(&ssrc_net, &packet_data[8], sizeof(uint32_t));
    g_last_ssrc = ((ssrc_net & 0xFF000000) >> 24) |
                  ((ssrc_net & 0x00FF0000) >> 8) |
                  ((ssrc_net & 0x0000FF00) << 8) |
                  ((ssrc_net & 0x000000FF) << 24);
}

static void test_rtp_single_nalu(void) {
    g_packet_count = 0;
    rtp_packetizer_config_t config = {.ssrc = 0x11223344, .payload_type = 96, .mtu = 1200};
    rtp_packetizer_t *packetizer = NULL;

    int ret = rtp_packetizer_create(&config, on_rtp_packet, NULL, &packetizer);
    assert(ret == 0);
    assert(packetizer != NULL);

    /* Construct Annex-B frame with SPS + PPS + IDR */
    uint8_t dummy_frame[] = {
        0x00, 0x00, 0x00, 0x01, 0x67, 0x42, 0x00, 0x1f, /* SPS */
        0x00, 0x00, 0x00, 0x01, 0x68, 0xce, 0x3c, 0x80, /* PPS */
        0x00, 0x00, 0x01, 0x65, 0x88, 0x84, 0x00, 0x10  /* IDR */
    };

    media_buffer_t *buf = media_buffer_create(sizeof(dummy_frame));
    memcpy(buf->data, dummy_frame, sizeof(dummy_frame));
    buf->size = sizeof(dummy_frame);

    media_frame_t frame = {
        .buffer = buf,
        .data = buf->data,
        .size = buf->size,
        .timestamp = {.pts_us = 1000000, .capture_time_us = 1000000, .sequence = 1},
        .codec = MEDIA_CODEC_H264,
        .frame_type = MEDIA_FRAME_TYPE_IDR,
        .flags = 0
    };

    ret = rtp_packetizer_push_frame(packetizer, &frame);
    assert(ret == 0);
    assert(g_packet_count == 3);
    assert(g_last_payload_type == config.payload_type);
    assert(g_last_ssrc == config.ssrc);

    media_buffer_release(buf);
    rtp_packetizer_destroy(packetizer);
    printf("test_rtp_single_nalu passed with cross-layer PT/SSRC assertions!\n");
}

static void test_rtp_fua_fragmentation(void) {
    g_packet_count = 0;
    rtp_packetizer_config_t config = {.ssrc = 0x55667788, .payload_type = 96, .mtu = 500};
    rtp_packetizer_t *packetizer = NULL;

    int ret = rtp_packetizer_create(&config, on_rtp_packet, NULL, &packetizer);
    assert(ret == 0);

    /* Construct 1200-byte NALU */
    size_t frame_size = 1200;
    media_buffer_t *buf = media_buffer_create(frame_size);
    buf->data[0] = 0x00;
    buf->data[1] = 0x00;
    buf->data[2] = 0x00;
    buf->data[3] = 0x01;
    buf->data[4] = 0x65; /* IDR NALU Header */
    memset(&buf->data[5], 0xAA, frame_size - 5);
    buf->size = frame_size;

    media_frame_t frame = {
        .buffer = buf,
        .data = buf->data,
        .size = buf->size,
        .timestamp = {.pts_us = 2000000, .capture_time_us = 2000000, .sequence = 2},
        .codec = MEDIA_CODEC_H264,
        .frame_type = MEDIA_FRAME_TYPE_IDR,
        .flags = 0
    };

    ret = rtp_packetizer_push_frame(packetizer, &frame);
    assert(ret == 0);
    assert(g_packet_count >= 3);
    assert(g_last_payload_type == config.payload_type);
    assert(g_last_ssrc == config.ssrc);

    media_buffer_release(buf);
    rtp_packetizer_destroy(packetizer);
    printf("test_rtp_fua_fragmentation passed!\n");
}

int main(void) {
    printf("Running rtp_packetizer tests...\n");
    test_rtp_single_nalu();
    test_rtp_fua_fragmentation();
    printf("All rtp_packetizer tests passed successfully.\n");
    return 0;
}
