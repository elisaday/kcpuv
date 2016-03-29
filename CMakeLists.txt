cmake_minimum_required (VERSION 2.6)

project(kcpuv)

option(USING_STATIC_CRT "" NO)
option(USING_FPIC_FLAG "whether user -fPIC flag" NO)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
	set(CMAKE_BUILD_TYPE Release CACHE filepath "")
endif ()

add_subdirectory(src)
add_subdirectory(test)