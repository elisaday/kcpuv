#include "test_common.cpp"
#include <map>

static const int MAX_MSG_SIZE = 1024 * 64;

struct MSG {
	uint64_t tick;
	char data[MAX_MSG_SIZE];
};

static std::map<uint64_t, MSG> snd_msg_map;

static int make_msg(uint64_t tick, MSG& msg, int size_min, int size_max) {
	msg.tick = tick;
	uint32_t size = (rand_u32() % (size_max - size_min + 1)) + size_min;
	for (uint32_t i = 0; i < size; ++i) {
		msg.data[i] = 'a' + (rand_u32() % 26);
	}
	return size;
}

int main(int argc, char* argv[]) {
#if defined(PLATFORM_WINDOWS) && defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(216);
#endif

	if (argc < 7) {
		printf("usage: test_delay_client <remote_ip> <remote_port> <duration second> <snd_interval_ms> <msg_size_min> <msg_size_max>");
		return -1;
	}

	char* remote_ip = argv[1];
	uint32_t remote_port = (uint32_t)atoi(argv[2]);
	uint64_t dur = (uint64_t)atoi(argv[3]);
	uint64_t snd_interval = (uint64_t)atoi(argv[4]);
	uint32_t msg_size_min = (uint32_t)atoi(argv[5]);
	uint32_t msg_size_max = (uint32_t)atoi(argv[6]);

	if (msg_size_max < msg_size_min) {
		printf("msg_size_max must greater than msg_size_min\n");
		return -2;
	}

	if (msg_size_max > MAX_MSG_SIZE) {
		printf("msg_size_max must less than 64kh\n");
		return -3;
	}

	kcpuv_t kcpuv = kcpuv_create();
	kcpuv_conv_t conv = kcpuv_connect(kcpuv, remote_ip, remote_port);
	uint64_t t = get_tick_ms();
	uint64_t nextSend = t + snd_interval;
	while (get_tick_ms() - t < dur) {
		kcpuv_run(kcpuv);

		uint64_t cur = get_tick_ms();
		if (cur > nextSend) {
			MSG& msg = snd_msg_map[cur];
			memset(&msg, 0, sizeof(msg));
			int size = make_msg(cur, msg, msg_size_min, msg_size_max);
			if (size > 0) {
				snd_msg_map[msg.tick] = msg;
				kcpuv_send(kcpuv, conv, &msg, sizeof(uint64_t) + size);
				nextSend = cur + snd_interval;
			}
		}

		kcpuv_msg_t msg;
		uint64_t now = get_tick_ms();
		while (true) {
			int r = kcpuv_recv(kcpuv, &msg);
			if (r < 0) break;

			MSG m;
			memset(&m, 0, sizeof(m));
			memcpy(&m, msg.data, msg.size < sizeof(m) ? msg.size : sizeof(m));

			std::map<uint64_t, MSG>::iterator it = snd_msg_map.find(m.tick);
			if (it == snd_msg_map.end()) {
				printf("msg not found %"PRIu64"\n", m.tick);
			} else {
				if (memcmp(&it->second, &m, sizeof(m)) == 0) {
					printf("%"PRIu64"\n", now - m.tick);
				} else {
					printf("recved a diff msg %"PRIu64"\n", m.tick);
				}
			}
		}

		sleep_ms(1);
	}

	kcpuv_destroy(kcpuv);
	return 0;
}
