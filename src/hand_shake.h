#pragma once

const uint32_t CONV_REQ_CONN = 1;
const uint32_t CONV_SND_CONV = 2;
const uint32_t CONV_ACK_CONV = 3;
const uint32_t CONV_ESTABLISHED = 4;

const uint32_t CONV_RESERVED = 100;
const uint32_t CONV_USER = CONV_RESERVED + 1;

// client request server to alloc a new conv
typedef struct hs_req_conn_s {
	uint32_t conv; // CONV_REQ_CONN
	uint32_t n; // a random number
} hs_req_conn_s;

// server alloc a new conv for client
typedef struct hs_snd_conv_s {
	uint32_t conv; // CONV_SND_CONV
	uint32_t n; // same as hs_req_conn_s.n
	uint32_t new_conv;
	uint32_t key; // server side generated key for auth
} hs_snd_conv_s;

typedef struct kcpuv_pack_header_s {
	uint32_t key;
} kcpuv_pack_header_s;

// client response server to establish a new connection
typedef struct hs_ack_conv_s {
	kcpuv_pack_header_s header;
} hs_ack_conv_s;

