cmake_minimum_required(VERSION 3.7)
project(ORM1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -Wall  ")
set(CMAKE_C_STANDARD 11)

#SET(CMAKE_C_COMPILER      /home/dushn/toolchains/tegra-4.8.1-nv/usr/bin/arm-cortex_a15-linux-gnueabi/arm-cortex_a15-linux-gnueabi-gcc)
set(SOURCE_FILES main.c file_handeling.c file_handeling.h)
add_executable(ORM1 ${SOURCE_FILES})
target_link_libraries(ORM1 m)