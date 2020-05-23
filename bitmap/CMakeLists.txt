cmake_minimum_required(VERSION 3.1)

project(bitmap)
add_library(lib_bitmap STATIC ./src/iobitmap.cpp ./src/process.cpp)

target_compile_features(lib_bitmap PRIVATE cxx_std_17)
target_include_directories(lib_bitmap PUBLIC ./include)
