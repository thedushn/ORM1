cmake_minimum_required(VERSION 3.7)
project(ORM1C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -Wall")
set(SOURCE_FILES main.c file_handeling.c file_handeling.h)
add_executable(ORM1C ${SOURCE_FILES})