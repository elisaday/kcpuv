#pragma once

#include "uv.h"

#include "platform.h"
#include "err.h"
#include "log.h"

#if defined(PLATFORM_WINDOWS)
#   include <winsock2.h>
#   include <windows.h>
#   pragma comment(lib ,"ws2_32.lib")
#   pragma comment(lib, "psapi.lib")
#   pragma comment(lib, "Iphlpapi.lib")
#   pragma comment(lib, "userenv.lib")
#elif defined(PLATFORM_LINUX)
#   include <sys/types.h>
#   include <sys/socket.h>
#   include <arpa/inet.h>
#   include <netinet/in.h>
#   include <netinet/tcp.h>
#   include <sys/epoll.h>
#elif defined(PLATFORM_MACOS) || defined(PLATFORM_BSD)
# include <sys/event.h>
#   include <sys/socket.h>
#   include <arpa/inet.h>
#   include <sys/types.h>
#   include <sys/event.h>
#   include <sys/time.h>
#endif

#include "ikcp.h"
#include "kcpuv.h"

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include<crtdbg.h>
#endif