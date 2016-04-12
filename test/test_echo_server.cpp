#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "kcpuv.h"

#if defined(PLATFORM_MACOS) || defined(PLATFORM_LINUX)
#	include <sys/time.h>
#	include <unistd.h>
#elif defined(PLATFORM_WINDOWS)
#	include <Windows.h>
#	ifdef _DEBUG
#		include <crtdbg.h>
#	endif
#endif

#if defined(PLATFORM_LINUX)
#	ifndef __STDC_FORMAT_MACROS
#		define __STDC_FORMAT_MACROS
#	endif
#endif

#include <inttypes.h>

static uint64_t get_tick_us() {
#if defined(PLATFORM_WINDOWS)
	FILETIME ft;
	ULARGE_INTEGER li;
	GetSystemTimeAsFileTime(&ft);
	li.LowPart  = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;
	return li.QuadPart / 10;
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (uint64_t)((uint64_t) tv.tv_sec * 1000000 + (uint64_t) tv.tv_usec);
#endif
}

static uint64_t get_tick_ms() {
	return get_tick_us() / 1000;
}

static void sleep_ms(uint64_t ms) {
#if defined(PLATFORM_WINDOWS)
	Sleep((DWORD)ms);
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
	usleep(ms * 1000);
#endif
}

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
