#pragma once
#include "uv.h"
#include "conn.h"

typedef int (*recv_msg_callback)(char* buf, uint32_t size);

class Network {
public:
	Network();
	~Network();

	int init();
	void shutdown();

	void run();

	int udp_listen(const char* local_addr, int port);
	Conn* kcp_conn(kcpuv_conv_t conv, const char* local_addr, int port);

	Conn* get_conn_by_conv(kcpuv_conv_t conv);

	int get_msg(kcpuv_msg_t* msg);

public:
	void on_recv_udp(const char* buf, ssize_t size, const struct sockaddr* addr);
	Conn* add_conn(kcpuv_conv_t conv, const sockaddr* addr);
	void push_msg(kcpuv_msg_t& msg);

private:
	uv_loop_t* _loop;
	uv_udp_t _udp;

	std::map<kcpuv_conv_t, Conn*> _map_conn;

	std::list<kcpuv_msg_t> _queue_msg;
};
