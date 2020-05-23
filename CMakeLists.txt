cmake_minimum_required(VERSION 3.1)

project(image_processing CXX)
add_subdirectory(bitmap)
add_executable(main main.cpp)
target_link_libraries(main PRIVATE lib_bitmap)