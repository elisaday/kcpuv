#include "header.h"
#include "conn.h"
#include "network.h"
#include "rand.h"
#include "tm.h"

static const uint64_t CONN_EXPIRED_TIMEOUT = 30 * 1000;
static const uint32_t MAX_SEND_KCP_SIZE = 64 * 1024;

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
	_udp = NULL;
	_conv = 0;
	_kcp = NULL;
	_key = 0;
	_status = CONV_INVALID;
	_conv_expired_tick = 0;
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
	kcpuv_pack_header_s* header = NULL;

	int len = ikcp_peeksize(_kcp);
	CHK_COND_NOLOG(len > 0);

	data = new char[len];
	r = ikcp_recv(_kcp, data, len);
	PROC_ERR(r);

	// verify key
	CHK_COND_NOLOG(len >= sizeof(kcpuv_pack_header_s));
	header = (kcpuv_pack_header_s*)data;
	CHK_COND_NOLOG(header->key == _key);

	size = (uint32_t)len - sizeof(kcpuv_pack_header_s);
	if (size > 0) {
		buf = new char[size];
		memcpy(buf, data + sizeof(kcpuv_pack_header_s), size);
	} else {
		buf = NULL;
	}

	r = 0;
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

int Conn::send_kcp_raw(const char* buf, uint32_t len) {
	return ikcp_send(_kcp, buf, len);
}

int Conn::send_kcp(const char* buf, uint32_t len) {
	if (len > MAX_SEND_KCP_SIZE - sizeof(kcpuv_pack_header_s))
		return -1;

	static char tmp[MAX_SEND_KCP_SIZE];
	kcpuv_pack_header_s header;
	header.key = _key;
	memcpy(tmp, &header, sizeof(header));
	memcpy(tmp + sizeof(header), buf, len);
	return ikcp_send(_kcp, tmp, len + sizeof(header));
}

int Conn::expired() {
	return get_tick_ms() > _conv_expired_tick ? 0 : -1;
}

void Conn::alive() {
	_conv_expired_tick = get_tick_ms() + CONN_EXPIRED_TIMEOUT;
}

int Conn::run(uint64_t tick) {
	ikcp_update(_kcp, (uint32_t)tick);

	int msg_cnt = 0;
	while (true) {
		char* buf;
		uint32_t size;

		int r = recv_kcp(buf, size);
		if (r < 0) {
			break;
		} else if (r == 0) {
			kcpuv_msg_t msg;
			msg.conv = _conv;
			msg.data = (uint8_t*)buf;
			msg.size = size;
			_network->push_msg(msg);
		}

		msg_cnt++;
	}

	if (msg_cnt > 0) {
		alive();
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
	return rand_uint32();
}

uint32_t Conn::new_conv() {
	uint32_t conv;
	do {
		conv = rand_uint32();
	} while (_network->get_conn_by_conv(conv) != NULL);
	return conv;
}
