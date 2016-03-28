#include <Windows.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#ifdef _DEBUG
#include <crtdbg.h>
#endif
#include "kcpuv.h"

int main() {

#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(216);
#endif

	kcpuv_conv_t  conv = 0x12345678;
	kcpuv_t kcpuv = kcpuv_create();
	kcpuv_connect(kcpuv, conv, "127.0.0.1", 9527);
	uint64_t t = GetTickCount();
	uint64_t nextSend = t + 1000;
	while (GetTickCount() - t < 15000) {
		kcpuv_run(kcpuv);

		kcpuv_msg_t msg;
		while (true) {
			int r = kcpuv_recv(kcpuv, &msg);
			if (r < 0) break;

			char buf[1024] = { 0 };
			strncpy(buf, (const char*)msg.data, msg.size);
			printf("recv: %s\n", buf);
			kcpuv_msg_free(&msg);
		}

		uint64_t cur = GetTickCount();
		if (cur > nextSend) {
			char xxx[1024];
			sprintf(xxx, "tick - %"PRIu64, cur);
			kcpuv_send(kcpuv, conv, xxx, strlen(xxx));
			nextSend = cur + 1000;
		}
		Sleep(1);
	}
	kcpuv_destroy(kcpuv);
	return 0;
}
