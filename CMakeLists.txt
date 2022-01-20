cmake_minimum_required(VERSION 3.21)
project(283_kitchen)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc")

add_executable(283_kitchen main.cpp)
