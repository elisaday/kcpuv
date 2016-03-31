cmake_minimum_required(VERSION 2.6)

if (MSVC)
  link_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../libuv/lib/x64/)
elseif (APPLE)
  link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../libuv/lib/macos_x64_i386/
  )
elseif (UNIX)
  link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../libuv/lib/linux_x64/
  )
endif ()

add_executable(test_echo_client "test_echo_client.cpp")
add_executable(test_echo_server "test_echo_server.cpp")

if (MSVC)
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

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/../inc/
)

target_link_libraries(test_echo_client kcpuv pthread)
target_link_libraries(test_echo_server kcpuv pthread)
