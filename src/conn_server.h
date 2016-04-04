#pragma once
#include "conn.h"

class ConnServer : public Conn {
public:
	ConnServer(Network* network);
	~ConnServer();

	virtual int run(uint64_t tick);

	int prepare_snd_conv(const sockaddr* addr, uv_udp_t* handle, uint32_t n);
	void snd_conn_run();
	int expired();

private:
	uint32_t _n;
	uint64_t _next_snd_conv_tick;
	hs_snd_conv_s _snd_conv;

	uint64_t _conv_expired_tick;
};