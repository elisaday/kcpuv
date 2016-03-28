#include "header.h"
#include "tm.h"

uint64_t get_tick_ms() {
	return get_tick_us() / 1000;
}

uint64_t get_tick_us() {
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

void sleep(uint64_t ms) {
#if defined(PLATFORM_WINDOWS)
	Sleep((DWORD)ms);
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
	usleep(ms * 1000);
#endif
}