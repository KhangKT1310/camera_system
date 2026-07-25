#include "rtp_packetizer.h"
#include <arpa/inet.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define RTP_HEADER_SIZE 12
#define DEFAULT_MTU 1200

struct rtp_packetizer {
    rtp_packetizer_config_t config;
    rtp_packet_cb callback;
    void *user_data;
    uint16_t sequence_number;
};

static void write_rtp_header(uint8_t *buffer,
                             uint8_t payload_type,
                             uint16_t seq,
                             uint32_t timestamp,
                             uint32_t ssrc,
                             bool marker) {
    buffer[0] = 0x80; /* Version 2 */
    buffer[1] = (uint8_t)((marker ? 0x80 : 0x00) | (payload_type & 0x7F));

    uint16_t net_seq = htons(seq);
    memcpy(&buffer[2], &net_seq, sizeof(net_seq));

    uint32_t net_ts = htonl(timestamp);
    memcpy(&buffer[4], &net_ts, sizeof(net_ts));

    uint32_t net_ssrc = htonl(ssrc);
    memcpy(&buffer[8], &net_ssrc, sizeof(net_ssrc));
}

int rtp_packetizer_create(const rtp_packetizer_config_t *config,
                          rtp_packet_cb callback,
                          void *user_data,
                          rtp_packetizer_t **out_packetizer) {
    if (!callback || !out_packetizer) {
        return -1;
    }

    rtp_packetizer_t *p = (rtp_packetizer_t *)calloc(1, sizeof(rtp_packetizer_t));
    if (!p) {
        return -1;
    }

    if (config) {
        p->config = *config;
    } else {
        p->config.ssrc = 0x12345678;
        p->config.payload_type = 96;
        p->config.mtu = DEFAULT_MTU;
    }

    if (p->config.mtu < 100) {
        p->config.mtu = DEFAULT_MTU;
    }

    p->callback = callback;
    p->user_data = user_data;
    p->sequence_number = 1;

    *out_packetizer = p;
    return 0;
}

void rtp_packetizer_destroy(rtp_packetizer_t *packetizer) {
    if (packetizer) {
        free(packetizer);
    }
}

static const uint8_t *find_next_nalu(const uint8_t *start, const uint8_t *end, size_t *nalu_size, size_t *start_code_len) {
    const uint8_t *p = start;
    while (p + 3 < end) {
        if (p[0] == 0x00 && p[1] == 0x00) {
            if (p[2] == 0x01) {
                *start_code_len = 3;
                break;
            } else if (p[2] == 0x00 && p[3] == 0x01) {
                *start_code_len = 4;
                break;
            }
        }
        p++;
    }

    if (p + 3 >= end) {
        return NULL;
    }

    const uint8_t *nalu_start = p + (*start_code_len);
    const uint8_t *next_p = nalu_start;

    while (next_p + 3 < end) {
        if (next_p[0] == 0x00 && next_p[1] == 0x00) {
            if (next_p[2] == 0x01 || (next_p[2] == 0x00 && next_p[3] == 0x01)) {
                break;
            }
        }
        next_p++;
    }

    if (next_p + 3 >= end) {
        *nalu_size = (size_t)(end - nalu_start);
    } else {
        *nalu_size = (size_t)(next_p - nalu_start);
    }

    return nalu_start;
}

int rtp_packetizer_push_frame(rtp_packetizer_t *p, const media_frame_t *frame) {
    if (!p || !frame || !frame->data || frame->size == 0) {
        return -1;
    }

    uint32_t rtp_timestamp = (uint32_t)((frame->timestamp.pts_us * 90000ULL) / 1000000ULL);
    const uint8_t *buf_ptr = frame->data;
    const uint8_t *end_ptr = frame->data + frame->size;

    size_t nalu_size = 0;
    size_t start_code_len = 0;
    const uint8_t *nalu = find_next_nalu(buf_ptr, end_ptr, &nalu_size, &start_code_len);

    while (nalu && nalu_size > 0) {
        bool is_last_nalu = (nalu + nalu_size >= end_ptr);
        size_t max_payload = (size_t)p->config.mtu - RTP_HEADER_SIZE;

        if (nalu_size <= max_payload) {
            /* Single NALU Packet */
            uint8_t packet[DEFAULT_MTU + 64];
            write_rtp_header(packet, p->config.payload_type, p->sequence_number++, rtp_timestamp, p->config.ssrc, is_last_nalu);
            memcpy(&packet[RTP_HEADER_SIZE], nalu, nalu_size);
            p->callback(packet, RTP_HEADER_SIZE + nalu_size, p->user_data);
        } else {
            /* FU-A Fragmentation */
            uint8_t nalu_header = nalu[0];
            uint8_t nal_type = nalu_header & 0x1F;
            uint8_t nal_nri = nalu_header & 0x60;

            const uint8_t *payload_ptr = nalu + 1;
            size_t payload_remaining = nalu_size - 1;
            size_t fu_max_payload = max_payload - 2;

            bool is_first = true;
            while (payload_remaining > 0) {
                size_t chunk_size = (payload_remaining > fu_max_payload) ? fu_max_payload : payload_remaining;
                bool is_last_chunk = (payload_remaining == chunk_size);
                bool marker = is_last_nalu && is_last_chunk;

                uint8_t packet[DEFAULT_MTU + 64];
                write_rtp_header(packet, p->config.payload_type, p->sequence_number++, rtp_timestamp, p->config.ssrc, marker);

                packet[RTP_HEADER_SIZE] = (uint8_t)(nal_nri | 28); /* FU Indicator (28 = FU-A) */
                packet[RTP_HEADER_SIZE + 1] = (uint8_t)((is_first ? 0x80 : 0x00) | (is_last_chunk ? 0x40 : 0x00) | nal_type);

                memcpy(&packet[RTP_HEADER_SIZE + 2], payload_ptr, chunk_size);
                p->callback(packet, RTP_HEADER_SIZE + 2 + chunk_size, p->user_data);

                payload_ptr += chunk_size;
                payload_remaining -= chunk_size;
                is_first = false;
            }
        }

        buf_ptr = nalu + nalu_size;
        nalu = find_next_nalu(buf_ptr, end_ptr, &nalu_size, &start_code_len);
    }

    return 0;
}
