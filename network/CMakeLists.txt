add_library(network network-common.c)
add_executable(echo-server echo-server.c)
target_link_libraries(echo-server network)
add_executable(echo-client echo-client.c)
target_link_libraries(echo-client network)

