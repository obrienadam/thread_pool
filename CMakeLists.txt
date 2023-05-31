cmake_minimum_required(VERSION 3.9)
project(thread_pool)

set(CMAKE_CXX_STANDARD 17)

add_library(thread_pool thread_pool.h thread_pool.cc)
target_link_libraries(thread_pool pthread)

add_executable(thread_pool_test main.cc)
target_link_libraries(thread_pool_test thread_pool)
