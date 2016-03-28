#pragma once

class Network;

class Conn {
public:
	Conn(Network* network);
	~Conn();

	int init(kcpuv_conv_t conv, const sockaddr* addr, uv_udp_t* handle);
	void shutdown();

	int send_kcp(const char* buf, uint32_t len);
	int recv_kcp(char*& buf, uint32_t& size);
	int run(uint64_t tick);

public:
	void on_recv_udp(const char* buf, ssize_t size, const struct sockaddr* addr);
	int send_udp(const char* buf, uint32_t len);

private:
	Network* _network;
	kcpuv_conv_t _conv;
	uv_udp_t* _udp;
	struct sockaddr _addr;
	ikcpcb* _kcp;
};
