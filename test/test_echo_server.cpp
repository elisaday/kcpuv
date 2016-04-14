#include "test_common.cpp"

int main() {
#if defined(PLATFORM_WINDOWS) && defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(216);
#endif

	kcpuv_t kcpuv = kcpuv_create();
	kcpuv_listen(kcpuv, "0.0.0.0", 9527);
	uint64_t t = get_tick_ms();
	while (get_tick_ms() - t < 15000000) {
		kcpuv_run(kcpuv);

		kcpuv_msg_t msg;
		while (true) {
			int r = kcpuv_recv(kcpuv, &msg);
			if (r < 0) break;

			char buf[1024] = { 0 };
			strncpy(buf, (const char*)msg.data, msg.size);
			printf("conv: %d recv: %s\n", msg.conv, buf);
			strcpy(buf + msg.size, " - reply");
			kcpuv_send(kcpuv, msg.conv, buf, strlen(buf));
			kcpuv_msg_free(&msg);
		}
		sleep_ms(1);
	}
	kcpuv_destroy(kcpuv);
	return 0;
}
