#include "header.h"
#include "rand.h"

void rand_seed(uint32_t seed) {
#if defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
	srandom((unsigned int)seed);
#elif defined(PLATFORM_WINDOWS)
	srand((unsigned int)seed);
#endif
}

uint32_t rand_uint32() {
#if defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
	return random();
#elif defined(PLATFORM_WINDOWS)
	return rand();
#endif
}