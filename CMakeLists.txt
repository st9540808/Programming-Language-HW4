cmake_minimum_required(VERSION 3.5)

project(matrix)

file(GLOB SOURCE main.c matrix.c)
file(GLOB HEADER matrix.h)
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -pthread")

add_executable(matrix ${SOURCE} ${HEADER})
