cmake_minimum_required(VERSION 3.5)

project(matrix)

file(GLOB SOURCE *.c)
file(GLOB HEADER *.h)
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -pthread -g -ggdb3 -O3")
MESSAGE(STATUS "CMAKE_C_FLAGS: " ${CMAKE_C_FLAGS})

add_executable(matrix ${SOURCE} ${HEADER})
