cmake_minimum_required(VERSION 3.17.2)

project(ThreadboxLib CXX ASM)

set(CMAKE_CXX_STANDARD 14)
add_compile_options(-Wall -Wextra -pedantic -Werror)

add_library(ThreadboxLib SHARED switch_stack_macos_x64.s Thread.cpp Threadbox.cpp)

target_include_directories(ThreadboxLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
