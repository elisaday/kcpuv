#pragma once

#if defined(linux) || defined(__linux) || defined(__linux__)
# ifndef PLATFORM_LINUX
#   define PLATFORM_LINUX
# endif
#elif defined(WIN32) || defined(_WIN32) || defined(_WIN64)
# ifndef PLATFORM_WINDOWS
#   define PLATFORM_WINDOWS
# endif
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
# ifndef PLATFORM_MACOS
#   define PLATFORM_MACOS
# endif
#endif

#if defined(_MSC_VER)
# ifndef COMPILER_MSVC
#   define COMPILER_MSVC
# endif
#elif defined(__GNUC__)
# ifndef COMPILER_GCC
#   define COMPILER_GCC
# endif
#endif

#if defined(PLATFORM_WINDOWS)
# ifndef _CRT_SECURE_NO_WARNINGS
#   define _CRT_SECURE_NO_WARNINGS
# endif
# ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
# endif
# include <windows.h>
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS) || defined(PLATFORM_BSD)
# include <sys/time.h>
# include <sys/resource.h>
# include <unistd.h>
# include <fcntl.h>
#endif

// Standard ANSI include files
#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <float.h>
#include <math.h>
#include <signal.h>
#include <setjmp.h>
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <inttypes.h>

#ifdef __cplusplus
// STL header files
# include <string>
# include <fstream>
# include <map>
# include <vector>
# include <list>
# include <iostream>
# include <exception>
# include <deque>
# include <exception>
# include <algorithm>
# include <iterator>
# include <sstream>
# include <stdexcept>
#endif