#include "header.h"
#include "conn.h"
#include "network.h"
#include "rand.h"
#include "tm.h"

static const uint64_t CONN_EXPIRED_TIMEOUT = 30 * 1000;

struct send_req_s {
	uv_udp_send_t req;
	uv_buf_t buf;
};

static int on_kcp_output(const char* buf, int len, struct IKCPCB* kcp, void* user) {
	Conn* conn = (Conn*)user;
	return conn->send_udp(buf, len);
}

static void on_send_done(uv_udp_send_t* req, int status) {
	send_req_s* send_req = (send_req_s*)req;
	SAFE_DELETE_ARRAY(send_req->buf.base);
	SAFE_DELETE(send_req);
}

Conn::Conn(Network* network) {
	_network = network;
	_conv = 0;
	_udp = NULL;
	_kcp = NULL;
}

Conn::~Conn() {

}

int Conn::init_kcp(kcpuv_conv_t conv) {
	int r = -1;

	_kcp = ikcp_create(conv, (void*)this);
	CHK_COND(_kcp);

	_kcp->output = on_kcp_output;

	r =- ikcp_nodelay(_kcp, 1, 10, 2, 1);
	PROC_ERR(r);

	_conv = conv;
	return 0;
Exit0:
	ikcp_release(_kcp);
	_kcp = NULL;
	return -1;
}

void Conn::shutdown() {
	ikcp_release(_kcp);
}

void Conn::on_recv_udp(const char* buf, ssize_t size, const struct sockaddr* addr) {
	_addr = *addr;
	ikcp_input(_kcp, buf, (long)size);
}

int Conn::recv_kcp(char*& buf, uint32_t& size) {
	int r = -1;
	char* data = NULL;

	int len = ikcp_peeksize(_kcp);
	CHK_COND_NOLOG(len > 0);

	data = new char[len];
	r = ikcp_recv(_kcp, data, len);
	PROC_ERR(r);

	// verify key
	CHK_COND_NOLOG(len >= sizeof(kcpuv_pack_header_s));
	kcpuv_pack_header_s* header = (kcpuv_pack_header_s*)data;
	CHK_COND_NOLOG(header->key == _key);

	buf = data;
	size = (uint32_t)len;

	if (len == sizeof(kcpuv_pack_header_s)) {
		hs_ack_conv_s ack;
		ack.header.key = _key;
		r = send_kcp((const char*)&ack, sizeof(ack));
		PROC_ERR(r);
	}

	return 0;

Exit0:
	SAFE_DELETE_ARRAY(data);
	return r;
}

int Conn::send_udp(const char* buf, uint32_t len) {
	int r = -1;
	send_req_s* req = NULL;
	
	req = new send_req_s;
	CHK_COND(req);

	req->buf.base = new char[len];
	req->buf.len = len;

	memcpy(req->buf.base, buf, len);

	r = uv_udp_send((uv_udp_send_t*)req, _udp, &req->buf, 1, &_addr, on_send_done);
	PROC_ERR(r);

	return 0;
Exit0:
	if (req) {
		SAFE_DELETE_ARRAY(req->buf.base);
	}

	SAFE_DELETE(req);
	return r;
}

int Conn::send_kcp(const char* buf, uint32_t len) {
	return ikcp_send(_kcp, buf, len);
}

int Conn::expired() {
	return get_tick_ms() > _conv_expired_tick ? 0 : -1;
}

int Conn::run(uint64_t tick) {
	ikcp_update(_kcp, (uint32_t)tick);

	int msg_cnt = 0;
	while (true) {
		char* buf;
		uint32_t size;

		int r = recv_kcp(buf, size);
		if (r < 0)
			break;

		kcpuv_msg_t msg;
		msg.conv = _conv;
		msg.data = (uint8_t*)buf;
		msg.size = size;
		_network->push_msg(msg);

		msg_cnt++;
	}

	if (msg_cnt > 0) {
		_conv_expired_tick = get_tick_ms() + CONN_EXPIRED_TIMEOUT;
	}

	return msg_cnt;
}

uint32_t Conn::status() {
	return _status;
}

kcpuv_conv_t Conn::get_conv() {
	return _conv;
}

uint32_t Conn::new_key() {
	static uint32_t key = 1000;
	return key++;
}

uint32_t Conn::new_conv() {
	static uint32_t conv = 1000;
	return conv++;
}