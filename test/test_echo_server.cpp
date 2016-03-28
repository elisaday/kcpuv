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
	//_CrtSetBreakAlloc(100);
#endif

	kcpuv_t kcpuv = kcpuv_create();
	kcpuv_listen(kcpuv, "0.0.0.0", 9527);
	uint64_t t = GetTickCount();
	while (GetTickCount() - t < 15000) {
		kcpuv_run(kcpuv);

		kcpuv_msg_t msg;
		while (true) {
			int r = kcpuv_recv(kcpuv, &msg);
			if (r < 0) break;

			char buf[1024] = { 0 };
			strncpy(buf, (const char*)msg.data, msg.size);
			printf("recv: %s\n", buf);
			strcpy(buf + msg.size, " - reply");
			kcpuv_send(kcpuv, msg.conv, buf, strlen(buf));
			kcpuv_msg_free(&msg);
		}
		Sleep(1);
	}
	kcpuv_destroy(kcpuv);
	return 0;
}
