#pragma once
#include "hand_shake.h"

class Network;

class Conn {
public:
	Conn(Network* network);
	~Conn();

	int init_kcp(kcpuv_conv_t conv);
	void shutdown();

	int send_kcp(const char* buf, uint32_t len);

	virtual int recv_kcp(char*& buf, uint32_t& size);

	virtual int run(uint64_t tick);
	virtual void on_recv_udp(const char* buf, ssize_t size, const struct sockaddr* addr);

	uint32_t status();
	
	int send_udp(const char* buf, uint32_t len);
	void send_ack_conv();

	kcpuv_conv_t get_conv();

	int expired();
	void alive();

protected:
	uint32_t new_key();
	uint32_t new_conv();

	int send_kcp_raw(const char* buf, uint32_t len);

protected:
	Network* _network;
	uv_udp_t* _udp;
	struct sockaddr _addr;
	
	kcpuv_conv_t _conv;
	ikcpcb* _kcp;
	uint32_t _key;

	uint32_t _status;

	uint64_t _conv_expired_tick;
};
