cmake_minimum_required(VERSION 2.6)

project(kcpuv)

file(GLOB SRC_ROOT RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h *.cpp *.c)
file(GLOB INTERFACE RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ../inc/*.h)
file(GLOB KCP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ../kcp/*.c ../kcp/*.h)

source_group(src FILES ${SRC_ROOT})
source_group(interface FILES ${INTERFACE})
source_group(kcp FILES ${KCP})

set(SRC ${SRC_ROOT} ${INTERFACE} ${KCP})

add_library(kcpuv STATIC ${SRC})

if (MSVC)
  set_source_files_properties (header.cpp PROPERTIES COMPILE_FLAGS "/Ycheader.h")
  set_target_properties (kcpuv PROPERTIES COMPILE_FLAGS "/Yuheader.h")
  set_source_files_properties (../kcp/ikcp.c PROPERTIES COMPILE_FLAGS "/Y-")

  if (USING_STATIC_CRT)
    foreach (flag_var
      CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
      CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO
      CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
      CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)

      if (${flag_var} MATCHES "/MD")
        string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
      endif (${flag_var} MATCHES "/MD")

      if (${flag_var} MATCHES "/MDd")
        string(REGEX REPLACE "/MDd" "/MTd" ${flag_var} "${${flag_var}}")
      endif (${flag_var} MATCHES "/MDd")           
    endforeach (flag_var)
  endif ()   
endif (MSVC)

if (UNIX)
	if (USING_FPIC_FLAG)
		set_target_properties(kcpuv PROPERTIES COMPILE_FLAGS -fPIC)
	endif ()
endif ()

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/../inc/
  ${CMAKE_CURRENT_SOURCE_DIR}/../libuv/inc/
  ${CMAKE_CURRENT_SOURCE_DIR}/../kcp/
)

