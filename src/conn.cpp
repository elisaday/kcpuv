#include "header.h"
#include "conn.h"
#include "network.h"

struct send_req_s {
	uv_udp_send_t req;
	uv_buf_t buf;
};

static int on_kcp_output(const char *buf, int len, struct IKCPCB *kcp, void *user) {
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

int Conn::init(kcpuv_conv_t conv, const sockaddr* addr, uv_udp_t* handle) {
	_conv = conv;
	_addr = *addr;
	_udp = handle;

	int r = -1;
	_kcp = ikcp_create(conv, (void*)this);
	CHK_COND(_kcp);

	_kcp->output = on_kcp_output;

	r = ikcp_nodelay(_kcp, 1, 10, 2, 1);
	PROC_ERR(r);

	return 0;
Exit0:
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
	int len = ikcp_peeksize(_kcp);
	if (len < 0) {
		return -1;
	}

	char* data = new char[len];
	int r = ikcp_recv(_kcp, data, len);
	if (r < 0) {
		delete[]data;
		return r;
	}

	buf = data;
	size = (uint32_t)len;
	return 0;
}

int Conn::send_udp(const char* buf, uint32_t len) {
	int r = -1;
	send_req_s* req = new send_req_s;
	CHK_COND(req);

	req->buf.base = new char[len];
	req->buf.len = len;

	memcpy(req->buf.base, buf, len);

	r = uv_udp_send((uv_udp_send_t*)req, _udp, &req->buf, 1, &_addr, on_send_done);
	if (r < 0) {
		SAFE_DELETE_ARRAY(req->buf.base);
		SAFE_DELETE(req);
		return -1;
	}

	return 0;
Exit0:
	return r;
}

int Conn::send_kcp(const char* buf, uint32_t len) {
	return ikcp_send(_kcp, buf, len);
}

int Conn::run(uint64_t tick) {
	ikcp_update(_kcp, (uint32_t)tick);

	char* buf;
	uint32_t size;
	int r = recv_kcp(buf, size);
	if (r < 0)
		return r;

	kcpuv_msg_t msg;
	msg.conv = _conv;
	msg.data = (uint8_t*)buf;
	msg.size = size;
	_network->push_msg(msg);
	return 0;
}
