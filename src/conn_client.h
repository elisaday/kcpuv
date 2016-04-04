#pragma once
#include "conn.h"

class ConnClient : public Conn {
public:
	ConnClient(Network* network);
	~ConnClient();

	virtual int run(uint64_t tick);
	virtual void on_recv_udp(const char* buf, ssize_t size, const struct sockaddr* addr);

	int prepare_req_conn(const sockaddr* addr, uv_udp_t* handle);
	void req_conn_run();
	
private:
	void on_recv_udp_snd_conv(const char* buf, ssize_t size);
	void send_ping();

private:
	uint64_t _next_ping_tick;

	int32_t _req_conn_times;
	uint64_t _next_req_conn_tick;
	hs_req_conn_s _req_conn;
};