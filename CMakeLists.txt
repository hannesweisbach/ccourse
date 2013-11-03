cmake_minimum_required(VERSION 2.8)

project(c-course)

if(NOT ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR
        "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"))
  message(FATAL_ERROR "This project is only for clang or gcc")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wextra -pedantic")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

add_subdirectory(introduction)
