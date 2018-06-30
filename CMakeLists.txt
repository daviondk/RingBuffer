cmake_minimum_required(VERSION 3.10)
project(RingBuffer)

set(CMAKE_CXX_STANDARD 17)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -pedantic -g")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -D_GLIBCXX_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3")


add_executable(RingBuffer RingBuffer.h gtest/gtest_main.cc gtest/gtest-all.cc gtest/gtest.h deque_testing.cpp fault_injection.cpp fault_injection.h counted.cpp counted.h)


target_link_libraries(RingBuffer -lpthread)
