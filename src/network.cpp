#include "header.h"
#include "network.h"
#include "tm.h"

static void on_alloc_buffer(uv_handle_t* handle, size_t size, uv_buf_t* buf) {
	buf->len = (unsigned long)size;
	buf->base = new char[size];
}

static void on_network_recv_udp(
    uv_udp_t* handle,
    ssize_t nread,
    const uv_buf_t* rcvbuf,
    const struct sockaddr* addr,
    unsigned flags) {

	Network* server;
	if (nread <= 0) {
		goto Exit0;
	}

	server = (Network*)(handle->data);
	server->on_recv_udp(rcvbuf->base, nread, addr);

Exit0:
	delete []rcvbuf->base;
}

static void on_conn_recv_udp(
    uv_udp_t* handle,
    ssize_t nread,
    const uv_buf_t* rcvbuf,
    const struct sockaddr* addr,
    unsigned flags) {

	Conn* conn;
	if (nread <= 0) {
		goto Exit0;
	}

	conn = (Conn*)(handle->data);
	conn->on_recv_udp(rcvbuf->base, nread, addr);

Exit0:
	delete[]rcvbuf->base;
}

static void on_close_done(uv_handle_t* handle) {

}

Network::Network() {
	_loop = NULL;
}

Network::~Network() {

}

int Network::init() {
	_loop = uv_loop_new();
#ifdef PLATFORM_WINDOWS
	SetErrorMode(0);
#endif
	return 0;
}

void Network::shutdown() {
	uv_close((uv_handle_t*)&_udp, on_close_done);
	uv_run(_loop, UV_RUN_DEFAULT);
	uv_loop_delete(_loop);

	for (std::map<kcpuv_conv_t, Conn*>::iterator it = _map_conn.begin();
	        it != _map_conn.end(); ++it) {
		Conn* conn = it->second;
		conn->shutdown();
		SAFE_DELETE(conn);
	}
}

void Network::run() {
	uv_run(_loop, UV_RUN_NOWAIT);

	for (std::map<kcpuv_conv_t, Conn*>::iterator it = _map_conn.begin();
	        it != _map_conn.end(); ++it) {
		Conn* conn = it->second;
		conn->run(get_tick_ms());
	}
}

int Network::udp_listen(const char* local_addr, int32_t port) {
	int r = -1;
	struct sockaddr_in bind_addr;

	_udp.data = this;
	uv_udp_init(_loop, &_udp);

	r = uv_ip4_addr(local_addr, port, &bind_addr);
	PROC_ERR(r);

	r = uv_udp_bind(&_udp, (const struct sockaddr*)&bind_addr, 0);
	PROC_ERR(r);

	r = uv_udp_recv_start(&_udp, on_alloc_buffer, on_network_recv_udp);
	PROC_ERR(r);

	log_info("udp listen port: %d", port);

	return 0;
Exit0:
	return r;
}

Conn* Network::kcp_conn(kcpuv_conv_t conv, const char* local_addr, int port) {
	int r = -1;
	Conn* conn = NULL;

	struct sockaddr_in addr;
	r = uv_ip4_addr(local_addr, port, &addr);
	PROC_ERR(r);

	conn = new Conn(this);
	conn->init(conv, (const struct sockaddr*)&addr, &_udp);

	_udp.data = conn;
	uv_udp_init(_loop, &_udp);

	r = uv_udp_recv_start(&_udp, on_alloc_buffer, on_conn_recv_udp);
	PROC_ERR(r);

	_map_conn[conv] = conn;
	return conn;

Exit0:
	SAFE_DELETE(conn);
	return NULL;
}

void Network::on_recv_udp(const char* buf, ssize_t size, const struct sockaddr* addr) {
	kcpuv_conv_t conv;
	int ret = ikcp_get_conv(buf, (long)size, &conv);

	if (ret == 0)
		return;

	Conn* conn = get_conn_by_conv(conv);

	if (!conn) {
		conn = add_conn(conv, addr);
	}

	if (conn) {
		conn->on_recv_udp(buf, size, addr);
	}
}

Conn* Network::get_conn_by_conv(kcpuv_conv_t conv) {
	std::map<kcpuv_conv_t, Conn*>::iterator it = _map_conn.find(conv);

	if (it != _map_conn.end())
		return it->second;

	return NULL;
}

Conn* Network::add_conn(kcpuv_conv_t conv, const sockaddr* addr) {
	Conn* conn = new Conn(this);
	conn->init(conv, addr, &_udp);
	_map_conn[conv] = conn;
	return conn;
}

int Network::get_msg(kcpuv_msg_t* msg) {
	if (_queue_msg.size() == 0)
		return -1;

	*msg = _queue_msg.front();
	_queue_msg.pop_front();
	return 0;
}

void Network::push_msg(kcpuv_msg_t& msg) {
	_queue_msg.push_back(msg);
}

struct kcpuv_s {
	Network network;
};

kcpuv_t kcpuv_create() {
	kcpuv_s* kcpuv = new kcpuv_s;

	int r = kcpuv->network.init();
	PROC_ERR(r);

	return kcpuv;
Exit0:
	SAFE_DELETE(kcpuv);
	return NULL;
}

void kcpuv_destroy(kcpuv_t kcpuv) {
	kcpuv->network.shutdown();
	SAFE_DELETE(kcpuv);
}

int kcpuv_listen(kcpuv_t kcpuv, const char* addr, uint32_t port) {
	return kcpuv->network.udp_listen(addr, port);
}

int kcpuv_connect(kcpuv_t kcpuv, kcpuv_conv_t conv, const char* addr, uint32_t port) {
	Conn* conn = kcpuv->network.kcp_conn(conv, addr, port);
	if (!conn)
		return -1;
	return 0;
}

void kcpuv_run(kcpuv_t kcpuv) {
	kcpuv->network.run();
}

int kcpuv_recv(kcpuv_t kcpuv, kcpuv_msg_t* msg) {
	return kcpuv->network.get_msg(msg);
}

int kcpuv_send(kcpuv_t kcpuv, kcpuv_conv_t conv, const void* data, uint32_t size) {
	Conn* conn = kcpuv->network.get_conn_by_conv(conv);

	if (!conn)
		return -1;

	return conn->send_kcp((const char*)data, size);
}

void kcpuv_msg_free(kcpuv_msg_t* msg) {
	SAFE_DELETE_ARRAY(msg->data);
}