#ifndef __H_KCPUV__
#define __H_KCPUV__

#if defined(linux) || defined(__linux) || defined(__linux__)
#   ifndef PLATFORM_LINUX
#       define PLATFORM_LINUX
#   endif
#elif defined(WIN32) || defined(_WIN32) || defined(_WIN64)
#   ifndef PLATFORM_WINDOWS
#       define PLATFORM_WINDOWS
#   endif
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
#   ifndef PLATFORM_MACOS
#       define PLATFORM_MACOS
#   endif
#else
#   error Unsupported platform.
#endif

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifdef KCPUV_DLL
#   if defined(PLATFORM_WINDOWS)
#      ifdef KCPUV_EXPORT
#           define KCPUV_API __declspec(dllexport)
#       else
#           define KCPUV_API __declspec(dllimport)
#       endif
#   else
#       define KCPUV_API __attribute ((visibility("default")))
#   endif
#else
#   define KCPUV_API
#endif

struct kcpuv_s;
typedef kcpuv_s* kcpuv_t;

typedef uint32_t kcpuv_conv_t;

typedef struct kcpuv_msg_t {
	kcpuv_conv_t conv;
	uint8_t* data;
	uint32_t size;
} kcpuv_msg_t;

KCPUV_API kcpuv_t kcpuv_create();
KCPUV_API void kcpuv_destroy(kcpuv_t kcpuv);

KCPUV_API int kcpuv_listen(kcpuv_t kcpuv, const char* addr, uint32_t port);
KCPUV_API kcpuv_conv_t kcpuv_connect(kcpuv_t kcpuv, const char* addr, uint32_t port);

KCPUV_API void kcpuv_run(kcpuv_t kcpuv);

KCPUV_API int kcpuv_recv(kcpuv_t kcpuv, kcpuv_msg_t* msg);
KCPUV_API int kcpuv_send(kcpuv_t kcpuv, kcpuv_conv_t conv, const void* data, uint32_t size);

KCPUV_API void kcpuv_msg_free(kcpuv_msg_t* msg);

KCPUV_API int kcpuv_conv_valid(kcpuv_t kcpuv, kcpuv_conv_t conv);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __H_KCPUV__
