#include "header.h"
#include "conn_server.h"
#include "tm.h"
#include "rand.h"

ConnServer::ConnServer(Network* network) 
	: Conn(network) {

}

ConnServer::~ConnServer() {

}

int ConnServer::expired() {
	return get_tick_ms() > _conv_expired_tick ? 0 : -1;
}

int ConnServer::run(uint64_t tick) {
	if (expired() == 0)
		return -1;

	int r = Conn::run(tick);
	if (_status == CONV_SND_CONV) {
		if (r == 0) {
			_status = CONV_ESTABLISHED;
		} else {
			snd_conn_run();
		}
		return 0;
	}

	return 0;
}

int ConnServer::prepare_snd_conv(const sockaddr* addr, uv_udp_t* handle, uint32_t n) {
	_addr = *addr;
	_udp = handle;
	_n = n;
	_conv = new_conv();

	_conv_expired_tick = get_tick_ms() + 1000 * 100000;

	_next_snd_conv_tick = 0;

	_snd_conv.conv = CONV_SND_CONV;
	_snd_conv.n = n;
	_snd_conv.new_conv = _conv;
	_snd_conv.key = new_key();

	_status = CONV_SND_CONV;
	return init_kcp(_conv);
}

void ConnServer::snd_conn_run() {
	uint64_t now = get_tick_ms();
	if (_status == CONV_SND_CONV) {
		if (now > _next_snd_conv_tick) {
			send_udp((const char*)&_snd_conv, sizeof(_snd_conv));
			_next_snd_conv_tick = now + 1000;
		}
	}
}

