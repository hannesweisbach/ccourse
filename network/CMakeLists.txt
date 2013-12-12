add_library(network network-common.c)
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set_target_properties(network PROPERTIES COMPILE_DEFINITIONS "_BSD_SOURCE;_POSIX_C_SOURCE")
endif()
add_executable(echo-server echo-server.c)
target_link_libraries(echo-server network)
add_executable(echo-client echo-client.c)
target_link_libraries(echo-client network)

