cmake_minimum_required(VERSION 3.13)

include("$ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake")
project(cw1031 C CXX ASM)

pico_sdk_init()

add_executable(cw1031 morse_code.c)
target_link_libraries(cw1031 pico_stdlib)

